#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include <zephyr/sys/printk.h>   // For printing messages to the console
#include <zephyr/sys/util.h>
#include <zephyr/kernel.h>
#include <zephyr/stats/stats.h>
#include <zephyr/debug/thread_analyzer.h>
#include <zephyr/sys/printk.h>
#include <zephyr/arch/cpu.h>

#include <zephyr/kernel.h>
#include <zephyr/stats/stats.h>
#include <zephyr/debug/thread_analyzer.h>
#include <zephyr/sys/printk.h>
#include <zephyr/arch/cpu.h>
#include <zephyr/sys/atomic.h>

#define BALANCER_INTERVAL_MS  100
#define LOAD_THRESHOLD_HIGH   75  // %
#define LOAD_THRESHOLD_LOW    25  // %
#define MAX_THREAD_NAME_LEN   32

K_THREAD_STACK_DEFINE(balancer_stack, 1024);
static struct k_thread balancer_thread;

/* Per-core load tracking */
struct cpu_load {
    uint64_t idle_count;
    uint64_t last_idle_count;
    uint8_t utilization;
};
static struct cpu_load loads[CONFIG_MP_MAX_NUM_CPUS];

/* Thread migration control */
static bool allow_migration = true;
K_MUTEX_DEFINE(balancer_mutex);

/* CPU utilization calculation */
static void update_cpu_load(void)
{
    for (int i = 0; i < arch_num_cpus(); i++) {
        struct k_thread *idle = _kernel.cpus[i].idle_thread;

        /* Get idle count from idle thread */
        uint64_t new_idle = (uint64_t)idle->base.user_data;
        uint64_t delta_idle = new_idle - loads[i].last_idle_count;

        /* Calculate utilization */
        uint64_t total_cycles = BALANCER_INTERVAL_MS * sys_clock_hw_cycles_per_sec() / 1000;
        if (total_cycles > 0) {
            uint8_t utilization = 100 - (100 * delta_idle) / total_cycles;
            loads[i].utilization = utilization;
        }

        loads[i].last_idle_count = new_idle;
    }
}

/* Idle thread hook to count idle cycles */
void idle_thread_hook(void)
{
    struct k_thread *idle = _current;
    uint64_t count = (uint64_t)idle->base.user_data;
    idle->base.user_data = (void *)(count + 1);
}

/* Callback for thread iteration */
struct balance_data {
    int max_cpu;
    struct k_thread **target;
    uint64_t *max_count;
};

static void thread_analyze_cb(const struct k_thread *thread, void *user_data)
{
    struct balance_data *data = (struct balance_data *)user_data;

    /* Skip system threads */
    if ((thread->base.user_options & K_ESSENTIAL) ||
        (thread == _kernel.cpus[data->max_cpu].idle_thread) ||
        (thread == k_current_get())) {
        return;
    }

    /* Check if on busy core */
    #ifdef CONFIG_SMP
    if (thread->base.cpu == data->max_cpu) {
    #else
    if (arch_curr_cpu() == data->max_cpu) {
    #endif
        /* Use simple run count for migration decision */
        uint64_t run_count = (uint64_t)thread->base.user_data;

        /* Find most executed thread */
        if (run_count > *(data->max_count)) {
            *(data->target) = (struct k_thread *)thread;
            *(data->max_count) = run_count;
        }
    }
}

/* Thread migration logic */
static void balance_load(void)
{
    uint8_t max_load = 0, min_load = 100;
    int max_cpu = -1, min_cpu = -1;

    /* Find most/least loaded cores */
    for (int i = 0; i < arch_num_cpus(); i++) {
        if (loads[i].utilization > max_load) {
            max_load = loads[i].utilization;
            max_cpu = i;
        }
        if (loads[i].utilization < min_load) {
            min_load = loads[i].utilization;
            min_cpu = i;
        }
    }

    /* Check if balancing needed */
    if (max_cpu == min_cpu || max_cpu < 0 || min_cpu < 0 ||
        max_load < LOAD_THRESHOLD_HIGH || min_load > LOAD_THRESHOLD_LOW) {
        return;
    }

    /* Find migratable thread on busy core */
    struct k_thread *target = NULL;
    uint64_t max_count = 0;

    struct balance_data data = {
        .max_cpu = max_cpu,
        .target = &target,
        .max_count = &max_count
    };

    k_thread_foreach(thread_analyze_cb, &data);

    /* Migrate thread */
    if (target != NULL) {
        #ifdef CONFIG_SCHED_CPU_MASK
        k_thread_cpu_mask_clear(target, max_cpu);
        k_thread_cpu_mask_enable(target, min_cpu);
        #else
        k_thread_cpu_pin(target, min_cpu);
        #endif

        printk("Migrated %s: CPU%d -> CPU%d\n",
               target->name, max_cpu, min_cpu);
    }
}

/* Thread run counter hook */
void thread_run_hook(struct k_thread *thread)
{
    uint64_t count = (uint64_t)thread->base.user_data;
    thread->base.user_data = (void *)(count + 1);
}

/* Balancer thread entry point */
static void balancer_thread_entry(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    while (true) {
        k_msleep(BALANCER_INTERVAL_MS);

        k_mutex_lock(&balancer_mutex, K_FOREVER);
        if (allow_migration) {
            update_cpu_load();
            balance_load();
        }
        k_mutex_unlock(&balancer_mutex);
    }
}

/* Public API */
void balancer_init(void)
{
    /* Set up idle thread hooks */
    for (int i = 0; i < arch_num_cpus(); i++) {
        _kernel.cpus[i].idle_thread->base.user_data = (void *)0;
    }

    /* Create balancer thread */
    k_thread_create(&balancer_thread,
                   balancer_stack,
                   K_THREAD_STACK_SIZEOF(balancer_stack),
                   balancer_thread_entry,
                   NULL, NULL, NULL,
                   K_PRIO_PREEMPT(1), 0, K_NO_WAIT);
    k_thread_name_set(&balancer_thread, "load_balancer");
}

void balancer_pause(void)
{
    k_mutex_lock(&balancer_mutex, K_FOREVER);
    allow_migration = false;
    k_mutex_unlock(&balancer_mutex);
}

void balancer_resume(void)
{
    k_mutex_lock(&balancer_mutex, K_FOREVER);
    allow_migration = true;
    k_mutex_unlock(&balancer_mutex);
}
