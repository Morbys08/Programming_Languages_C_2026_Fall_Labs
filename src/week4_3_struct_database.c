/*
 * week4_3_struct_database.c
 * Author: [Alisultan Dadashov]
 * Student ID: [251ADB251]
 * Description:
 *   Simple in-memory "database" using an array of structs.
 *   Use malloc to allocate space for n Student records,
 *   read each record from the user, print them as a table,
 *   and then free the memory.
 *
 *   Output must match the format in the Week 4 instructions exactly
 *   (it is checked by the autograder).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO: Define struct Student with fields name (char[50]), id (int), grade (float)
struct Student {
    char name [50];
    int id;
    float grade;
};
//       (same definition as in Task 2)

int main(void) {
    int n;
    struct Student *students = NULL;
 
    printf("Enter number of students: ");
    if (scanf("%d", &n) != 1 || n <= 0) {
        printf("Invalid number.\n");
        return 1;
    }

    // TODO: Allocate memory for n Student structs using malloc
    students = (struct Student *)malloc((size_t)n * sizeof(struct Student));
    if (students == NULL){
        printf("Memory allocation failed.\n");
        return 1;
    }
    //       Example: students = malloc(n * sizeof(struct Student));

    // TODO: Check allocation success
    // If students is NULL: print "Memory allocation failed." and return 1

    // TODO: Read student data in a loop. For student i (counting from 1):
    for (int i = 0; i < n; i++) {
        printf("Enter data for student %d: ", i + 1);
        if (scanf("%49s %d %f", students[i].name, &students[i].id, &students[i].grade) != 3) {
            printf("Invalid input.\n");
            free(students);
            return 1;
        }
    }

    printf("\n");
    
    printf("%-6s %-11s %s\n", "ID", "Name", "Grade");
    //       print "Enter data for student %d: ", then read
    //       name (scanf("%49s", ...)), id and grade.
    //       If a value cannot be read: print "Invalid input.",
    //       free the array and return 1

    // TODO: Print an empty line, then the table:
    for (int i = 0; i < n; i++) {
        printf("%-6d %-11s %.1f\n", students[i].id, students[i].name, students[i].grade);
    }

    //       printf("%-6s %-11s %s\n", "ID", "Name", "Grade");
    //       and for each student:
    //       printf("%-6d %-11s %.1f\n", id, name, grade);

    // Optional (not autograded): after the table, print the average
    // grade or the top student

    // TODO: Free allocated memory
    free(students);  // remove this line once you use students

    return 0;
}
