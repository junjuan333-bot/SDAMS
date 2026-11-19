#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "manager.h"
struct User {
    char id[50];
    char pass[50];
    char role[50];
}; 
struct Inventory {
    char id[10];
    char name[50];
    char category[30];
    int quantity;
    double unitCost; 
    char status[20]; 
};
struct FacilityProblem {
    char problemID[10];
    char description[100];
    char status[20];
    char reportedBy[50];
    char reportedDate[20];
    char facilityName[50];
};
struct instructor{
     int id;
     char name[50];
    char course[50];
    char contact[15];
    char status[20];
    };
struct student{
    int id;
    char name[50];
    char course[50];
    char contact[15];
    char status[20];
    };
struct payment{
    int Id;
    char studentid[50];
     double amount;
    char paymentDate[20];
    };
struct Admin{
    char id[50];
    char name[50];
    char role[50];
    char status[20];
    char pass[50];
};
struct RefundRequest {
    char refundID[20];
    char studentID[20];
    char studentName[50];
    double amount;
    char reason[100];
    char status[20]; 
};
int isStrongPassword(char pass[]) {
    int hasUpper = 0;
    int hasLower = 0;
    int hasDigit = 0;
    int hasSpecial = 0;
    int len = strlen(pass);

    if (len < 8) {
        return 0;
    }

    for (int i = 0; i < len; i++) {
        if (pass[i] == '|') {
            return 0;
        }
        if (isupper(pass[i])) {
            hasUpper = 1;
        } else if (islower(pass[i])) {
            hasLower = 1;
        } else if (isdigit(pass[i])) {
            hasDigit = 1;
        } else if (!isalnum(pass[i]) && pass[i] != '|' && pass[i] != ' ') {
            hasSpecial = 1;
        }
    }

    return (hasUpper && hasLower && hasDigit && hasSpecial);
}
int manager_log_system(char loggedInId[]) {
    struct User input;
    struct User fileData;
    int found = 0;
    printf("\n--- Manager Login ---\n");
    printf("Enter Manager ID: ");
    scanf(" %[^\n]", input.id); 
    printf("Enter Manager Password: ");
    scanf(" %[^\n]", input.pass);

    FILE *filemanager = fopen("manager.txt", "r");
    if (filemanager == NULL) {
        printf("Error opening file.\n");
        return 0;
    }

    while (fscanf(filemanager, "%[^|]|%[^|]|%[^\n]\n", fileData.id, fileData.pass, fileData.role) != EOF) {
        if (strcmp(input.id, fileData.id) == 0 && 
            strcmp(input.pass, fileData.pass) == 0 && 
            strcmp(fileData.role, "Manager") == 0) {
            found = 1;
            strcpy(loggedInId, fileData.id); 
            printf("Login Successful! Role: %s\n", fileData.role);
            break;
        }
    }

    fclose(filemanager);

    if (found == 0) {
        printf("Invalid Manager ID or Password.\n");
    }

    return found;
}

int manager_register() {
    struct User newUser;
    struct User checkUser;
   
    printf("\n--- Register New Manager ---\n");
    printf("Enter Manager ID: ");
    scanf(" %[^\n]", newUser.id); 
    FILE *checkFile = fopen("manager.txt", "r");
    if (checkFile != NULL) {
        while (fscanf(checkFile, "%[^|]|%[^|]|%[^\n]\n", checkUser.id, checkUser.pass, checkUser.role) != EOF) {
            if (strcmp(newUser.id, checkUser.id) == 0) {
                printf("Error: Manager ID '%s' already exists!\n", newUser.id);
                fclose(checkFile);
                return 0; 
            }
        }
        fclose(checkFile); 
    }
    do {
        printf("Enter Manager Password (min 8 chars, 1 upper, 1 lower, 1 digit, 1 special, no '|' or press 0 to exit): ");
        scanf(" %[^\n]", newUser.pass);

        if (strcmp(newUser.pass, "0") == 0) {
            printf("Registration cancelled.\n");
            return 0;
        }

        if (!isStrongPassword(newUser.pass)) {
            printf("Weak password! Please make sure it meets all requirements.\n");
        }
    } while (!isStrongPassword(newUser.pass));

    strcpy(newUser.role, "Manager");
    
    FILE *filemanager = fopen("manager.txt", "a");
    if (filemanager == NULL) {
        printf("Error opening file.\n");
        return 0;
    }
    fprintf(filemanager, "%s|%s|%s\n", newUser.id, newUser.pass, newUser.role);
    fclose(filemanager);

    printf("Manager registered successfully!\n");
    return 1;

}

int change_password(char currentId[]) {
    struct User fileData;
    char oldPass[50];
    char newPass[50];
    int found = 0;
    printf("\n--- Change Manager Password ---\n");
    printf("Enter Current Password (or 0 to cancel): ");
    scanf(" %[^\n]", oldPass);
    if (strcmp(oldPass, "0") == 0) {
    printf("Password change cancelled.\n");
    return 0;
    }

    FILE *filemanager = fopen("manager.txt", "r");
    if (filemanager == NULL) {
        printf("Error opening file.\n");
        return 0;
    }
    while (fscanf(filemanager, "%[^|]|%[^|]|%[^\n]\n", fileData.id, fileData.pass, fileData.role) != EOF) {
        if (strcmp(currentId, fileData.id) == 0 && 
            strcmp(oldPass, fileData.pass) == 0 && 
            strcmp(fileData.role, "Manager") == 0) {
            found = 1; 
            break;
        }
    }
    fclose(filemanager);
    if (found == 0) {
        printf("Invalid  Password.\n");
        return 0;
    }
    do {
        printf("Enter New Password (min 8 chars, 1 upper, 1 lower, 1 digit, 1 special, no '|' or press 0 to exit): ");
        scanf(" %[^\n]", newPass);

        if (strcmp(newPass, "0") == 0) {
            printf("Password change cancelled.\n");
            return 0;
        }
        if (strcmp(newPass, oldPass) == 0) {
            printf("New password cannot be the same as old password!\n");
            continue;
        }
        if (!isStrongPassword(newPass)) {
            printf("Weak password! Please make sure it meets all requirements.\n");
        }
    } while (!isStrongPassword(newPass) || strcmp(newPass, oldPass) == 0);
    
    filemanager = fopen("manager.txt", "r");
    FILE *tempFile = fopen("temp.txt", "w");
    if (filemanager == NULL || tempFile == NULL) {
        printf("Error updating file.\n");
        if (filemanager != NULL) fclose(filemanager);
        if (tempFile != NULL) fclose(tempFile);
        return 0;
    }
    while (fscanf(filemanager, "%[^|]|%[^|]|%[^\n]\n", fileData.id, fileData.pass, fileData.role) != EOF) {
        if (strcmp(currentId, fileData.id) == 0 && strcmp(fileData.role, "Manager") == 0) {

            fprintf(tempFile, "%s|%s|%s\n", fileData.id, newPass, fileData.role);
        } else {
    
            fprintf(tempFile, "%s|%s|%s\n", fileData.id, fileData.pass, fileData.role);
        }
    }
    fclose(filemanager);
    fclose(tempFile);
    remove("manager.txt");
    rename("temp.txt", "manager.txt");
    printf("Password changed successfully.\n");
    return 1;
}
int isSameIgnoreCase(char s1[], char s2[]) {
    int i = 0;
    while (s1[i] != '\0' && s2[i] != '\0') {
        if (tolower((unsigned char)s1[i]) != tolower((unsigned char)s2[i])) {
            return 0;
        }
        i++;
    }
    return (s1[i] == '\0' && s2[i] == '\0');
}
double getRestockUnitCost(char targetId[]) {
    struct Inventory temp;
    FILE *fp = fopen("restock_needed.txt", "r");
    if (fp == NULL) {
        return -1.0;
    }
    while (fscanf(fp, "%[^|]|%[^|]|%[^|]|%d|%lf|%[^\n]\n",
                  temp.id, temp.name, temp.category,
                  &temp.quantity, &temp.unitCost, temp.status) == 6) {
        if (isSameIgnoreCase(targetId, temp.id) ==1) {
            fclose(fp);
            return temp.unitCost; 
        }
    }
    fclose(fp);
    return -1.0; 
}
int approve_Restock(char currentId[]){
    struct Inventory fileData;
    int needRestock = 0;
    char approveId[10];
    int restockQuantity;
    double unitCost = -1.0;

    FILE*filestock=fopen("inventory.txt","r");
    if(filestock==NULL){
        printf("Error opening file.\n");
        return 0;
    }
    FILE *restockneededFile = fopen("restock_needed.txt", "w");
    if (restockneededFile == NULL) {
        printf("Error opening restock_needed.txt for writing.\n");
        fclose(filestock);
        return 0;
    }
    printf("\n=================================== Equipment Needing Restock Approval ===================================\n");
    printf("%-8s | %-22s | %-16s | %-8s | %-12s | %-15s\n",
           "Item ID", "Name", "Category", "Quantity", "Unit Cost", "Status");
    printf("----------------------------------------------------------------------------------------------------------\n");

    while (fscanf(filestock, "%19[^|]|%49[^|]|%29[^|]|%d|%lf|%19[^\n]\n",
                  fileData.id, fileData.name, fileData.category,
                  &fileData.quantity, &fileData.unitCost, fileData.status) == 6) {
        if (fileData.quantity < 5 || isSameIgnoreCase(fileData.status, "Low Stock") == 1 || isSameIgnoreCase(fileData.status, "Damaged") == 1) {
            needRestock++;
          
            fprintf(restockneededFile, "%s|%s|%s|%d|%.2f|%s\n",
                    fileData.id, fileData.name, fileData.category,
                    fileData.quantity, fileData.unitCost, fileData.status);
          
            printf("%-8s | %-22s | %-16s | %-8d | RM %-9.2f | %-15s\n",
                   fileData.id, fileData.name, fileData.category,
                   fileData.quantity, fileData.unitCost, fileData.status);
        }
    }
    fclose(filestock);
    fclose(restockneededFile);
    if(needRestock==0){
        printf("No equipment needs restock approval.\n");
        remove("restock_needed.txt");
        return 0;
    }
   
    do{
        printf("\nEnter the ID of the equipment to approve restock(press 0 to cancel): ");
        scanf(" %[^\n]", approveId);
        while (getchar() != '\n');
        for (int i = 0; approveId[i] != '\0'; i++) {
            approveId[i] = toupper((unsigned char)approveId[i]);
        }   
        if(strcmp(approveId,"0")==0){
            printf("Restock approval cancelled.\n");
            remove("restock_needed.txt");
            return 0;
        }
        unitCost = getRestockUnitCost(approveId);
        if (unitCost > 0) {
            break; 
        } else {
            printf("Invalid equipment ID. Please enter a valid ID from the list above.\n");
        }
    } while (1);

    while (1) {
        printf("Enter the quantity to restock for equipment ID %s (press 0 to cancel): ", approveId);
        if (scanf("%d", &restockQuantity) != 1) {
            while (getchar() != '\n');
            printf("Invalid input! Please enter a valid number.\n");
            continue;
        }
        while (getchar() != '\n');

        if (restockQuantity == 0) {
            printf("Restock approval cancelled.\n");
            remove("restock_needed.txt");
            return 0;
        }
        if (restockQuantity < 0) {
            printf("Invalid quantity. Please enter a positive number.\n");
            continue;
        }
        break;
    }

    double totalCost = restockQuantity * unitCost;
    char confirm;
    printf("\nTotal Restock Cost: RM %.2f (%d units x RM %.2f)\n", totalCost, restockQuantity, unitCost);
    printf("Confirm approval? (Y/N): ");
    scanf(" %c", &confirm);
    while (getchar() != '\n');

    if (confirm != 'Y' && confirm != 'y') {
        printf("Restock approval cancelled.\n");
        remove("restock_needed.txt");
        return 0;
    }
    FILE *inFp = fopen("inventory.txt", "r");
    FILE *tempFp = fopen("temp.txt", "w");
    if (inFp == NULL || tempFp == NULL) {
        printf("Error updating inventory file.\n");
        if (inFp) fclose(inFp);
        if (tempFp) fclose(tempFp);
        remove("restock_needed.txt");
        return 0;
    }

    while (fscanf(inFp, "%[^|]|%[^|]|%[^|]|%d|%lf|%[^\n]\n",
                  fileData.id, fileData.name, fileData.category,
                  &fileData.quantity, &fileData.unitCost, fileData.status) == 6) {
        if (isSameIgnoreCase(fileData.id, approveId) == 1) {
            fileData.quantity += restockQuantity;
            if (fileData.quantity >= 5) {
                strcpy(fileData.status, "Available");
            }
        }
        fprintf(tempFp, "%s|%s|%s|%d|%.2f|%s\n",
                fileData.id, fileData.name, fileData.category,
                fileData.quantity, fileData.unitCost, fileData.status);
    }

    fclose(inFp);
    fclose(tempFp);
    remove("inventory.txt");
    rename("temp.txt", "inventory.txt");
    remove("restock_needed.txt");

    printf("[OK] Restock approved for %s! Added %d units (Total: RM %.2f).\n",
           approveId, restockQuantity, totalCost);
    FILE *histFp = fopen("restock_history.txt", "a");
    if (histFp != NULL) {
        fprintf(histFp, "%s|%d|%.2f|%s\n", approveId, restockQuantity, totalCost, currentId);
        fclose(histFp);
    }
    return 1;
}

int Unresolved_Problems() {
    struct FacilityProblem fileData;
    int count=0;
FILE*fc=fopen("facility_problems.txt","r");
if(fc==NULL){
    printf("Error opening file.\n");
    return 0;
}
printf("\n====================================== Unresolved Facility Problems ======================================\n");
    printf("%-10s | %-20s | %-30s | %-15s | %-12s | %-12s\n",
           "Problem ID", "Facility Name", "Description", "Reported By", "Reported Date", "Status");
    printf("----------------------------------------------------------------------------------------------------------\n");

    while (fscanf(fc, "%9[^|]|%49[^|]|%99[^|]|%49[^|]|%19[^|]|%19[^\n]\n",
                  fileData.problemID, fileData.facilityName, fileData.description,
                  fileData.reportedBy, fileData.reportedDate, fileData.status) == 6) {
        if (isSameIgnoreCase(fileData.status, "Resolved") == 0) {
            printf("%-10s | %-20s | %-30s | %-15s | %-12s | %-12s\n",
                   fileData.problemID, fileData.facilityName, fileData.description,
                   fileData.reportedBy, fileData.reportedDate, fileData.status);
            count++;
        }
    }
fclose(fc);
if (count == 0) {
    printf("No unresolved facility problems found.\n");
} else {
    printf("\nTotal Unresolved Problems: %d\n", count);
}
return 1;
}
int overall_facility(){
    struct Inventory fileData;
    struct FacilityProblem fileDatae;
    int totalItems = 0, totalUnits = 0, alertCount = 0;
    double totalValue = 0.0;
    int totalProblems = 0, unresolvedCount = 0, resolvedCount = 0;
    FILE *fi = fopen("inventory.txt", "r");
    FILE *fp = fopen("facility_problems.txt", "r");

    if (fi == NULL || fp == NULL) {
        printf("Error opening files.\n");
        if (fi != NULL) fclose(fi);
        if (fp != NULL) fclose(fp);
        return 0;
    }
printf("\n====================================== INVENTORY OVERVIEW ======================================\n");
    printf("%-16s | %-6s | %-20s | %-15s | %-8s | %-10s | %-12s\n",
           "Alert", "ID", "Item Name", "Category", "Qty", "Unit Cost", "Status");
    printf("------------------------------------------------------------------------------------------------\n");

    while (fscanf(fi, "%19[^|]|%49[^|]|%29[^|]|%d|%lf|%19[^\n]\n",
                  fileData.id, fileData.name, fileData.category,
                  &fileData.quantity, &fileData.unitCost, fileData.status) == 6) {
        totalItems++;
        totalUnits += fileData.quantity;
        totalValue += fileData.quantity * fileData.unitCost;

        if (fileData.quantity < 5 || isSameIgnoreCase(fileData.status, "Low Stock") == 1 || isSameIgnoreCase(fileData.status, "Damaged") == 1) {
            printf("%-16s | %-6s | %-20s | %-15s | %-8d | RM %-7.2f | %-12s\n",
                   "[Restock Needed]", fileData.id, fileData.name, fileData.category,
                   fileData.quantity, fileData.unitCost, fileData.status);
            alertCount++;
        } else {
            printf("%-16s | %-6s | %-20s | %-15s | %-8d | RM %-7.2f | %-12s\n",
                   "[Normal]", fileData.id, fileData.name, fileData.category,
                   fileData.quantity, fileData.unitCost, fileData.status);
        }
    }
printf("\n=================================== FACILITY PROBLEMS OVERVIEW ===================================\n");
    printf("%-8s | %-18s | %-28s | %-14s | %-12s | %-12s\n",
           "Prob ID", "Facility", "Description", "Reported By", "Date", "Status");
    printf("--------------------------------------------------------------------------------------------------\n");

    while (fscanf(fp, "%9[^|]|%49[^|]|%99[^|]|%49[^|]|%19[^|]|%19[^\n]\n",
                  fileDatae.problemID, fileDatae.facilityName, fileDatae.description,
                  fileDatae.reportedBy, fileDatae.reportedDate, fileDatae.status) == 6) {
        
        totalProblems++;
        printf("%-8s | %-18s | %-28s | %-14s | %-12s | %-12s\n",
               fileDatae.problemID, fileDatae.facilityName, fileDatae.description,
               fileDatae.reportedBy, fileDatae.reportedDate, fileDatae.status);

        if (isSameIgnoreCase(fileDatae.status, "Resolved") == 0) {
            unresolvedCount++;
        } else {
            resolvedCount++;
        }
    }

    fclose(fi);
    fclose(fp);
    printf("\n====================== OVERALL SUMMARY =====================\n");
    printf("Total Equipment Types     : %d\n", totalItems);
    printf("Total Physical Units      : %d\n", totalUnits);
    printf("Total Inventory Valuation : RM %.2f\n", totalValue);
    printf("Low Stock / Damaged Items : %d\n", alertCount);
    printf("------------------------------------------------------------\n");
    printf("Total Facility Problems   : %d\n", totalProblems);
    printf("Unresolved Problems       : %d\n", unresolvedCount);
    printf("Resolved Problems         : %d\n", resolvedCount);
    printf("============================================================\n");
    return 1;
}
void view_restock_history(void) {
    char itemId[10], approvedBy[50];
    int qty, count = 0;
    double totalCost, grandTotal = 0.0;

    FILE *fp = fopen("restock_history.txt", "r");
    if (fp == NULL) {
        printf("\nNo restock approval history found yet.\n");
        return;
    }

    printf("\n================ RESTOCK APPROVAL HISTORY ================\n");
    printf("%-10s | %-8s | %-12s | %-15s\n", "Item ID", "Qty", "Total Cost", "Approved By");
    printf("----------------------------------------------------------\n");

    while (fscanf(fp, "%[^|]|%d|%lf|%[^\n]\n", itemId, &qty, &totalCost, approvedBy) == 4) {
        printf("%-10s | %-8d | RM %-9.2f | %-15s\n", itemId, qty, totalCost, approvedBy);
        count++;
        grandTotal += totalCost;
    }

    fclose(fp);
    printf("----------------------------------------------------------\n");
    printf("Total Approvals: %d | Cumulative Spend: RM %.2f\n", count, grandTotal);
    printf("==========================================================\n");
}
int view_Dojo_Executive_Summary(){
    struct instructor inst;
    struct student stu;
    struct payment pay;
    int totalInst = 0, activeInst = 0;
    int totalStu = 0, activeStu = 0;
    int totalPay = 0;
    double totalAmount = 0.0,totalrestockcost=0.0;
    FILE*fin=fopen("instructors.txt","r");
    if(fin==NULL){
        printf("Error opening file.\n");
        return 0;
    }
    FILE*fstu=fopen("students.txt","r");
    if(fstu==NULL){
        printf("Error opening file.\n");
        fclose(fin);
        return 0;
    }
    FILE*fpay=fopen("payments.txt","r");
    if(fpay==NULL){
        printf("Error opening file.\n");
        fclose(fin);
        fclose(fstu);
        return 0;
    }

    printf("\n==================== ACTIVE INSTRUCTORS ====================\n");
    printf("%-10s | %-15s | %-15s | %-15s\n", "ID", "Name", "Course/Role", "Phone Number");
    printf("------------------------------------------------------------\n");
    while (fscanf(fin, "%d %s %s %s %s", &inst.id, inst.name, inst.course, inst.contact, inst.status) == 5) {
        totalInst++;
        if (isSameIgnoreCase(inst.status, "Active") == 1) {
            activeInst++;
            printf("%-10d | %-15s | %-15s | %-15s\n", inst.id, inst.name, inst.course, inst.contact);
        }
    }
    printf("------------------------------------------------------------\n");
    printf("Total Instructors: %d (Active: %d | Inactive: %d)\n", totalInst, activeInst, totalInst - activeInst);

    printf("\n==================== ACTIVE STUDENTS ====================\n");
    printf("%-10s | %-15s | %-15s | %-15s\n", "ID", "Name", "Course", "Phone Number");
    printf("------------------------------------------------------------\n");
    while (fscanf(fstu, "%d %s %s %s %s", &stu.id, stu.name, stu.course, stu.contact, stu.status) == 5) {
        totalStu++;
        if (isSameIgnoreCase(stu.status, "Active") == 1) {
            activeStu++;
            printf("%-10d | %-15s | %-15s | %-15s\n", stu.id, stu.name, stu.course, stu.contact);
        }
    }
    printf("------------------------------------------------------------\n");
    printf("Total Students: %d (Active: %d | Inactive: %d)\n", totalStu, activeStu, totalStu - activeStu);

    fclose(fstu);
    fclose(fin);
    printf("\n==================== PAYMENT SUMMARY ====================\n");
    printf("%-10s | %-15s | %-15s | %-15s\n", "Payment ID", "Student ID", "Amount", "Payment Date");
    printf("------------------------------------------------------------\n");
    while (fscanf(fpay, "%d %s %lf %s", &pay.Id, pay.studentid, &pay.amount, pay.paymentDate) == 4) {
        totalPay++;
        totalAmount += pay.amount;
        printf("%-10d | %-15s | RM %-9.2f | %-15s\n",
               pay.Id, pay.studentid, pay.amount, pay.paymentDate);

    }
    fclose(fpay);
    FILE *fhist = fopen("restock_history.txt", "r");
    if (fhist != NULL) {
        char itemId[10], approvedBy[50];
        int qty;
        double cost;
        while (fscanf(fhist, "%[^|]|%d|%lf|%[^\n]\n", itemId, &qty, &cost, approvedBy) == 4) {
            totalrestockcost += cost;
        }
        fclose(fhist);
    }
    double totalRefund = 0.0;
    FILE *fr = fopen("refund_requests.txt", "r");
    if (fr != NULL) {
        struct RefundRequest refData;
        while (fscanf(fr, "%19[^|]|%19[^|]|%49[^|]|%lf|%99[^|]|%19[^\n]\n",
                      refData.refundID, refData.studentID, refData.studentName,
                      &refData.amount, refData.reason, refData.status) == 6) {
            if (isSameIgnoreCase(refData.status, "Approved") == 1) {
                totalRefund += refData.amount;
            }
        }
        fclose(fr);
    }

    double totalSalaryExpense = activeInst * 2800.00;
    double netProfit = totalAmount - totalSalaryExpense - totalrestockcost- totalRefund;

    printf("\n================ DOJO EXECUTIVE & FINANCIAL SUMMARY ================\n");
    printf("Total Enrolled Students   : %d (Active: %d | Inactive: %d)\n", totalStu, activeStu, totalStu - activeStu);
    printf("Total Fee Transactions    : %d\n", totalPay);
    printf("--------------------------------------------------------------------\n");
    printf("(+) Total Tuition Revenue : RM %.2f\n", totalAmount);
    printf("(-) Instructor Salaries   : RM %.2f (%d Active x RM 2800.00)\n", totalSalaryExpense, activeInst);
    printf("(-) Equipment Restock Cost: RM %.2f\n", totalrestockcost);
    printf("(-) Total Refund          : RM %.2f\n",totalRefund);
    printf("--------------------------------------------------------------------\n");
    printf("(=) Estimated Net Profit  : RM %.2f\n", netProfit);
    printf("====================================================================\n");

    return 1;
}
int view_Inactive_Personnel(){
    struct instructor inst;
    struct student stu;
    int totalinactive = 0;
    int totalinactivestu = 0;
    FILE *fin = fopen("instructors.txt", "r");
    if (fin == NULL) {
        printf("Error opening file.\n");
        return 0;
    }
    FILE *fstu = fopen("students.txt", "r");
    if (fstu == NULL) {
        printf("Error opening file.\n");
        fclose(fin);
        return 0;
    }
    printf("\n==================== INACTIVE INSTRUCTORS ====================\n");
    printf("%-10s | %-15s | %-15s | %-15s\n", "ID", "Name", "Course/Role", "Phone Number");
    printf("------------------------------------------------------------\n");
    while(fscanf(fin, "%d %s %s %s %s", &inst.id, inst.name, inst.course, inst.contact, inst.status) == 5) {
        
        if (isSameIgnoreCase(inst.status, "Not_Active") == 1) {
            printf("%-10d | %-15s | %-15s | %-15s\n", inst.id, inst.name, inst.course, inst.contact);
            totalinactive++;
        }
    }
    if (totalinactive == 0) {
            printf("All instructors are currently active.\n");
        }

    
    printf("------------------------------------------------------------\n");
    printf("Total Inactive Instructors: %d\n", totalinactive);

    printf("\n==================== INACTIVE STUDENTS ====================\n");
    printf("%-10s | %-15s | %-15s | %-15s\n", "ID", "Name", "Course", "Phone Number");
    printf("------------------------------------------------------------\n");
    while(fscanf(fstu, "%d %s %s %s %s", &stu.id, stu.name, stu.course, stu.contact, stu.status) == 5) {
        if (isSameIgnoreCase(stu.status, "Not_Active") == 1) {
            printf("%-10d | %-15s | %-15s | %-15s\n", stu.id, stu.name, stu.course, stu.contact);
            totalinactivestu++;
        }

    }
    if (totalinactivestu == 0) {
            printf("All students are currently active.\n");
        }
    printf("------------------------------------------------------------\n");
    printf("Total Inactive Students: %d\n", totalinactivestu);
    fclose(fstu);
    fclose(fin);
    return 1;

}
int view_payment_History(){
    struct payment pay;
    int totalPay = 0;
    double totalAmount = 0.0;

    FILE *fpay = fopen("payments.txt", "r");
    if (fpay == NULL) { 
        printf("Error opening file.\n");
        return 0;
    }
    printf("\n==================== PAYMENT HISTORY ====================\n");
    printf("%-10s | %-15s | %-15s | %-15s\n", "Payment ID", "Student ID", "Amount", "Payment Date");
    printf("------------------------------------------------------------\n");
    while (fscanf(fpay, "%d %s %lf %s", &pay.Id, pay.studentid, &pay.amount, pay.paymentDate) == 4) {
        totalPay++;
        totalAmount += pay.amount;
        printf("%-10d | %-15s | RM %-9.2f | %-15s\n",
               pay.Id, pay.studentid, pay.amount, pay.paymentDate);
}
if (totalPay == 0) {
        printf("No payment records found.\n");
    } else {
        printf("------------------------------------------------------------\n");
        printf("Total Payments: %d | Total Amount: RM %.2f\n", totalPay, totalAmount);
    }

    fclose(fpay);
    return 1;
}
int admin_register() {
    struct Admin newUser;
    struct Admin checkUser;
    int maxId = 0;
    int currentNum = 0;
    FILE *checkFile = fopen("admin.txt", "r");
    if (checkFile != NULL) {
        while (fscanf(checkFile, "%49[^|]|%49[^|]|%49[^|]|%49[^|]|%19[^\n]\n",
                      checkUser.id, checkUser.name, checkUser.role, checkUser.pass, checkUser.status) == 5) {
            if (sscanf(checkUser.id, "A%d", &currentNum) == 1) {
                if (currentNum > maxId) {
                    maxId = currentNum;
                }
            }
        }
        fclose(checkFile);
    }

    sprintf(newUser.id, "A%03d", maxId + 1);
    printf("\n--- Register New Admin ---\n");

    printf("Enter Admin Name: ");
    scanf(" %[^\n]", newUser.name);

    
    do {
        printf("Enter Admin Password (min 8 chars, 1 upper, 1 lower, 1 digit, 1 special, no '|' or press 0 to exit): ");
        scanf(" %[^\n]", newUser.pass);

        if (strcmp(newUser.pass, "0") == 0) {
            printf("Registration cancelled.\n");
            return 0;
        }

        if (!isStrongPassword(newUser.pass)) {
            printf("Weak password! Please make sure it meets all requirements.\n");
        }
    } while (!isStrongPassword(newUser.pass));

    strcpy(newUser.role, "Admin");
    strcpy(newUser.status, "Active");
    
    FILE *filemanager = fopen("admin.txt", "a");
    if (filemanager == NULL) {
        printf("Error opening file.\n");
        return 0;
    }
    fprintf(filemanager, "%s|%s|%s|%s|%s\n", newUser.id, newUser.name, newUser.role, newUser.pass, newUser.status);
    fclose(filemanager);

    printf("Admin registered successfully!\n");
    return 1;

}
int admin_fired_sys(){
    struct Admin fileData;
    char targetId[50];
    int found = 0;
    FILE *fadmin = fopen("admin.txt", "r");
    if (fadmin == NULL) {
        printf("Error opening file.\n");
        return 0;
    }
     printf("\n==================== CURRENT ADMIN LIST ====================\n");
    printf("%-10s | %-15s | %-15s | %-15s\n", "ID", "Name", "Role", "Status");
    while (fscanf(fadmin, "%[^|]|%[^|]|%[^|]|%[^|]|%[^\n]\n", fileData.id, fileData.name, fileData.role, fileData.pass, fileData.status) != EOF) {
    printf("%-10s | %-15s | %-15s | %-15s\n", fileData.id, fileData.name, fileData.role, fileData.status);
    }
    printf("Enter Admin Id (press 0 to exit): ");
    scanf(" %[^\n]", targetId);
    while (getchar() != '\n');

    if (strcmp(targetId, "0") == 0) {
        printf("Exiting Admin Management Menu...\n");
        fclose(fadmin);
        return 0;
    }
    rewind(fadmin);
    FILE *tempFile = fopen("temp.txt", "w");
    if (tempFile == NULL) {
        printf("Error opening temporary file.\n");
        fclose(fadmin);
        return 0;
    }

    while (fscanf(fadmin, "%[^|]|%[^|]|%[^|]|%[^|]|%[^\n]\n", fileData.id, fileData.name, fileData.role, fileData.pass, fileData.status) != EOF) {
        if (isSameIgnoreCase(fileData.id, targetId) == 1) {
            found = 1;

            } else {
            fprintf(tempFile, "%s|%s|%s|%s|%s\n", fileData.id, fileData.name, fileData.role, fileData.pass, fileData.status);
        }
    }
    fclose(fadmin);
    fclose(tempFile);

    if (found == 0) {
        printf("Admin not found.\n");
        remove("temp.txt");
        return 0;
    }

    remove("admin.txt");
    rename("temp.txt", "admin.txt");

    printf("Admin fired successfully!\n");
    return 1;
}
int manage_refund_sys() {
    struct RefundRequest fileData;
    char targetId[20];
    int found = 0;
    int pendingCount = 0;
    int decision = 0;

    FILE *frefund = fopen("refund_requests.txt", "r");
    if (frefund == NULL) {
        printf("No refund requests file found (refund_requests.txt).\n");
        return 0;
    }

    printf("\n=================================== PENDING REFUND REQUESTS ===================================\n");
    printf("%-10s | %-10s | %-18s | %-12s | %-25s | %-10s\n",
           "Refund ID", "Student ID", "Student Name", "Amount", "Reason", "Status");
    printf("-----------------------------------------------------------------------------------------------\n");

    while (fscanf(frefund, "%19[^|]|%19[^|]|%49[^|]|%lf|%99[^|]|%19[^\n]\n",
                  fileData.refundID, fileData.studentID, fileData.studentName,
                  &fileData.amount, fileData.reason, fileData.status) == 6) {
        if (isSameIgnoreCase(fileData.status, "Pending") == 1) {
            printf("%-10s | %-10s | %-18s | RM %-9.2f | %-25s | %-10s\n",
                   fileData.refundID, fileData.studentID, fileData.studentName,
                   fileData.amount, fileData.reason, fileData.status);
            pendingCount++;
        }
    }
    printf("-----------------------------------------------------------------------------------------------\n");

    if (pendingCount == 0) {
        printf("No pending refund requests at the moment.\n");
        fclose(frefund);
        return 0;
    }

    printf("Enter Refund ID to process (press 0 to exit): ");
    scanf(" %19[^\n]", targetId);
    while (getchar() != '\n');

    if (strcmp(targetId, "0") == 0) {
        printf("Exiting Refund Approval Menu...\n");
        fclose(frefund);
        return 0;
    }

    printf("Action for [%s] -> [1] Approve Refund  [2] Reject Request  [0] Cancel: ", targetId);
    if (scanf("%d", &decision) != 1 || (decision != 1 && decision != 2)) {
        while (getchar() != '\n');
        printf("Operation cancelled.\n");
        fclose(frefund);
        return 0;
    }
    while (getchar() != '\n');

    rewind(frefund);
    FILE *tempFile = fopen("temp.txt", "w");
    if (tempFile == NULL) {
        printf("Error opening temporary file.\n");
        fclose(frefund);
        return 0;
    }

    while (fscanf(frefund, "%19[^|]|%19[^|]|%49[^|]|%lf|%99[^|]|%19[^\n]\n",
                  fileData.refundID, fileData.studentID, fileData.studentName,
                  &fileData.amount, fileData.reason, fileData.status) == 6) {
        if (isSameIgnoreCase(fileData.refundID, targetId) == 1 && isSameIgnoreCase(fileData.status, "Pending") == 1) {
            found = 1;
            if (decision == 1) {
                strcpy(fileData.status, "Approved");
            } else {
                strcpy(fileData.status, "Rejected");
            }
        }
        fprintf(tempFile, "%s|%s|%s|%.2f|%s|%s\n",
                fileData.refundID, fileData.studentID, fileData.studentName,
                fileData.amount, fileData.reason, fileData.status);
    }

    fclose(frefund);
    fclose(tempFile);

    if (found == 0) {
        printf("Pending Refund ID '%s' not found.\n", targetId);
        remove("temp.txt");
        return 0;
    }

    remove("refund_requests.txt");
    rename("temp.txt", "refund_requests.txt");

    if (decision == 1) {
        printf("Refund [%s] APPROVED! Net profit will automatically deduct this refund.\n", targetId);
    } else {
        printf("Refund [%s] REJECTED.\n", targetId);
    }
    return 1;
}
int view_Student_Enrollment(){
    struct student stu;
    int totalStu = 0, activeStu = 0;

    FILE*fstu=fopen("students.txt","r");
    if(fstu==NULL){
        printf("Error opening file.\n");
        return 0;
    }

    printf("\n==================== ACTIVE STUDENTS ====================\n");
    printf("%-10s | %-15s | %-15s | %-15s\n", "ID", "Name", "Course", "Phone Number");
    printf("------------------------------------------------------------\n");
    while (fscanf(fstu, "%d %s %s %s %s", &stu.id, stu.name, stu.course, stu.contact, stu.status) == 5) {
        totalStu++;
        if (isSameIgnoreCase(stu.status, "Active") == 1) {
            activeStu++;
            printf("%-10d | %-15s | %-15s | %-15s\n", stu.id, stu.name, stu.course, stu.contact);
        }
    }
    printf("------------------------------------------------------------\n");
    printf("Total Students: %d (Active: %d | Inactive: %d)\n", totalStu, activeStu, totalStu - activeStu);

    fclose(fstu);
  
    return 1;
}
int view_refund_his(){
    struct RefundRequest fileData;
    FILE*fhistory=fopen("refund_requests.txt", "r");
    if (fhistory == NULL) {
    printf("No refund requests file found (refund_requests.txt).\n");
    return 0;
    }
    printf("\n===================================  REFUND REQUESTS HISTORY ===================================\n");
    printf("%-10s | %-10s | %-18s | %-12s | %-25s | %-10s\n",
           "Refund ID", "Student ID", "Student Name", "Amount", "Reason", "Status");
    printf("-----------------------------------------------------------------------------------------------\n");

    while (fscanf(fhistory, "%19[^|]|%19[^|]|%49[^|]|%lf|%99[^|]|%19[^\n]\n",
                  fileData.refundID, fileData.studentID, fileData.studentName,
                  &fileData.amount, fileData.reason, fileData.status) == 6) {
            printf("%-10s | %-10s | %-18s | RM %-9.2f | %-25s | %-10s\n",
                   fileData.refundID, fileData.studentID, fileData.studentName,
                   fileData.amount, fileData.reason, fileData.status);
            }
    fclose(fhistory);
    return 1;
}
void student_oversight_menu() {
    int choice;

    do {
        printf("\n==================================================\n");
        printf("       MANAGER - STUDENT & REFUND OVERSIGHT       \n");
        printf("==================================================\n");
        printf("1. View Student Enrollment & Dropout Audit\n");
        printf("2. Approve / Reject Pending Refund Requests\n");
        printf("3. View All Refund History\n");
        printf("4. View Payment History\n");
        printf("0. Back to Manager Main Menu\n");
        printf("--------------------------------------------------\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            printf("Invalid input. Please enter a number.\n");
            choice = -1;
            continue;
        }
        while (getchar() != '\n');

        switch (choice) {
            case 1:
                view_Student_Enrollment();
                break;
            case 2:
                manage_refund_sys(); 
                break;
            case 3:
                view_refund_his();
                break;
            case 4:
                view_payment_History();
                break;
            case 0:
                printf("Returning to Manager Main Menu...\n");
                break;
            default:
                printf("Invalid option! Please select 0 - 4.\n");
        }
    } while (choice != 0);
}
int manager_Admin_Menu(char currentId[]){
    int choice;
    do {
        printf("\n====================================\n");
        printf("  SDAMS - Admin Management\n");
        printf("  Manager: %s\n", currentId);
        printf("====================================\n");
        printf("1. Register New Admin\n");
        printf("2. Fire Admin\n");
        printf("0. Back to Manager Menu\n");
        printf("====================================\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n'); 
            choice = -1;   
            } else {
            while (getchar() != '\n');
        }             
           switch (choice) {

            case 1:
                admin_register();
                break;
            case 2:
                admin_fired_sys();
                break;

            case 0:
                printf("Exiting Admin Management Menu...\n");
                break;
            default:
                printf("Invalid choice! Please enter a valid option.\n");
           }
        }while (choice != 0);
    
        return 1;
    }
   
void facility_Equipment_Menu(char currentId[]){
    int choice;
    do {
        printf("\n====================================\n");
        printf("   SDAMS - Facility Equipment Menu\n");
        printf("====================================\n");
        printf("1. Approve Equipment Restock\n");
        printf("2. View Unresolved Facility Problems\n");
        printf("3. View Overall Facility & Asset Summary\n");
        printf("4. View Restock Approval History\n");
        printf("0. Back to Manager Menu\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n'); 
            choice = -1;   
            } else {
            while (getchar() != '\n');
        }             
           switch (choice) {
            case 1:
                approve_Restock(currentId);
                break;
            case 2:
                Unresolved_Problems();
                break;
            case 3:
                overall_facility();
                break;
            case 4:
                view_restock_history();
                break;
            case 0:
                printf("Exiting Facility Equipment Menu...\n");
                break;
            default:
                printf("Invalid choice! Please enter a valid option.\n");
        }

    } while (choice != 0);

    return;
}
void manager_main_menu(char currentId[]) {
    int choice;
    do {
        printf("\n============================================\n");
        printf("      SDAMS - MANAGER PORTAL (%s)\n", currentId);
        printf("============================================\n");
        printf("1. Facilities & Equipment Oversight\n");
        printf("2. Admin & Personnel Oversight\n");
        printf("3. Student & Refund Oversight\n");
        printf("4. View Dojo Executive Summary\n");
        printf("5. View Inactive Personnel\n");
        printf("6. Change Manager Password\n");
        printf("7. Register new manager Id\n");
        printf("0. Logout\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            choice = -1;
        } else {
            while (getchar() != '\n');
        }

        switch (choice) {
            case 1:
                facility_Equipment_Menu(currentId);
                break;
            case 2:
                manager_Admin_Menu(currentId);
                break;
            case 3:
                student_oversight_menu();
                break;
            case 4:
                view_Dojo_Executive_Summary();
                break;
            case 5:
                view_Inactive_Personnel();
                break;
            case 6:
                change_password(currentId);
                break;
            case 7:
                manager_register();
            case 0:
                printf("\nLogging out Manager [%s]...\n", currentId);
                break;
            default:
                printf("\nInvalid choice! Please select 0 - 7.\n");
        }
    } while (choice != 0);
}
