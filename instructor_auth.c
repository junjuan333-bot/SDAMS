/*
 * File: instructor_auth.c
 * Module: Instructor Module - Authentication & Main Portal
 * Description: Handles instructor login verification and main menu routing.
 */
#include "instructor.h"

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

void instructor_login_portal(void) {
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
            return;
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
            return;
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
            printf("\nLogin Successful! Welcome, Instructor %s (%s).\n", inst.name, inst.id);
            instructor_main_menu(inst.id, inst.name);
            return;
        } else if (authenticated == -1) {
            printf("\n[Access Denied] Your instructor account is currently '%s'.\n", inst.status);
        } else {
            printf("\n[Error] Invalid Instructor ID or Name. Please try again.\n");
        }
    }
}

void instructor_main_menu(const char currentId[], const char currentName[]) {
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
            case 1:
                class_schedule_menu(currentId);
                break;
            case 2:
                facility_problem_menu(currentId, currentName);
                break;
            case 3:
                instructor_rating_menu(currentId);
                break;
            case 0:
                printf("\nLogging out Instructor [%s - %s]...\n", currentId, currentName);
                break;
            default:
                printf("\n[Invalid] Please select a valid option (0-3).\n");
        }
    } while (choice != 0);
}
