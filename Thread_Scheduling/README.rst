This demo showcases how threads transition through different states in Zephyr:

Sleeping → via k_sleep()

Pending (Blocked) → via k_sem_take()

Suspended → via k_thread_suspend()

Ready/Running → upon resume or resource availability

These transitions are orchestrated by a dedicated controller thread to visualize how the RTOS scheduler responds.

🧵 Thread Roles
✅ 1. Sleeper Thread (sleeper_thread)
Simulates a task that performs periodic work and sleeps in between.

Uses k_sleep(K_SECONDS(2)) to enter the sleeping state.

Can be externally suspended and resumed by another thread (k_thread_suspend, k_thread_resume).

✅ 2. Pending Thread (pending_thread)
Waits on a semaphore using k_sem_take() with K_FOREVER.

This places the thread in the pending state until the semaphore is given by the controller.

Simulates a task waiting for hardware or resource availability.

✅ 3. Controller Thread (controller_thread)
Manages the state of sleeper and pending threads.

Suspends/resumes the sleeper thread explicitly.

Gives the semaphore to unblock the pending thread.

Runs periodically to cycle through control logic.

📦 Key Kernel Concepts Used
Concept	Usage Purpose
k_sleep()	Moves thread to sleeping state for a duration
k_thread_suspend()	Moves thread to suspended state (manual halt)
k_thread_resume()	Moves suspended thread back to ready state
k_sem_take()	Blocks thread until a semaphore is given (pending)
k_sem_give()	Unblocks a thread waiting on semaphore
k_thread_create()	Creates threads dynamically
k_tid_t	Thread ID handle, used for controlling other threads
🔄 Thread State Transition Flow
plaintext
[Sleeper] --> sleeping via k_sleep()
            ↕ (Controller can suspend/resume)

[Pending] --> pending via k_sem_take()
            ↕ (Controller gives semaphore)

[Controller] --> runs all the time to control others
This design visually reveals the nature of how real-time operating systems handle thread scheduling and resource synchronization.

💡 Use Case Simulation
You could repurpose this model to simulate:

ISR-triggered wake-ups

Low-power suspended peripherals

Task-to-task dependency chains

Debugging RTOS scheduling behavior
