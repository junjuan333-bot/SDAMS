#ifndef INSTRUCTOR_H
#define INSTRUCTOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_ID_LEN 20
#define MAX_NAME_LEN 50
#define MAX_DESC_LEN 100
#define MAX_DATE_LEN 20
#define MAX_TIME_LEN 10
#define MAX_STATUS_LEN 20

#define INSTRUCTOR_FILE "instructors.txt"
#define CLASS_FILE "classes.txt"
#define PROBLEM_FILE "facility_problems.txt"
#define RATING_FILE "ratings.txt"
#define TEMP_FILE "temp_instructor.txt"

void instructor_login_portal(void);
void instructor_main_menu(const char currentId[], const char currentName[]);

void class_schedule_menu(const char currentId[]);
void add_class_schedule(const char currentId[]);
void view_class_schedules(const char currentId[]);
void search_class_schedule(const char currentId[]);
void update_class_schedule(const char currentId[]);

void facility_problem_menu(const char currentId[], const char currentName[]);
void add_facility_problem(const char currentId[], const char currentName[]);
void view_facility_problems(void);

void instructor_rating_menu(const char currentId[]);
void view_instructor_ratings(const char currentId[]);
void search_instructor_ratings(const char currentId[]);

void inst_clear_buffer(void);
void inst_trim_spaces(char str[]);
int inst_equals_ignore_case(const char s1[], const char s2[]);
int inst_contains_ignore_case(const char haystack[], const char needle[]);

#endif
