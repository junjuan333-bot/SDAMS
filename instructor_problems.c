/*
 * File: instructor_problems.c
 * Module: Instructor Module - Facility Problem Reporting
 * Description: Allows instructors to log and view facility problems in facility_problems.txt.
 */
#include "instructor.h"

static void generate_next_problem_id(char nextId[]) {
    FILE *fp = fopen(PROBLEM_FILE, "r");
    struct FacilityProblem temp;
    int maxNum = 0;
    int num = 0;

    if (fp != NULL) {
        while (fscanf(fp, "%19[^|]|%49[^|]|%99[^|]|%49[^|]|%19[^|]|%19[^\n]\n",
                      temp.problemID, temp.facilityName, temp.description,
                      temp.reportedBy, temp.reportedDate, temp.status) == 6) {
            inst_trim_spaces(temp.problemID);
            if (sscanf(temp.problemID, "FP%d", &num) == 1 || sscanf(temp.problemID, "fp%d", &num) == 1) {
                if (num > maxNum) {
                    maxNum = num;
                }
            }
        }
        fclose(fp);
    }
    sprintf(nextId, "FP%03d", maxNum + 1);
}

void facility_problem_menu(const char currentId[], const char currentName[]) {
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

        if (scanf("%d", &choice) != 1) {
            inst_clear_buffer();
            choice = -1;
        } else {
            inst_clear_buffer();
        }

        switch (choice) {
            case 1:
                add_facility_problem(currentId, currentName);
                break;
            case 2:
                view_facility_problems();
                break;
            case 0:
                break;
            default:
                printf("\n[Invalid] Please enter 0-2.\n");
        }
    } while (choice != 0);
}

void add_facility_problem(const char currentId[], const char currentName[]) {
    struct FacilityProblem prob;
    int catChoice;
    char detail[70];
    const char *categories[] = {
        "Damaged Mats",
        "Broken Equipment",
        "Lighting Problem",
        "Air-Conditioning Issue",
        "Damaged Room",
        "Other Facility Issue"
    };
    FILE *fp;

    (void)currentId;
    generate_next_problem_id(prob.problemID);
    sprintf(prob.reportedBy, "Inst. %s", currentName);
    strcpy(prob.status, "Pending");

    printf("\n--- Report Facility Problem (Auto ID: %s) ---\n", prob.problemID);
    printf("Select Problem Category:\n");
    printf("1. Damaged Mats\n");
    printf("2. Broken Equipment\n");
    printf("3. Lighting Problem\n");
    printf("4. Air-Conditioning Issue\n");
    printf("5. Damaged Room\n");
    printf("6. Other Facility Issue\n");
    printf("0. Cancel\n");
    printf("Choice: ");

    if (scanf("%d", &catChoice) != 1 || catChoice < 0 || catChoice > 6) {
        inst_clear_buffer();
        printf("[Invalid] Cancelled.\n");
        return;
    }
    inst_clear_buffer();

    if (catChoice == 0) {
        printf("Report cancelled.\n");
        return;
    }

    while (1) {
        printf("Enter Facility / Room Name (e.g. Dojo Hall A, no '|'): ");
        if (scanf("%49[^\n]", prob.facilityName) != 1) {
            inst_clear_buffer();
            continue;
        }
        inst_clear_buffer();
        inst_trim_spaces(prob.facilityName);

        if (strlen(prob.facilityName) == 0 || strchr(prob.facilityName, '|') != NULL) {
            printf("[Error] Facility name cannot be empty or contain '|'.\n");
            continue;
        }
        break;
    }

    while (1) {
        printf("Enter Specific Problem Details (no '|'): ");
        if (scanf("%69[^\n]", detail) != 1) {
            inst_clear_buffer();
            continue;
        }
        inst_clear_buffer();
        inst_trim_spaces(detail);

        if (strlen(detail) == 0 || strchr(detail, '|') != NULL) {
            printf("[Error] Description cannot be empty or contain '|'.\n");
            continue;
        }
        break;
    }

    sprintf(prob.description, "[%s] %s", categories[catChoice - 1], detail);

    while (1) {
        printf("Enter Reported Date (DD-MM-YYYY, e.g. 24-11-2026): ");
        if (scanf("%19[^\n]", prob.reportedDate) != 1) {
            inst_clear_buffer();
            continue;
        }
        inst_clear_buffer();
        inst_trim_spaces(prob.reportedDate);

        if (strlen(prob.reportedDate) != 10 || prob.reportedDate[2] != '-' || prob.reportedDate[5] != '-') {
            printf("[Error] Please use DD-MM-YYYY format.\n");
            continue;
        }
        break;
    }

    fp = fopen(PROBLEM_FILE, "a");
    if (fp == NULL) {
        printf("[Error] Unable to open %s.\n", PROBLEM_FILE);
        return;
    }

    fprintf(fp, "%s|%s|%s|%s|%s|%s\n",
            prob.problemID, prob.facilityName, prob.description,
            prob.reportedBy, prob.reportedDate, prob.status);
    fclose(fp);

    printf("[OK] Facility problem %s logged successfully! Status: %s\n",
           prob.problemID, prob.status);
}

void view_facility_problems(void) {
    FILE *fp = fopen(PROBLEM_FILE, "r");
    struct FacilityProblem prob;
    int count = 0;

    if (fp == NULL) {
        printf("\n[Info] No facility problem records found.\n");
        return;
    }

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
