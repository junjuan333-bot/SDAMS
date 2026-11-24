/*
 * File: instructor_classes.c
 * Module: Instructor Module - Class Schedule Management
 * Description: Implements Add, View, Search, and Update operations for classes.txt.
 */
#include "instructor.h"

struct ClassSchedule {
    char classId[MAX_ID_LEN];
    char className[MAX_NAME_LEN];
    char instructorId[MAX_ID_LEN];
    char date[MAX_DATE_LEN];
    char time[MAX_TIME_LEN];
};

union ClassSearchFilter {
    char keyword[MAX_NAME_LEN];
};

static void generate_next_class_id(char nextId[]) {
    FILE *fp = fopen(CLASS_FILE, "r");
    struct ClassSchedule temp;
    int maxNum = 100;
    int num = 0;

    if (fp != NULL) {
        while (fscanf(fp, "%19[^,],%49[^,],%19[^,],%19[^,],%9[^\n]\n",
                      temp.classId, temp.className, temp.instructorId, temp.date, temp.time) == 5) {
            if (sscanf(temp.classId, "C%d", &num) == 1 || sscanf(temp.classId, "c%d", &num) == 1) {
                if (num > maxNum) {
                    maxNum = num;
                }
            }
        }
        fclose(fp);
    }
    sprintf(nextId, "C%03d", maxNum + 1);
}

static int is_valid_date_format(const char date[]) {
    int y, m, d;
    if (strlen(date) != 10 || date[4] != '-' || date[7] != '-') return 0;
    if (sscanf(date, "%4d-%2d-%2d", &y, &m, &d) != 3) return 0;
    if (y < 2020 || y > 2100 || m < 1 || m > 12 || d < 1 || d > 31) return 0;
    return 1;
}

static int is_valid_time_format(const char timeStr[]) {
    int h, m;
    if (strlen(timeStr) != 5 || timeStr[2] != ':') return 0;
    if (sscanf(timeStr, "%2d:%2d", &h, &m) != 2) return 0;
    if (h < 0 || h > 23 || m < 0 || m > 59) return 0;
    return 1;
}

void class_schedule_menu(const char currentId[]) {
    int choice;

    do {
        printf("\n--------------------------------------------\n");
        printf("       CLASS SCHEDULE MANAGEMENT (%s)\n", currentId);
        printf("--------------------------------------------\n");
        printf("1. Add New Class Schedule\n");
        printf("2. View Class Schedules\n");
        printf("3. Search Class Schedule\n");
        printf("4. Update Class Schedule\n");
        printf("0. Back to Instructor Menu\n");
        printf("--------------------------------------------\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            inst_clear_buffer();
            choice = -1;
        } else {
            inst_clear_buffer();
        }

        switch (choice) {
            case 1:
                add_class_schedule(currentId);
                break;
            case 2:
                view_class_schedules(currentId);
                break;
            case 3:
                search_class_schedule(currentId);
                break;
            case 4:
                update_class_schedule(currentId);
                break;
            case 0:
                break;
            default:
                printf("\n[Invalid] Please enter 0-4.\n");
        }
    } while (choice != 0);
}

void add_class_schedule(const char currentId[]) {
    struct ClassSchedule newClass;
    FILE *fp;

    generate_next_class_id(newClass.classId);
    strcpy(newClass.instructorId, currentId);

    printf("\n--- Add New Class Schedule (Auto ID: %s) ---\n", newClass.classId);

    while (1) {
        printf("Enter Class Name (no commas, or 0 to cancel): ");
        if (scanf("%49[^\n]", newClass.className) != 1) {
            inst_clear_buffer();
            continue;
        }
        inst_clear_buffer();
        inst_trim_spaces(newClass.className);

        if (strcmp(newClass.className, "0") == 0) {
            printf("Operation cancelled.\n");
            return;
        }
        if (strlen(newClass.className) == 0 || strchr(newClass.className, ',') != NULL) {
            printf("[Error] Class name cannot be empty or contain commas (',').\n");
            continue;
        }
        break;
    }

    while (1) {
        printf("Enter Date (YYYY-MM-DD, e.g. 2026-11-28): ");
        if (scanf("%19[^\n]", newClass.date) != 1) {
            inst_clear_buffer();
            continue;
        }
        inst_clear_buffer();
        inst_trim_spaces(newClass.date);

        if (!is_valid_date_format(newClass.date)) {
            printf("[Error] Invalid date format. Use YYYY-MM-DD.\n");
            continue;
        }
        break;
    }

    while (1) {
        printf("Enter Time (HH:MM 24-hour, e.g. 18:00): ");
        if (scanf("%9[^\n]", newClass.time) != 1) {
            inst_clear_buffer();
            continue;
        }
        inst_clear_buffer();
        inst_trim_spaces(newClass.time);

        if (!is_valid_time_format(newClass.time)) {
            printf("[Error] Invalid time format. Use HH:MM.\n");
            continue;
        }
        break;
    }

    fp = fopen(CLASS_FILE, "a");
    if (fp == NULL) {
        printf("[Error] Unable to open %s for writing.\n", CLASS_FILE);
        return;
    }

    fprintf(fp, "%s,%s,%s,%s,%s\n",
            newClass.classId, newClass.className, newClass.instructorId, newClass.date, newClass.time);
    fclose(fp);

    printf("[OK] Class %s (%s) scheduled on %s at %s!\n",
           newClass.classId, newClass.className, newClass.date, newClass.time);
}

void view_class_schedules(const char currentId[]) {
    FILE *fp = fopen(CLASS_FILE, "r");
    struct ClassSchedule cls;
    int mode = 1;
    int count = 0;

    if (fp == NULL) {
        printf("\n[Info] No class schedule records found (%s missing).\n", CLASS_FILE);
        return;
    }

    printf("\nView Option: [1] My Classes Only (%s)  [2] All Academy Classes: ", currentId);
    if (scanf("%d", &mode) != 1) {
        inst_clear_buffer();
        mode = 1;
    } else {
        inst_clear_buffer();
    }

    printf("\n===========================================================================\n");
    printf("%-10s | %-25s | %-13s | %-12s | %-8s\n",
           "Class ID", "Class Name", "Instructor ID", "Date", "Time");
    printf("---------------------------------------------------------------------------\n");

    while (fscanf(fp, "%19[^,],%49[^,],%19[^,],%19[^,],%9[^\n]\n",
                  cls.classId, cls.className, cls.instructorId, cls.date, cls.time) == 5) {
        if (mode == 1 && !inst_equals_ignore_case(cls.instructorId, currentId)) {
            continue;
        }
        printf("%-10s | %-25s | %-13s | %-12s | %-8s\n",
               cls.classId, cls.className, cls.instructorId, cls.date, cls.time);
        count++;
    }

    printf("===========================================================================\n");
    printf("Total Displayed Classes: %d\n", count);
    fclose(fp);
}

void search_class_schedule(const char currentId[]) {
    FILE *fp = fopen(CLASS_FILE, "r");
    struct ClassSchedule cls;
    union SearchFilter filter;
    int searchBy;
    int found = 0;

    (void)currentId;

    if (fp == NULL) {
        printf("\n[Error] Cannot open %s.\n", CLASS_FILE);
        return;
    }

    printf("\nSearch Class By: [1] Class ID  [2] Class Name  [3] Date (YYYY-MM-DD): ");
    if (scanf("%d", &searchBy) != 1 || searchBy < 1 || searchBy > 3) {
        inst_clear_buffer();
        printf("[Invalid] Search cancelled.\n");
        fclose(fp);
        return;
    }
    inst_clear_buffer();

    printf("Enter search keyword: ");
    if (scanf("%49[^\n]", filter.keyword) != 1) {
        inst_clear_buffer();
        fclose(fp);
        return;
    }
    inst_clear_buffer();
    inst_trim_spaces(filter.keyword);

    printf("\n=========================== SEARCH RESULTS ================================\n");
    printf("%-10s | %-25s | %-13s | %-12s | %-8s\n",
           "Class ID", "Class Name", "Instructor ID", "Date", "Time");
    printf("---------------------------------------------------------------------------\n");

    while (fscanf(fp, "%19[^,],%49[^,],%19[^,],%19[^,],%9[^\n]\n",
                  cls.classId, cls.className, cls.instructorId, cls.date, cls.time) == 5) {
        int match = 0;
        if (searchBy == 1 && inst_equals_ignore_case(cls.classId, filter.keyword)) {
            match = 1;
        } else if (searchBy == 2 && inst_contains_ignore_case(cls.className, filter.keyword)) {
            match = 1;
        } else if (searchBy == 3 && strcmp(cls.date, filter.keyword) == 0) {
            match = 1;
        }

        if (match) {
            printf("%-10s | %-25s | %-13s | %-12s | %-8s\n",
                   cls.classId, cls.className, cls.instructorId, cls.date, cls.time);
            found++;
        }
    }

    if (found == 0) {
        printf("No matching class schedule found for '%s'.\n", filter.keyword);
    }
    printf("===========================================================================\n");
    fclose(fp);
}

void update_class_schedule(const char currentId[]) {
    FILE *fp, *tempFp;
    struct ClassSchedule cls;
    char targetId[MAX_ID_LEN];
    int found = 0;
    int unauthorized = 0;

    view_class_schedules(currentId);

    printf("\nEnter Class ID to update (or 0 to cancel): ");
    if (scanf("%19[^\n]", targetId) != 1) {
        inst_clear_buffer();
        return;
    }
    inst_clear_buffer();
    inst_trim_spaces(targetId);

    if (strcmp(targetId, "0") == 0) {
        printf("Update cancelled.\n");
        return;
    }

    fp = fopen(CLASS_FILE, "r");
    tempFp = fopen(TEMP_FILE, "w");
    if (fp == NULL || tempFp == NULL) {
        printf("[Error] File error during update.\n");
        if (fp) fclose(fp);
        if (tempFp) fclose(tempFp);
        return;
    }

    while (fscanf(fp, "%19[^,],%49[^,],%19[^,],%19[^,],%9[^\n]\n",
                  cls.classId, cls.className, cls.instructorId, cls.date, cls.time) == 5) {
        if (inst_equals_ignore_case(cls.classId, targetId)) {
            if (!inst_equals_ignore_case(cls.instructorId, currentId)) {
                unauthorized = 1;
            } else {
                char newName[MAX_NAME_LEN];
                char newDate[MAX_DATE_LEN];
                char newTime[MAX_TIME_LEN];

                found = 1;
                printf("\nUpdating [%s] (%s | %s %s)\n", cls.classId, cls.className, cls.date, cls.time);

                printf("New Class Name (enter '-' to keep current): ");
                if (scanf("%49[^\n]", newName) == 1) {
                    inst_clear_buffer();
                    inst_trim_spaces(newName);
                    if (strcmp(newName, "-") != 0 && strlen(newName) > 0 && strchr(newName, ',') == NULL) {
                        strcpy(cls.className, newName);
                    }
                } else {
                    inst_clear_buffer();
                }

                while (1) {
                    printf("New Date YYYY-MM-DD (enter '-' to keep current): ");
                    if (scanf("%19[^\n]", newDate) != 1) {
                        inst_clear_buffer();
                        continue;
                    }
                    inst_clear_buffer();
                    inst_trim_spaces(newDate);
                    if (strcmp(newDate, "-") == 0) break;
                    if (is_valid_date_format(newDate)) {
                        strcpy(cls.date, newDate);
                        break;
                    }
                    printf("[Error] Invalid date format.\n");
                }

                while (1) {
                    printf("New Time HH:MM (enter '-' to keep current): ");
                    if (scanf("%9[^\n]", newTime) != 1) {
                        inst_clear_buffer();
                        continue;
                    }
                    inst_clear_buffer();
                    inst_trim_spaces(newTime);
                    if (strcmp(newTime, "-") == 0) break;
                    if (is_valid_time_format(newTime)) {
                        strcpy(cls.time, newTime);
                        break;
                    }
                    printf("[Error] Invalid time format.\n");
                }
            }
        }

        fprintf(tempFp, "%s,%s,%s,%s,%s\n",
                cls.classId, cls.className, cls.instructorId, cls.date, cls.time);
    }

    fclose(fp);
    fclose(tempFp);

    if (unauthorized) {
        remove(TEMP_FILE);
        printf("[Access Denied] You can only update classes assigned to your ID (%s).\n", currentId);
    } else if (!found) {
        remove(TEMP_FILE);
        printf("[Not Found] Class ID '%s' does not exist.\n", targetId);
    } else {
        remove(CLASS_FILE);
        rename(TEMP_FILE, CLASS_FILE);
        printf("[OK] Class schedule [%s] updated successfully!\n", targetId);
    }
}
