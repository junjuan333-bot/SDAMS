#ifndef INSTRUCTOR_H
#define INSTRUCTOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Public interfaces called by mainMenu.c */
int instructorLogin(char currentId[], char currentName[]);
void instructorMenu(const char currentId[], const char currentName[]);

#endif /* INSTRUCTOR_H */
