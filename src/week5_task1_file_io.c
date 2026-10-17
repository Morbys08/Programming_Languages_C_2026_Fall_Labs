// week5_task1_file_io.c
// Name: ________  Surname: ________  Student ID: ________
//
// Task 1: Read and write data from text files
// Week 5 – Files & Modular Programming
// TODO: Fill in the missing parts marked below.
//
// Build: make lab5   Run: ./bin/week5_task1_file_io

#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_FILENAME "data.txt"
#define LINE_LEN 256

int main(int argc, char* argv[]) {
  FILE* fp = NULL;
  const char* filename = DEFAULT_FILENAME;
  char line[LINE_LEN];
  int line_count = 0;

  // BONUS: if a file name was given on the command line (argc > 1), use argv[1]
  // instead.

  // TODO: 1. Print "Writing lines to <filename>..." and open the file for
  // writing (mode "w")
  printf("Writing lines to %s...\n", filename);
  fp = fopen(filename, "w");
  // TODO: 2. Check that fp is not NULL. If it is: perror(filename); return
  // EXIT_FAILURE;
  if (fp == NULL) {
    perror(filename);
    return EXIT_FAILURE;
  }
  // TODO: 3. Write 2–3 lines of text to the file using fprintf()
  fprintf(fp, "Hello, file I/0 in C!\n");
  fprintf(fp, "This is another line. \n");
  // TODO: 4. Close the file with fclose()

  fclose(fp);
  // TODO: 5. Open the file again for reading (mode "r") and check for NULL
  // again

  fp = fopen(filename, "r");
  if (fp == NULL) {
    perror(filename);
    return EXIT_FAILURE;
  }
  // TODO: 6. Print "Reading contents:" and use fgets() in a loop to print each
  // line
  printf("Reading contents: \n");
  while (fgets(line, sizeof(line), fp) != NULL) {
    printf("%s", line);
    line_count++;
  }
  // TODO: 7. Close the file
  fclose(fp);

  printf("Lines read: %d\n", line_count);

  // BONUS: printf("Lines read: %d\n", line_count);

  // Remove this line once you use these variables:

  return EXIT_SUCCESS;
}
