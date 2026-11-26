/*
 * File: facilities_officer.c
 * Module: Facilities Officer Module - Self Defence Academy Management System (SDAMS)
 * Description: Implements facilities officer authentication, equipment inventory management, facility problem management, and reporting.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "facilities_officer.h"

/* =========================================================================
   HELPER & VALIDATION FUNCTIONS
   ========================================================================= */

/* Function: generateNextEquipmentID
 * Assumption: Existing equipment IDs in 'inventory.txt' follow the 'EQxxx'
 * numeric pattern. If the file does not exist or is empty, numbering starts at EQ001. */
void generateNextEquipmentID(char *newID) {
    FILE *fp = fopen(INVENTORY_FILE, "r");
    Equipment temp;
    int maxNum = 0;
    int currentNum = 0;

    if (fp != NULL) {
        while (fscanf(fp, "%9[^|]|%49[^|]|%29[^|]|%d|%lf|%19[^\n]\n",
                      temp.equipmentID, temp.name, temp.category,
                      &temp.quantity, &temp.unitCost, temp.status) == 6) {
            if (strncmp(temp.equipmentID, "EQ", 2) == 0) {
                currentNum = atoi(temp.equipmentID + 2);
                if (currentNum > maxNum) {
                    maxNum = currentNum;
                }
            }
        }
        fclose(fp);
    }

    sprintf(newID, "EQ%03d", maxNum + 1);
}

/* Function: foClearBuffer
 * Assumption: Standard input stream (stdin) may contain residual newline 
 * or invalid characters after formatted input scans. */
void foClearBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* Function: foReadString
 * Assumption: Pointer 'buffer' points to a valid character array of at least 
 * 'size' bytes, and user input does not exceed the defined buffer size. */
void foReadString(const char *prompt, char *buffer, int size) {
    size_t len;
    printf("%s", prompt);
    if (fgets(buffer, size, stdin) != NULL) {
        len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        } else {
            foClearBuffer();
        }
    }
}

/* Function: foValidateID
 * Assumption: All valid system primary keys strictly follow a 5-character 
 * format consisting of a 2-letter uppercase prefix followed by 3 digits. */
int foValidateID(const char *id, const char *prefix) {
    if (strlen(id) != 5) return 0;
    if (strncmp(id, prefix, 2) != 0) return 0;
    if (!isdigit((unsigned char)id[2]) || !isdigit((unsigned char)id[3]) || !isdigit((unsigned char)id[4])) return 0;
    return 1;
}

/* Function: isEquipmentIDExists
 * Assumption: If 'inventory.txt' exists, its records are formatted with 
 * pipe ('|') delimiters and each Equipment ID is unique. */
int isEquipmentIDExists(const char *id) {
    FILE *fp = fopen(INVENTORY_FILE, "r");
    Equipment temp;
    if (fp == NULL) return 0;
    
    while (fscanf(fp, "%9[^|]|%49[^|]|%29[^|]|%d|%lf|%19[^\n]\n",
                  temp.equipmentID, temp.name, temp.category,
                  &temp.quantity, &temp.unitCost, temp.status) == 6) {
        if (strcmp(temp.equipmentID, id) == 0) {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

/* =========================================================================
   AUTHENTICATION & NAVIGATION MENU
   ========================================================================= */

/* Function: facilitiesOfficerLogin
 * Assumption: Default credentials ('JunJie' / 'JJ@156935') are pre-assigned 
 * to the Facilities Officer role and contain no whitespace characters. */
int facilitiesOfficerLogin(void) {
    char username[20];
    char password[20];
    
    printf("\n========================================\n");
    printf("|        Facilities Officer LOGIN        |\n");
    printf("========================================\n");
    printf("Enter Username: ");
    scanf("%19s", username);
    printf("Enter Password: ");
    scanf("%19s", password);
    foClearBuffer();
    
    if (strcmp(username, "JunJie") == 0 && strcmp(password, "JJ@156935") == 0) {
        printf("\nLogin Successful! Welcome Facilities Officer.\n");
        return 1;
    } else {
        printf("\nInvalid Credentials! Access Denied.\n");
        return 0;
    }
}

/* Function: facilitiesOfficerMenu
 * Assumption: The user is already authenticated before accessing this menu 
 * and will navigate options using integer inputs between 0 and 8. */
void facilitiesOfficerMenu(void) {
    int choice;
    do {
        printf("\n============================================================\n");
        printf("   SDAMS - FACILITIES OFFICER MANAGEMENT DASHBOARD\n");
        printf("============================================================\n");
        printf("  [MODULE A: EQUIPMENT INVENTORY]\n");
        printf("    1. Add New Equipment\n");
        printf("    2. View All Equipment Inventory\n");
        printf("    3. Search Equipment (by ID or Name)\n");
        printf("    4. Update Equipment Details\n");
        printf("------------------------------------------------------------\n");
        printf("  [MODULE B: FACILITY PROBLEM MANAGEMENT]\n");
        printf("    5. View Reported Facility Problems\n");
        printf("    6. Search Facility Problems (by ID or Status)\n");
        printf("    7. Update Facility Problem Resolution Status\n");
        printf("------------------------------------------------------------\n");
        printf("  [MODULE C: REPORTING]\n");
        printf("    8. Generate Equipment Stock & Alert Report\n");
        printf("------------------------------------------------------------\n");
        printf("    0. Return to SDAMS Main Menu\n");
        printf("============================================================\n");
        printf("Enter your choice (0-8): ");

        if (scanf("%d", &choice) != 1) {
            printf("\n[!] Invalid input! Please enter a number between 0 and 8.\n");
            foClearBuffer();
            choice = -1;
            continue;
        }
        foClearBuffer();

        switch (choice) {
            case 1: addEquipment(); break;
            case 2: viewEquipment(); break;
            case 3: searchEquipment(); break;
            case 4: updateEquipment(); break;
            case 5: viewFacilityProblems(); break;
            case 6: searchFacilityProblems(); break;
            case 7: updateProblemStatus(); break;
            case 8: generateStockReport(); break;
            case 0: printf("\nReturning to SDAMS Main Menu...\n"); break;
            default: printf("\n[!] Invalid choice! Please select between 0 and 8.\n");
        }
    } while (choice != 0);
}

/* =========================================================================
   MODULE A: EQUIPMENT INVENTORY MANAGEMENT
   ========================================================================= */

/* Function: addEquipment
 * Assumption: The local directory has write permissions to create or append
 * to 'inventory.txt'. Equipment IDs are auto-generated sequentially to prevent
 * duplication, and items with quantity < LOW_STOCK_THRESHOLD (5) are tagged 'Low Stock'. */
void addEquipment(void) {
    Equipment eq;
    EquipmentMetric metric; /* Using Union (Requirement iv) */
    FILE *fp;

    printf("\n--- [1] ADD NEW EQUIPMENT ---\n");

    /* Auto-generate next sequential Equipment ID */
    generateNextEquipmentID(eq.equipmentID);
    printf("[i] Auto-Generated Equipment ID: %s\n", eq.equipmentID);

    do {
        foReadString("Enter Equipment Name (e.g., Karate Mats): ", eq.name, FO_MAX_NAME);
        if (strlen(eq.name) == 0) printf("[!] Name cannot be empty.\n");
    } while (strlen(eq.name) == 0);

    do {
        foReadString("Enter Category (Mats / Gear / Uniforms / Weapons): ", eq.category, FO_MAX_CATEGORY);
        if (strlen(eq.category) == 0) printf("[!] Category cannot be empty.\n");
    } while (strlen(eq.category) == 0);

    while (1) {
        printf("Enter Quantity in Stock (>= 0): ");
        if (scanf("%d", &eq.quantity) == 1 && eq.quantity >= 0) {
            foClearBuffer();
            break;
        }
        printf("[!] Invalid quantity! Please enter a non-negative integer.\n");
        foClearBuffer();
    }

    while (1) {
        printf("Enter Unit Cost in RM (> 0.00): ");
        if (scanf("%lf", &eq.unitCost) == 1 && eq.unitCost > 0.0) {
            foClearBuffer();
            break;
        }
        printf("[!] Invalid unit cost! Please enter a positive amount.\n");
        foClearBuffer();
    }

    if (eq.quantity < LOW_STOCK_THRESHOLD) {
        strcpy(eq.status, "Low Stock");
        metric.unitDeficit = LOW_STOCK_THRESHOLD - eq.quantity;
        printf("[i] Quantity is below %d (Deficit: %d unit(s)). Status automatically set to 'Low Stock'.\n",
               LOW_STOCK_THRESHOLD, metric.unitDeficit);
    } else {
        foReadString("Enter Condition Status (Available / Damaged): ", eq.status, FO_MAX_STATUS);
        if (strlen(eq.status) == 0) strcpy(eq.status, "Available");
    }

    fp = fopen(INVENTORY_FILE, "a");
    if (fp == NULL) {
        printf("[!] Error: Unable to open file for writing!\n");
        return;
    }
    fprintf(fp, "%s|%s|%s|%d|%.2f|%s\n", eq.equipmentID, eq.name, eq.category, eq.quantity, eq.unitCost, eq.status);
    fclose(fp);
    printf("[OK] Equipment '%s' (%s) added successfully!\n", eq.name, eq.equipmentID);
}

/* Function: viewEquipment
 * Assumption: 'inventory.txt' resides in the current working directory and 
 * contains valid 6-column pipe-delimited equipment records. */
void viewEquipment(void) {
    FILE *fp = fopen(INVENTORY_FILE, "r");
    Equipment eq;
    int count = 0;
    printf("\n--- [2] ALL EQUIPMENT INVENTORY ---\n");
    if (fp == NULL) {
        printf("[i] No inventory records found. Add equipment first.\n");
        return;
    }
    printf("---------------------------------------------------------------------------------------\n");
    printf("%-8s | %-22s | %-12s | %-8s | %-10s | %-12s\n", "EQ ID", "Equipment Name", "Category", "Qty", "Unit(RM)", "Status");
    printf("---------------------------------------------------------------------------------------\n");
    while (fscanf(fp, "%9[^|]|%49[^|]|%29[^|]|%d|%lf|%19[^\n]\n", eq.equipmentID, eq.name, eq.category, &eq.quantity, &eq.unitCost, eq.status) == 6) {
        printf("%-8s | %-22s | %-12s | %-8d | %-10.2f | %-12s\n", eq.equipmentID, eq.name, eq.category, eq.quantity, eq.unitCost, eq.status);
        count++;
    }
    printf("---------------------------------------------------------------------------------------\n");
    printf("Total Equipment Records: %d\n", count);
    fclose(fp);
}

/* Function: searchEquipment
 * Assumption: Search matching is case-sensitive and matches either an exact 
 * Equipment ID or a substring within the Equipment Name. */
void searchEquipment(void) {
    FILE *fp = fopen(INVENTORY_FILE, "r");
    Equipment eq;
    char keyword[FO_MAX_NAME];
    int found = 0;
    printf("\n--- [3] SEARCH EQUIPMENT ---\n");
    if (fp == NULL) {
        printf("[i] Inventory database is empty.\n");
        return;
    }
    foReadString("Enter Equipment ID or Name keyword to search: ", keyword, FO_MAX_NAME);
    printf("\nSearch Results for '%s':\n", keyword);
    printf("---------------------------------------------------------------------------------------\n");
    printf("%-8s | %-22s | %-12s | %-8s | %-10s | %-12s\n", "EQ ID", "Equipment Name", "Category", "Qty", "Unit(RM)", "Status");
    printf("---------------------------------------------------------------------------------------\n");
    while (fscanf(fp, "%9[^|]|%49[^|]|%29[^|]|%d|%lf|%19[^\n]\n", eq.equipmentID, eq.name, eq.category, &eq.quantity, &eq.unitCost, eq.status) == 6) {
        if (strcmp(eq.equipmentID, keyword) == 0 || strstr(eq.name, keyword) != NULL) {
            printf("%-8s | %-22s | %-12s | %-8d | %-10.2f | %-12s\n", eq.equipmentID, eq.name, eq.category, eq.quantity, eq.unitCost, eq.status);
            found++;
        }
    }
    printf("---------------------------------------------------------------------------------------\n");
    if (found == 0) printf("[!] No matching equipment found.\n");
    else printf("[OK] Found %d matching record(s).\n", found);
    fclose(fp);
}

/* Function: updateEquipment
 * Assumption: Equipment IDs and Names are immutable once registered; only 
 * Quantity, Unit Cost, and Condition Status can be updated via temp file replacement. */
void updateEquipment(void) {
    FILE *fp = fopen(INVENTORY_FILE, "r");
    FILE *tempFp = fopen(FO_TEMP_FILE, "w");
    Equipment eq;
    char targetID[FO_MAX_ID];
    int found = 0;
    printf("\n--- [4] UPDATE EQUIPMENT DETAILS ---\n");
    if (fp == NULL || tempFp == NULL) {
        printf("[!] Error opening database files.\n");
        if (fp) fclose(fp); if (tempFp) fclose(tempFp); return;
    }
    foReadString("Enter Equipment ID to update (e.g., EQ001): ", targetID, FO_MAX_ID);
    while (fscanf(fp, "%9[^|]|%49[^|]|%29[^|]|%d|%lf|%19[^\n]\n", eq.equipmentID, eq.name, eq.category, &eq.quantity, &eq.unitCost, eq.status) == 6) {
        if (strcmp(eq.equipmentID, targetID) == 0) {
            found = 1;
            printf("\n[Record Found! Current Details]\nName: %s | Category: %s | Qty: %d | Unit Cost: RM%.2f | Status: %s\n\n", eq.name, eq.category, eq.quantity, eq.unitCost, eq.status);
            while (1) {
                printf("Enter New Quantity (>= 0): ");
                if (scanf("%d", &eq.quantity) == 1 && eq.quantity >= 0) { foClearBuffer(); break; }
                printf("[!] Invalid input! Quantity must be >= 0.\n"); foClearBuffer();
            }
            while (1) {
                printf("Enter New Unit Cost in RM (> 0.00): ");
                if (scanf("%lf", &eq.unitCost) == 1 && eq.unitCost > 0.0) { foClearBuffer(); break; }
                printf("[!] Invalid input! Cost must be > 0.00.\n"); foClearBuffer();
            }
            foReadString("Enter New Status (Available / Low Stock / Damaged): ", eq.status, FO_MAX_STATUS);
            if (eq.quantity < LOW_STOCK_THRESHOLD && strcmp(eq.status, "Available") == 0) {
                strcpy(eq.status, "Low Stock");
                printf("[i] Quantity < %d, status automatically adjusted to 'Low Stock'.\n", LOW_STOCK_THRESHOLD);
            }
        }
        fprintf(tempFp, "%s|%s|%s|%d|%.2f|%s\n", eq.equipmentID, eq.name, eq.category, eq.quantity, eq.unitCost, eq.status);
    }
    fclose(fp); fclose(tempFp);
    if (found) { remove(INVENTORY_FILE); rename(FO_TEMP_FILE, INVENTORY_FILE); printf("[OK] Equipment '%s' updated successfully!\n", targetID); }
    else { remove(FO_TEMP_FILE); printf("[!] Equipment ID '%s' not found.\n", targetID); }
}

/* =========================================================================
   MODULE B: FACILITY PROBLEM MANAGEMENT
   ========================================================================= */

/* Function: viewFacilityProblems
 * Assumption: 'facility_problems.txt' is populated by the Instructor module 
 * using a 6-field pipe-delimited structure. */
void viewFacilityProblems(void) {
    FILE *fp = fopen(PROBLEMS_FILE, "r");
    FacilityProblem prob;
    int count = 0;
    printf("\n--- [5] REPORTED FACILITY PROBLEMS ---\n");
    if (fp == NULL) {
        printf("[i] No facility problem reports found in '%s'.\n", PROBLEMS_FILE);
        return;
    }
    printf("------------------------------------------------------------------------------------------------------------\n");
    printf("%-7s | %-16s | %-32s | %-13s | %-11s | %-12s\n", "PROB ID", "Facility", "Issue Description", "Reported By", "Date", "Status");
    printf("------------------------------------------------------------------------------------------------------------\n");
    while (fscanf(fp, "%9[^|]|%49[^|]|%99[^|]|%49[^|]|%14[^|]|%19[^\n]\n", prob.problemID, prob.facilityName, prob.description, prob.reportedBy, prob.dateReported, prob.status) == 6) {
        printf("%-7s | %-16s | %-32s | %-13s | %-11s | %-12s\n", prob.problemID, prob.facilityName, prob.description, prob.reportedBy, prob.dateReported, prob.status);
        count++;
    }
    printf("------------------------------------------------------------------------------------------------------------\n");
    printf("Total Reported Issues: %d\n", count);
    fclose(fp);
}

/* Function: searchFacilityProblems
 * Assumption: Users search facility tickets by providing either an exact 
 * Problem ID (e.g., FP001) or an exact resolution status string. */
void searchFacilityProblems(void) {
    FILE *fp = fopen(PROBLEMS_FILE, "r");
    FacilityProblem prob;
    char keyword[FO_MAX_NAME];
    int found = 0;
    printf("\n--- [6] SEARCH FACILITY PROBLEMS ---\n");
    if (fp == NULL) {
        printf("[i] No facility problem reports available.\n");
        return;
    }
    foReadString("Enter Problem ID or Status (Pending / In Progress / Resolved): ", keyword, FO_MAX_NAME);
    printf("\nSearch Results for '%s':\n", keyword);
    printf("------------------------------------------------------------------------------------------------------------\n");
    printf("%-7s | %-16s | %-32s | %-13s | %-11s | %-12s\n", "PROB ID", "Facility", "Issue Description", "Reported By", "Date", "Status");
    printf("------------------------------------------------------------------------------------------------------------\n");
    while (fscanf(fp, "%9[^|]|%49[^|]|%99[^|]|%49[^|]|%14[^|]|%19[^\n]\n", prob.problemID, prob.facilityName, prob.description, prob.reportedBy, prob.dateReported, prob.status) == 6) {
        if (strcmp(prob.problemID, keyword) == 0 || strcmp(prob.status, keyword) == 0) {
            printf("%-7s | %-16s | %-32s | %-13s | %-11s | %-12s\n", prob.problemID, prob.facilityName, prob.description, prob.reportedBy, prob.dateReported, prob.status);
            found++;
        }
    }
    printf("------------------------------------------------------------------------------------------------------------\n");
    if (found == 0) printf("[!] No matching facility problem records found.\n");
    else printf("[OK] Found %d matching issue(s).\n", found);
    fclose(fp);
}

/* Function: updateProblemStatus
 * Assumption: Only the resolution status of a reported problem can be modified 
 * by the Facilities Officer; original incident details remain unchanged. */
void updateProblemStatus(void) {
    FILE *fp = fopen(PROBLEMS_FILE, "r");
    FILE *tempFp = fopen(FO_TEMP_FILE, "w");
    FacilityProblem prob;
    char targetID[FO_MAX_ID];
    int statusOption, found = 0;
    printf("\n--- [7] UPDATE FACILITY PROBLEM RESOLUTION STATUS ---\n");
    if (fp == NULL || tempFp == NULL) {
        printf("[!] Error opening facility problem files.\n");
        if (fp) fclose(fp); if (tempFp) fclose(tempFp); return;
    }
    foReadString("Enter Problem ID to update (e.g., FP001): ", targetID, FO_MAX_ID);
    while (fscanf(fp, "%9[^|]|%49[^|]|%99[^|]|%49[^|]|%14[^|]|%19[^\n]\n", prob.problemID, prob.facilityName, prob.description, prob.reportedBy, prob.dateReported, prob.status) == 6) {
        if (strcmp(prob.problemID, targetID) == 0) {
            found = 1;
            printf("\n[Issue Found!]\nFacility: %s | Description: %s | Current Status: %s\n\n", prob.facilityName, prob.description, prob.status);
            printf("Select New Resolution Status:\n  1. Pending\n  2. In Progress\n  3. Resolved\n");
            while (1) {
                printf("Enter choice (1-3): ");
                if (scanf("%d", &statusOption) == 1 && statusOption >= 1 && statusOption <= 3) { foClearBuffer(); break; }
                printf("[!] Invalid choice! Enter 1, 2, or 3.\n"); foClearBuffer();
            }
            if (statusOption == 1) strcpy(prob.status, "Pending");
            else if (statusOption == 2) strcpy(prob.status, "In Progress");
            else if (statusOption == 3) strcpy(prob.status, "Resolved");
        }
        fprintf(tempFp, "%s|%s|%s|%s|%s|%s\n", prob.problemID, prob.facilityName, prob.description, prob.reportedBy, prob.dateReported, prob.status);
    }
    fclose(fp); fclose(tempFp);
    if (found) { remove(PROBLEMS_FILE); rename(FO_TEMP_FILE, PROBLEMS_FILE); printf("[OK] Problem '%s' resolution status updated successfully!\n", targetID); }
    else { remove(FO_TEMP_FILE); printf("[!] Problem ID '%s' not found.\n", targetID); }
}

/* =========================================================================
   MODULE C: REPORTING
   ========================================================================= */

/* Function: generateStockReport
 * Assumption: Financial valuation is calculated as Quantity * Unit Cost, and 
 * any item with quantity < LOW_STOCK_THRESHOLD (5) requires restock flagging. */
void generateStockReport(void) {
    FILE *fp = fopen(INVENTORY_FILE, "r");
    Equipment eq;
    EquipmentMetric metric; /* Using Union */
    int filterOption, totalItems = 0, lowStockCount = 0;
    char filterKeyword[FO_MAX_NAME] = "";
    double totalValuation = 0.0;
    const char *alertFlag;

    printf("\n--- [8] EQUIPMENT INVENTORY STOCK REPORT ---\n");
    if (fp == NULL) {
        printf("[i] No inventory data available to generate report.\n");
        return;
    }
    printf("Report Filter Options:\n  1. Complete Report\n  2. Filter by ID or Name\n  3. Show Low-Stock Items (< %d units)\nSelect filter (1-3): ", LOW_STOCK_THRESHOLD);
    if (scanf("%d", &filterOption) != 1 || filterOption < 1 || filterOption > 3) {
        printf("[!] Invalid selection. Defaulting to Complete Report.\n"); filterOption = 1;
    }
    foClearBuffer();
    if (filterOption == 2) foReadString("Enter Equipment ID or Name to filter: ", filterKeyword, FO_MAX_NAME);

    printf("\n====================================================================================================\n");
    printf("                         SDAMS EQUIPMENT STOCK & VALUATION REPORT\n");
    printf("====================================================================================================\n");
    printf("%-8s | %-20s | %-10s | %-6s | %-9s | %-11s | %-16s\n", "EQ ID", "Name", "Category", "Qty", "Unit(RM)", "Total(RM)", "Stock Alert");
    printf("----------------------------------------------------------------------------------------------------\n");
    while (fscanf(fp, "%9[^|]|%49[^|]|%29[^|]|%d|%lf|%19[^\n]\n", eq.equipmentID, eq.name, eq.category, &eq.quantity, &eq.unitCost, eq.status) == 6) {
        if (filterOption == 2 && strcmp(eq.equipmentID, filterKeyword) != 0 && strstr(eq.name, filterKeyword) == NULL) continue;
        if (filterOption == 3 && eq.quantity >= LOW_STOCK_THRESHOLD) continue;

        metric.itemValuation = eq.quantity * eq.unitCost;
        alertFlag = (eq.quantity < LOW_STOCK_THRESHOLD) ? "[! LOW STOCK !]" : "OK";
        printf("%-8s | %-20s | %-10s | %-6d | %-9.2f | %-11.2f | %-16s\n", eq.equipmentID, eq.name, eq.category, eq.quantity, eq.unitCost, metric.itemValuation, alertFlag);

        totalItems += eq.quantity;
        totalValuation += metric.itemValuation;
        if (eq.quantity < LOW_STOCK_THRESHOLD) lowStockCount++;
    }
    printf("====================================================================================================\n");
    printf("SUMMARY METRICS:\n  * Total Physical Units Listed : %d units\n  * Total Inventory Valuation   : RM %.2f\n  * Low-Stock Items Flagged     : %d item(s) (Threshold < %d units)\n", totalItems, totalValuation, lowStockCount, LOW_STOCK_THRESHOLD);
    printf("====================================================================================================\n");
    fclose(fp);
}
