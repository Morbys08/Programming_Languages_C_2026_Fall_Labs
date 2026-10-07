/*
 * week4_2_struct_student.c
 * Author: [Alisultan Dadashov]
 * Student ID: [251ADB251]
 * Description:
 *   Demonstrates defining and using a struct in C.
 *   Define a 'Student' struct with name, id and grade, create two
 *   instances with the values from the instructions, and print them.
 *
 *   This program reads no input. Output must match the format in the
 *   Week 4 instructions exactly (it is checked by the autograder).
 */

#include <stdio.h>
#include <string.h>

// TODO: Define struct Student with fields: name (char[50]), id (int), grade (float)
// Example:
struct Student {
    char name [50];
    int id;
    float grade;
};
// struct Student {
//     char name[50];
//     int id;
//     float grade;
// };

int main(void) {
    struct Student s1;
    struct Student s2;
    // TODO: Declare two Student variables

    // TODO: Assign the values (use strcpy for the name):
    //       Student 1: Alice Johnson, 1001, 9.1
    strcpy(s1.name, "Alice Johnson");
    s1.id = 1001;
    s1.grade = 9.1;
    //       Student 2: Bob Smith,     1002, 8.7
    strcpy(s2.name, "Bob Smith");
    s2.id = 1002;
    s2.grade = 8.7;

    // TODO: Print each student exactly as:
    printf("Student 1: %s, ID: %d, Grade: %.1f\n", s1.name, s1.id, s1.grade);
    printf("Student 2: %s, ID: %d, Grade: %.1f\n", s2.name, s2.id, s2.grade);
    //       Student <k>: <name>, ID: <id>, Grade: <grade with 1 decimal, %.1f>

    return 0;
}
