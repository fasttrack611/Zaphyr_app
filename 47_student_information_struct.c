// C Program to Store
// Information about Students
// Using Structure
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Create the student structure
struct Student {
    char* name;
    int roll_number;
    int age;
};

// Driver code
int main()
{
    int n = 3;

    // Create the student's structure variable
    // with n Student's records
    struct Student student[n];

    // Get the students data
    student[0].roll_number = 1;
    student[0].name = "Geeks1";
    student[0].age = 10;

    student[1].roll_number = 2;
    student[1].name = "Geeks2";
    student[1].age = 11;

    student[2].roll_number = 3;
    student[2].name = "Geeks3";
    student[2].age = 13;

    // Printing  the Structers
    printf("Student Records:\n\n");
    for (int i = 0; i < n; i++) {
        printf("\tName : %s", student[i].name);
        printf("\tRoll Number : %d",
               student[i].roll_number);
        printf("\tAge : %d\n", student[i].age);
    }

    return 0;
}
