#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "facilities_officer.h"

#define FO_MAX_ID 10
#define FO_MAX_NAME 50
#define FO_MAX_CATEGORY 30
#define FO_MAX_DESC 100
#define FO_MAX_STATUS 20
#define FO_MAX_DATE 15
#define LOW_STOCK_THRESHOLD 5

#define INVENTORY_FILE "inventory.txt"
#define PROBLEMS_FILE "facility_problems.txt"
#define FO_TEMP_FILE "fo_temp.txt"

/* Structure Declarations */
typedef struct {
    char equipmentID[FO_MAX_ID];     
    char name[FO_MAX_NAME];          
    char category[FO_MAX_CATEGORY];  
    int quantity;                    
    double unitCost;                 
    char status[FO_MAX_STATUS];      
} Equipment;

typedef struct {
    char problemID[FO_MAX_ID];       
    char facilityName[FO_MAX_NAME];  
    char description[FO_MAX_DESC];   
    char reportedBy[FO_MAX_NAME];    
    char dateReported[FO_MAX_DATE];  
    char status[FO_MAX_STATUS];      
} FacilityProblem;

/* Function Prototypes */
void facilitiesOfficerMenu(void);
void addEquipment(void);
void viewEquipment(void);
void searchEquipment(void);
void updateEquipment(void);
void viewFacilityProblems(void);
void searchFacilityProblems(void);
void updateProblemStatus(void);
void generateStockReport(void);

void foClearBuffer(void);
void foReadString(const char *prompt, char *buffer, int size);
int foValidateID(const char *id, const char *prefix);
int isEquipmentIDExists(const char *id);

/* Helper Functions */
void foClearBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void foReadString(const char *prompt, char *buffer, int size) {
    printf("%s", prompt);
    if (fgets(buffer, size, stdin) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        } else {
            foClearBuffer();
        }
    }
}

int foValidateID(const char *id, const char *prefix) {
    if (strlen(id) != 5) return 0;
    if (strncmp(id, prefix, 2) != 0) return 0;
    if (!isdigit((unsigned char)id[2]) || !isdigit((unsigned char)id[3]) || !isdigit((unsigned char)id[4])) return 0;
    return 1;
}

int isEquipmentIDExists(const char *id) {
    FILE *fp = fopen(INVENTORY_FILE, "r");
    if (fp == NULL) return 0;
    Equipment temp;
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

/* Facilities Officer Login */
int facilitiesOfficerLogin() {
    char username[20];
    char password[20];
    
    printf("\n========================================\n");
    printf("|        Facilities Officer LOGIN         |\n");
    printf("========================================\n");
    printf("Enter Username (default: facilities): ");
    scanf("%s", username);
    printf("Enter Password (default: 1234): ");
    scanf("%s", password);
    
    if (strcmp(username, "facilities") == 0 && strcmp(password, "1234") == 0) {
        printf("\nLogin Successful! Welcome Facilities Officer.\n");
        return 1;
    } else {
        printf("\nInvalid Credentials! Access Denied.\n");
        return 0;
    }
}

/* Navigation Dashboard */
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

/* Module A implementations */
void addEquipment(void) {
    Equipment eq;
    FILE *fp;
    printf("\n--- [1] ADD NEW EQUIPMENT ---\n");

    while (1) {
        foReadString("Enter Equipment ID (Format EQxxx, e.g., EQ001): ", eq.equipmentID, FO_MAX_ID);
        if (!foValidateID(eq.equipmentID, "EQ")) {
            printf("[!] Error: ID must start with 'EQ' followed by 3 digits (e.g., EQ001).\n");
            continue;
        }
        if (isEquipmentIDExists(eq.equipmentID)) {
            printf("[!] Error: Equipment ID '%s' already exists!\n", eq.equipmentID);
            continue;
        }
        break;
    }

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
        printf("[i] Quantity is below %d. Status automatically set to 'Low Stock'.\n", LOW_STOCK_THRESHOLD);
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

/* Module B implementations */
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

/* Module C implementations */
void generateStockReport(void) {
    FILE *fp = fopen(INVENTORY_FILE, "r");
    Equipment eq;
    int filterOption, totalItems = 0, lowStockCount = 0;
    char filterKeyword[FO_MAX_NAME] = "";
    double totalValuation = 0.0;
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

        double itemValuation = eq.quantity * eq.unitCost;
        const char *alertFlag = (eq.quantity < LOW_STOCK_THRESHOLD) ? "[! LOW STOCK !]" : "OK";
        printf("%-8s | %-20s | %-10s | %-6d | %-9.2f | %-11.2f | %-16s\n", eq.equipmentID, eq.name, eq.category, eq.quantity, eq.unitCost, itemValuation, alertFlag);

        totalItems += eq.quantity;
        totalValuation += itemValuation;
        if (eq.quantity < LOW_STOCK_THRESHOLD) lowStockCount++;
    }
    printf("====================================================================================================\n");
    printf("SUMMARY METRICS:\n  * Total Physical Units Listed : %d units\n  * Total Inventory Valuation   : RM %.2f\n  * Low-Stock Items Flagged     : %d item(s) (Threshold < %d units)\n", totalItems, totalValuation, lowStockCount, LOW_STOCK_THRESHOLD);
    printf("====================================================================================================\n");
    fclose(fp);
}
