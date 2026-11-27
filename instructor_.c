/*
 * File: instructor.c
 * Module: Instructor Module - Self Defence Academy Management System (SDAMS)
 * Description: Implements instructor login, class schedule management,
 *              facility problem reporting, and rating viewing.
 */
#include "instructor.h"

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

struct Instructor {
    char id[MAX_ID_LEN];
    char name[MAX_NAME_LEN];
    char role[MAX_NAME_LEN];
    char contact[MAX_ID_LEN];
    char status[MAX_STATUS_LEN];
};

struct ClassSchedule {
    char classId[MAX_ID_LEN];
    char className[MAX_NAME_LEN];
    char instructorId[MAX_ID_LEN];
    char date[MAX_DATE_LEN];
    char time[MAX_TIME_LEN];
};

struct FacilityProblem {
    char problemID[MAX_ID_LEN];
    char description[MAX_DESC_LEN];
    char status[MAX_STATUS_LEN];
    char reportedBy[MAX_NAME_LEN];
    char reportedDate[MAX_DATE_LEN];
    char facilityName[MAX_NAME_LEN];
};

struct InstructorRating {
    char ratingId[MAX_ID_LEN];
    char studentId[MAX_ID_LEN];
    char instructorId[MAX_ID_LEN];
    char classId[MAX_ID_LEN];
    int score;
    char comment[MAX_DESC_LEN];
};

union SearchFilter {
    char keyword[MAX_NAME_LEN];
    int minScore;
};

static void inst_clear_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

static void inst_trim_spaces(char str[]) {
    int start = 0;
    int end = (int)strlen(str) - 1;
    int i = 0;

    while (str[start] != '\0' && isspace((unsigned char)str[start])) start++;
    while (end >= start && isspace((unsigned char)str[end])) end--;
    while (start <= end) str[i++] = str[start++];
    str[i] = '\0';
}

static int inst_equals_ignore_case(const char s1[], const char s2[]) {
    int i = 0;
    while (s1[i] != '\0' && s2[i] != '\0') {
        if (tolower((unsigned char)s1[i]) != tolower((unsigned char)s2[i])) return 0;
        i++;
    }
    return (s1[i] == '\0' && s2[i] == '\0');
}

static int inst_contains_ignore_case(const char haystack[], const char needle[]) {
    int hLen = (int)strlen(haystack);
    int nLen = (int)strlen(needle);
    int i, j;

    if (nLen == 0) return 1;
    if (nLen > hLen) return 0;

    for (i = 0; i <= hLen - nLen; i++) {
        for (j = 0; j < nLen; j++) {
            if (tolower((unsigned char)haystack[i + j]) != tolower((unsigned char)needle[j])) break;
        }
        if (j == nLen) return 1;
    }
    return 0;
}

static int parse_instructor_line(const char line[], struct Instructor *inst) {
    char buf[256];
    strncpy(buf, line, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';
    inst_trim_spaces(buf);

    if (buf[0] == '\0') return 0;

    if (strchr(buf, '|') != NULL) {
        if (sscanf(buf, "%19[^|]|%49[^|]|%49[^|]|%19[^|]|%19[^\n]",
                   inst->id, inst->name, inst->role, inst->contact, inst->status) == 5) {
            inst_trim_spaces(inst->id);
            inst_trim_spaces(inst->name);
            inst_trim_spaces(inst->role);
            inst_trim_spaces(inst->contact);
            inst_trim_spaces(inst->status);
            return 1;
        }
    } else {
        if (sscanf(buf, "%19s %49s %49s %19s %19s",
                   inst->id, inst->name, inst->role, inst->contact, inst->status) == 5) {
            return 1;
        }
    }
    return 0;
}

// Login Part

int instructorLogin(char currentId[], char currentName[]) {
    char inputId[MAX_ID_LEN];
    char inputName[MAX_NAME_LEN];
    char line[256];
    struct Instructor inst;
    FILE *fp;
    int authenticated = 0;

    while (1) {
        printf("\n========================================\n");
        printf("         SDAMS - INSTRUCTOR LOGIN       \n");
        printf("========================================\n");
        printf("Enter Instructor ID (or 0 to Cancel): ");
        if (scanf("%19[^\n]", inputId) != 1) {
            inst_clear_buffer();
            continue;
        }
        inst_clear_buffer();
        inst_trim_spaces(inputId);

        if (strcmp(inputId, "0") == 0) {
            printf("Returning to Main System Menu...\n");
            return 0;
        }

        printf("Enter Instructor Name: ");
        if (scanf("%49[^\n]", inputName) != 1) {
            inst_clear_buffer();
            continue;
        }
        inst_clear_buffer();
        inst_trim_spaces(inputName);

        fp = fopen(INSTRUCTOR_FILE, "r");
        if (fp == NULL) {
            printf("[Error] Cannot open %s. Please contact Administrator.\n", INSTRUCTOR_FILE);
            return 0;
        }

        authenticated = 0;
        while (fgets(line, sizeof(line), fp) != NULL) {
            if (parse_instructor_line(line, &inst)) {
                if (inst_equals_ignore_case(inst.id, inputId) && inst_equals_ignore_case(inst.name, inputName)) {
                    if (inst_equals_ignore_case(inst.status, "Active")) {
                        authenticated = 1;
                    } else {
                        authenticated = -1;
                    }
                    break;
                }
            }
        }
        fclose(fp);

        if (authenticated == 1) {
            strcpy(currentId, inst.id);
            strcpy(currentName, inst.name);
            printf("\nLogin Successful! Welcome, Instructor %s (%s).\n", currentName, currentId);
            return 1;
        } else if (authenticated == -1) {
            printf("\n[Access Denied] Your instructor account is currently '%s'.\n", inst.status);
        } else {
            printf("\n[Error] Invalid Instructor ID or Name. Please try again.\n");
        }
    }
}

// Classes Management (Add / View / Search / Update)

static void generate_next_class_id(char nextId[]) {
    FILE *fp = fopen(CLASS_FILE, "r");
    struct ClassSchedule temp;
    int maxNum = 100, num = 0;

    if (fp != NULL) {
        while (fscanf(fp, "%19[^,],%49[^,],%19[^,],%19[^,],%9[^\n]\n",
                      temp.classId, temp.className, temp.instructorId, temp.date, temp.time) == 5) {
            if (sscanf(temp.classId, "C%d", &num) == 1 || sscanf(temp.classId, "c%d", &num) == 1) {
                if (num > maxNum) maxNum = num;
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
    return (y >= 2020 && y <= 2100 && m >= 1 && m <= 12 && d >= 1 && d <= 31);
}

static int is_valid_time_format(const char timeStr[]) {
    int h, m;
    if (strlen(timeStr) != 5 || timeStr[2] != ':') return 0;
    if (sscanf(timeStr, "%2d:%2d", &h, &m) != 2) return 0;
    return (h >= 0 && h <= 23 && m >= 0 && m <= 59);
}

static void add_class_schedule(const char currentId[]) {
    struct ClassSchedule newClass;
    FILE *fp;

    generate_next_class_id(newClass.classId);
    strcpy(newClass.instructorId, currentId);

    printf("\n--- Add New Class Schedule (Auto ID: %s) ---\n", newClass.classId);

    while (1) {
        printf("Enter Class Name (no commas, or 0 to cancel): ");
        if (scanf("%49[^\n]", newClass.className) != 1) { inst_clear_buffer(); continue; }
        inst_clear_buffer();
        inst_trim_spaces(newClass.className);
        if (strcmp(newClass.className, "0") == 0) return;
        if (strlen(newClass.className) == 0 || strchr(newClass.className, ',') != NULL) {
            printf("[Error] Class name cannot be empty or contain commas (',').\n");
            continue;
        }
        break;
    }

    while (1) {
        printf("Enter Date (YYYY-MM-DD, e.g. 2026-11-28): ");
        if (scanf("%19[^\n]", newClass.date) != 1) { inst_clear_buffer(); continue; }
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
        if (scanf("%9[^\n]", newClass.time) != 1) { inst_clear_buffer(); continue; }
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
        printf("[Error] Unable to open %s.\n", CLASS_FILE);
        return;
    }
    fprintf(fp, "%s,%s,%s,%s,%s\n",
            newClass.classId, newClass.className, newClass.instructorId, newClass.date, newClass.time);
    fclose(fp);
    printf("[OK] Class %s (%s) scheduled on %s at %s!\n",
           newClass.classId, newClass.className, newClass.date, newClass.time);
}

static void view_class_schedules(const char currentId[]) {
    FILE *fp = fopen(CLASS_FILE, "r");
    struct ClassSchedule cls;
    int mode = 1, count = 0;

    if (fp == NULL) {
        printf("\n[Info] No class schedule records found (%s missing).\n", CLASS_FILE);
        return;
    }

    printf("\nView Option: [1] My Classes Only (%s)  [2] All Academy Classes: ", currentId);
    if (scanf("%d", &mode) != 1) { inst_clear_buffer(); mode = 1; } else { inst_clear_buffer(); }

    printf("\n===========================================================================\n");
    printf("%-10s | %-25s | %-13s | %-12s | %-8s\n",
           "Class ID", "Class Name", "Instructor ID", "Date", "Time");
    printf("---------------------------------------------------------------------------\n");

    while (fscanf(fp, "%19[^,],%49[^,],%19[^,],%19[^,],%9[^\n]\n",
                  cls.classId, cls.className, cls.instructorId, cls.date, cls.time) == 5) {
        if (mode == 1 && !inst_equals_ignore_case(cls.instructorId, currentId)) continue;
        printf("%-10s | %-25s | %-13s | %-12s | %-8s\n",
               cls.classId, cls.className, cls.instructorId, cls.date, cls.time);
        count++;
    }
    printf("===========================================================================\n");
    printf("Total Displayed Classes: %d\n", count);
    fclose(fp);
}

static void search_class_schedule(void) {
    FILE *fp = fopen(CLASS_FILE, "r");
    struct ClassSchedule cls;
    union SearchFilter filter;
    int searchBy, found = 0;

    if (fp == NULL) { printf("\n[Error] Cannot open %s.\n", CLASS_FILE); return; }

    printf("\nSearch Class By: [1] Class ID  [2] Class Name  [3] Date (YYYY-MM-DD): ");
    if (scanf("%d", &searchBy) != 1 || searchBy < 1 || searchBy > 3) {
        inst_clear_buffer(); fclose(fp); return;
    }
    inst_clear_buffer();

    printf("Enter search keyword: ");
    if (scanf("%49[^\n]", filter.keyword) != 1) { inst_clear_buffer(); fclose(fp); return; }
    inst_clear_buffer();
    inst_trim_spaces(filter.keyword);

    printf("\n=========================== SEARCH RESULTS ================================\n");
    printf("%-10s | %-25s | %-13s | %-12s | %-8s\n",
           "Class ID", "Class Name", "Instructor ID", "Date", "Time");
    printf("---------------------------------------------------------------------------\n");

    while (fscanf(fp, "%19[^,],%49[^,],%19[^,],%19[^,],%9[^\n]\n",
                  cls.classId, cls.className, cls.instructorId, cls.date, cls.time) == 5) {
        int match = 0;
        if (searchBy == 1 && inst_equals_ignore_case(cls.classId, filter.keyword)) match = 1;
        else if (searchBy == 2 && inst_contains_ignore_case(cls.className, filter.keyword)) match = 1;
        else if (searchBy == 3 && strcmp(cls.date, filter.keyword) == 0) match = 1;

        if (match) {
            printf("%-10s | %-25s | %-13s | %-12s | %-8s\n",
                   cls.classId, cls.className, cls.instructorId, cls.date, cls.time);
            found++;
        }
    }
    if (found == 0) printf("No matching class schedule found for '%s'.\n", filter.keyword);
    printf("===========================================================================\n");
    fclose(fp);
}

static void update_class_schedule(const char currentId[]) {
    FILE *fp, *tempFp;
    struct ClassSchedule cls;
    char targetId[MAX_ID_LEN];
    int found = 0, unauthorized = 0;

    view_class_schedules(currentId);
    printf("\nEnter Class ID to update (or 0 to cancel): ");
    if (scanf("%19[^\n]", targetId) != 1) { inst_clear_buffer(); return; }
    inst_clear_buffer();
    inst_trim_spaces(targetId);
    if (strcmp(targetId, "0") == 0) return;

    fp = fopen(CLASS_FILE, "r");
    tempFp = fopen(TEMP_FILE, "w");
    if (fp == NULL || tempFp == NULL) {
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
                char newName[MAX_NAME_LEN], newDate[MAX_DATE_LEN], newTime[MAX_TIME_LEN];
                found = 1;
                printf("\nUpdating [%s] (%s | %s %s)\n", cls.classId, cls.className, cls.date, cls.time);

                printf("New Class Name (enter '-' to keep current): ");
                if (scanf("%49[^\n]", newName) == 1) {
                    inst_clear_buffer();
                    inst_trim_spaces(newName);
                    if (strcmp(newName, "-") != 0 && strlen(newName) > 0 && strchr(newName, ',') == NULL)
                        strcpy(cls.className, newName);
                } else { inst_clear_buffer(); }

                while (1) {
                    printf("New Date YYYY-MM-DD (enter '-' to keep current): ");
                    if (scanf("%19[^\n]", newDate) != 1) { inst_clear_buffer(); continue; }
                    inst_clear_buffer();
                    inst_trim_spaces(newDate);
                    if (strcmp(newDate, "-") == 0) break;
                    if (is_valid_date_format(newDate)) { strcpy(cls.date, newDate); break; }
                    printf("[Error] Invalid date format.\n");
                }

                while (1) {
                    printf("New Time HH:MM (enter '-' to keep current): ");
                    if (scanf("%9[^\n]", newTime) != 1) { inst_clear_buffer(); continue; }
                    inst_clear_buffer();
                    inst_trim_spaces(newTime);
                    if (strcmp(newTime, "-") == 0) break;
                    if (is_valid_time_format(newTime)) { strcpy(cls.time, newTime); break; }
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

static void class_schedule_menu(const char currentId[]) {
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

        if (scanf("%d", &choice) != 1) { inst_clear_buffer(); choice = -1; } else { inst_clear_buffer(); }

        switch (choice) {
            case 1: add_class_schedule(currentId); break;
            case 2: view_class_schedules(currentId); break;
            case 3: search_class_schedule(); break;
            case 4: update_class_schedule(currentId); break;
            case 0: break;
            default: printf("\n[Invalid] Please enter 0-4.\n");
        }
    } while (choice != 0);
}

// Facilities Problems Report

static void generate_next_problem_id(char nextId[]) {
    FILE *fp = fopen(PROBLEM_FILE, "r");
    struct FacilityProblem temp;
    int maxNum = 0, num = 0;

    if (fp != NULL) {
        while (fscanf(fp, "%19[^|]|%49[^|]|%99[^|]|%49[^|]|%19[^|]|%19[^\n]\n",
                      temp.problemID, temp.facilityName, temp.description,
                      temp.reportedBy, temp.reportedDate, temp.status) == 6) {
            inst_trim_spaces(temp.problemID);
            if (sscanf(temp.problemID, "FP%d", &num) == 1 || sscanf(temp.problemID, "fp%d", &num) == 1) {
                if (num > maxNum) maxNum = num;
            }
        }
        fclose(fp);
    }
    sprintf(nextId, "FP%03d", maxNum + 1);
}

static void add_facility_problem(const char currentName[]) {
    struct FacilityProblem prob;
    int catChoice;
    char detail[70];
    const char *categories[] = {
        "Damaged Mats", "Broken Equipment", "Lighting Problem",
        "Air-Conditioning Issue", "Damaged Room", "Other Facility Issue"
    };
    FILE *fp;

    generate_next_problem_id(prob.problemID);
    sprintf(prob.reportedBy, "Inst. %s", currentName);
    strcpy(prob.status, "Pending");

    printf("\n--- Report Facility Problem (Auto ID: %s) ---\n", prob.problemID);
    printf("1. Damaged Mats\n2. Broken Equipment\n3. Lighting Problem\n");
    printf("4. Air-Conditioning Issue\n5. Damaged Room\n6. Other Facility Issue\n0. Cancel\n");
    printf("Select Category: ");

    if (scanf("%d", &catChoice) != 1 || catChoice < 0 || catChoice > 6) {
        inst_clear_buffer(); return;
    }
    inst_clear_buffer();
    if (catChoice == 0) return;

    while (1) {
        printf("Enter Facility / Room Name (e.g. Dojo Hall A, no '|'): ");
        if (scanf("%49[^\n]", prob.facilityName) != 1) { inst_clear_buffer(); continue; }
        inst_clear_buffer();
        inst_trim_spaces(prob.facilityName);
        if (strlen(prob.facilityName) == 0 || strchr(prob.facilityName, '|') != NULL) {
            printf("[Error] Cannot be empty or contain '|'.\n");
            continue;
        }
        break;
    }

    while (1) {
        printf("Enter Specific Problem Details (no '|'): ");
        if (scanf("%69[^\n]", detail) != 1) { inst_clear_buffer(); continue; }
        inst_clear_buffer();
        inst_trim_spaces(detail);
        if (strlen(detail) == 0 || strchr(detail, '|') != NULL) {
            printf("[Error] Cannot be empty or contain '|'.\n");
            continue;
        }
        break;
    }

    sprintf(prob.description, "[%s] %s", categories[catChoice - 1], detail);

    while (1) {
        printf("Enter Reported Date (DD-MM-YYYY, e.g. 24-11-2026): ");
        if (scanf("%19[^\n]", prob.reportedDate) != 1) { inst_clear_buffer(); continue; }
        inst_clear_buffer();
        inst_trim_spaces(prob.reportedDate);
        if (strlen(prob.reportedDate) != 10 || prob.reportedDate[2] != '-' || prob.reportedDate[5] != '-') {
            printf("[Error] Please use DD-MM-YYYY format.\n");
            continue;
        }
        break;
    }

    fp = fopen(PROBLEM_FILE, "a");
    if (fp == NULL) { printf("[Error] Unable to open %s.\n", PROBLEM_FILE); return; }

    fprintf(fp, "%s|%s|%s|%s|%s|%s\n",
            prob.problemID, prob.facilityName, prob.description,
            prob.reportedBy, prob.reportedDate, prob.status);
    fclose(fp);
    printf("[OK] Facility problem %s logged! Status: %s\n", prob.problemID, prob.status);
}

static void view_facility_problems(void) {
    FILE *fp = fopen(PROBLEM_FILE, "r");
    struct FacilityProblem prob;
    int count = 0;

    if (fp == NULL) { printf("\n[Info] No facility problem records found.\n"); return; }

    printf("\n========================================================================================================\n");
    printf("%-8s | %-18s | %-34s | %-15s | %-12s | %-12s\n",
           "Prob ID", "Facility Name", "Description", "Reported By", "Date", "Status");
    printf("--------------------------------------------------------------------------------------------------------\n");

    while (fscanf(fp, "%19[^|]|%49[^|]|%99[^|]|%49[^|]|%19[^|]|%19[^\n]\n",
                  prob.problemID, prob.facilityName, prob.description,
                  prob.reportedBy, prob.reportedDate, prob.status) == 6) {
        inst_trim_spaces(prob.problemID);
        inst_trim_spaces(prob.facilityName);
        inst_trim_spaces(prob.description);
        inst_trim_spaces(prob.reportedBy);
        inst_trim_spaces(prob.reportedDate);
        inst_trim_spaces(prob.status);

        printf("%-8s | %-18s | %-34s | %-15s | %-12s | %-12s\n",
               prob.problemID, prob.facilityName, prob.description,
               prob.reportedBy, prob.reportedDate, prob.status);
        count++;
    }
    printf("========================================================================================================\n");
    printf("Total Facility Problems Listed: %d\n", count);
    fclose(fp);
}

static void facility_problem_menu(const char currentName[]) {
    int choice;
    do {
        printf("\n--------------------------------------------\n");
        printf("         FACILITY PROBLEM REPORTING         \n");
        printf("--------------------------------------------\n");
        printf("1. Report New Facility Problem\n");
        printf("2. View Facility Problems\n");
        printf("0. Back to Instructor Menu\n");
        printf("--------------------------------------------\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) { inst_clear_buffer(); choice = -1; } else { inst_clear_buffer(); }

        switch (choice) {
            case 1: add_facility_problem(currentName); break;
            case 2: view_facility_problems(); break;
            case 0: break;
            default: printf("\n[Invalid] Please enter 0-2.\n");
        }
    } while (choice != 0);
}

// Instructor Ratings

static void view_instructor_ratings(const char currentId[]) {
    FILE *fp = fopen(RATING_FILE, "r");
    struct InstructorRating r;
    int count = 0, totalScore = 0;

    if (fp == NULL) { printf("\n[Info] No rating records file (%s) found yet.\n", RATING_FILE); return; }

    printf("\n=================================================================================\n");
    printf("%-10s | %-10s | %-10s | %-7s | %-35s\n",
           "Rating ID", "Student ID", "Class ID", "Score", "Student Comment");
    printf("---------------------------------------------------------------------------------\n");

    while (fscanf(fp, "%19[^|]|%19[^|]|%19[^|]|%19[^|]|%d|%99[^\n]\n",
                  r.ratingId, r.studentId, r.instructorId, r.classId, &r.score, r.comment) == 6) {
        inst_trim_spaces(r.instructorId);
        if (inst_equals_ignore_case(r.instructorId, currentId)) {
            inst_trim_spaces(r.ratingId);
            inst_trim_spaces(r.studentId);
            inst_trim_spaces(r.classId);
            inst_trim_spaces(r.comment);

            printf("%-10s | %-10s | %-10s | %d / 5   | %-35s\n",
                   r.ratingId, r.studentId, r.classId, r.score, r.comment);
            totalScore += r.score;
            count++;
        }
    }
    printf("=================================================================================\n");
    if (count == 0) printf("No student ratings recorded for Instructor %s yet.\n", currentId);
    else printf("Total Ratings: %d | Average Rating Score: %.2f / 5.00\n", count, (double)totalScore / count);
    fclose(fp);
}

static void search_instructor_ratings(const char currentId[]) {
    FILE *fp = fopen(RATING_FILE, "r");
    struct InstructorRating r;
    union SearchFilter filter;
    int searchType, found = 0;

    if (fp == NULL) { printf("\n[Info] No rating records file (%s) found.\n", RATING_FILE); return; }

    printf("\nSearch Ratings By: [1] Class ID  [2] Minimum Score (1-5): ");
    if (scanf("%d", &searchType) != 1 || (searchType != 1 && searchType != 2)) {
        inst_clear_buffer(); fclose(fp); return;
    }
    inst_clear_buffer();

    if (searchType == 1) {
        printf("Enter Class ID (e.g. C101): ");
        if (scanf("%49[^\n]", filter.keyword) != 1) { inst_clear_buffer(); fclose(fp); return; }
        inst_clear_buffer();
        inst_trim_spaces(filter.keyword);
    } else {
        printf("Enter Minimum Score (1-5): ");
        if (scanf("%d", &filter.minScore) != 1 || filter.minScore < 1 || filter.minScore > 5) {
            inst_clear_buffer(); fclose(fp); return;
        }
        inst_clear_buffer();
    }

    printf("\n============================ RATING SEARCH RESULT ===============================\n");
    printf("%-10s | %-10s | %-10s | %-7s | %-35s\n",
           "Rating ID", "Student ID", "Class ID", "Score", "Student Comment");
    printf("---------------------------------------------------------------------------------\n");

    while (fscanf(fp, "%19[^|]|%19[^|]|%19[^|]|%19[^|]|%d|%99[^\n]\n",
                  r.ratingId, r.studentId, r.instructorId, r.classId, &r.score, r.comment) == 6) {
        inst_trim_spaces(r.instructorId);
        if (!inst_equals_ignore_case(r.instructorId, currentId)) continue;

        inst_trim_spaces(r.classId);
        if ((searchType == 1 && inst_equals_ignore_case(r.classId, filter.keyword)) ||
            (searchType == 2 && r.score >= filter.minScore)) {
            inst_trim_spaces(r.ratingId);
            inst_trim_spaces(r.studentId);
            inst_trim_spaces(r.comment);

            printf("%-10s | %-10s | %-10s | %d / 5   | %-35s\n",
                   r.ratingId, r.studentId, r.classId, r.score, r.comment);
            found++;
        }
    }
    if (found == 0) printf("No matching ratings found.\n");
    printf("=================================================================================\n");
    fclose(fp);
}

static void instructor_rating_menu(const char currentId[]) {
    int choice;
    do {
        printf("\n--------------------------------------------\n");
        printf("         INSTRUCTOR RATING (%s)\n", currentId);
        printf("--------------------------------------------\n");
        printf("1. View My Overall Ratings\n");
        printf("2. Search Ratings (by Class ID or Min Score)\n");
        printf("0. Back to Instructor Menu\n");
        printf("--------------------------------------------\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) { inst_clear_buffer(); choice = -1; } else { inst_clear_buffer(); }

        switch (choice) {
            case 1: view_instructor_ratings(currentId); break;
            case 2: search_instructor_ratings(currentId); break;
            case 0: break;
            default: printf("\n[Invalid] Please enter 0-2.\n");
        }
    } while (choice != 0);
}

// Instructor's Main Menu

void instructorMenu(const char currentId[], const char currentName[]) {
    int choice;

    do {
        printf("\n============================================\n");
        printf("   SDAMS - INSTRUCTOR PORTAL (%s | %s)\n", currentId, currentName);
        printf("============================================\n");
        printf("1. Class Schedule Management\n");
        printf("2. Issue Facility Problem\n");
        printf("3. View Instructor Rating\n");
        printf("0. Logout\n");
        printf("============================================\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            inst_clear_buffer();
            choice = -1;
        } else {
            inst_clear_buffer();
        }

        switch (choice) {
            case 1: class_schedule_menu(currentId); break;
            case 2: facility_problem_menu(currentName); break;
            case 3: instructor_rating_menu(currentId); break;
            case 0: printf("\nLogging out Instructor [%s - %s]...\n", currentId, currentName); break;
            default: printf("\n[Invalid] Please select a valid option (0-3).\n");
        }
    } while (choice != 0);
}
