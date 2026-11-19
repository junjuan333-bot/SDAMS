#include <stdio.h>
#include <string.h>
#include <ctype.h>
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
    printf("\n--- Equipment Needing Restock Approval ---\n");
    while(fscanf(filestock,"%[^|]|%[^|]|%[^|]|%d|%lf|%[^\n]\n",fileData.id,fileData.name,fileData.category,&fileData.quantity,&fileData.unitCost,fileData.status)==6){
        if(fileData.quantity < 5||isSameIgnoreCase(fileData.status, "Low Stock") == 1||isSameIgnoreCase(fileData.status, "Damaged") == 1){
            printf("Item ID: %s| Name: %s| Category: %s| Quantity: %d| Unit Cost: %.2lf| Status: %s\n",fileData.id,fileData.name,fileData.category,fileData.quantity,fileData.unitCost,fileData.status);
            needRestock ++;
    
            fprintf(restockneededFile, "%s|%s|%s|%d|%.2lf|%s\n", fileData.id, fileData.name, fileData.category, fileData.quantity, fileData.unitCost, fileData.status);
  
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
while (fscanf(fc, "%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^\n]\n",
              fileData.problemID, fileData.facilityName, fileData.description,
              fileData.reportedBy, fileData.reportedDate, fileData.status) == 6) {
    if ((isSameIgnoreCase(fileData.status, "Resolved") == 0)) {
        printf("\n-----------------------------\n");
        printf("Problem ID: %s\n", fileData.problemID);
        printf("Description: %s\n", fileData.description);
        printf("Status: %s\n", fileData.status);
        printf("Reported By: %s\n", fileData.reportedBy);
        printf("Reported Date: %s\n", fileData.reportedDate);
        printf("Facility Name: %s\n", fileData.facilityName);
        printf("-----------------------------\n");
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
    printf("\n==================== INVENTORY OVERVIEW ====================\n");
    while(fscanf(fi, "%[^|]|%[^|]|%[^|]|%d|%lf|%[^\n]\n",
                  fileData.id, fileData.name, fileData.category,
                  &fileData.quantity, &fileData.unitCost, fileData.status) == 6) {
                    totalItems++;
        totalUnits += fileData.quantity;
        totalValue += fileData.quantity * fileData.unitCost;
        if (fileData.quantity < 5 || (isSameIgnoreCase(fileData.status, "Low Stock") == 1 || isSameIgnoreCase(fileData.status, "Damaged") == 1)) {
            printf("\n[Restock Needed] %s (%s) - Quantity: %d, Status: %s\n",
                   fileData.name, fileData.id, fileData.quantity, fileData.status);
            alertCount++;
            
        }
    else {
        printf("[Normal]         %s | %s | %s | %d | RM %.2f | %s\n",
                   fileData.id, fileData.name, fileData.category,
                   fileData.quantity, fileData.unitCost, fileData.status);
    }
}
printf("\n================ FACILITY PROBLEMS OVERVIEW ================\n");
    while (fscanf(fp, "%9[^|]|%49[^|]|%99[^|]|%49[^|]|%19[^|]|%19[^\n]\n",
                  fileDatae.problemID, fileDatae.facilityName, fileDatae.description,
                  fileDatae.reportedBy, fileDatae.reportedDate, fileDatae.status) == 6) {
        
        totalProblems++;
        printf("%s | %s | %s | %s | %s | %s\n",
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

    printf("\n==================== STUDENT ENROLLMENT ====================\n");
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

    double totalSalaryExpense = activeInst * 2800.00;
    double netProfit = totalAmount - totalSalaryExpense - totalrestockcost;

    printf("\n================ DOJO EXECUTIVE & FINANCIAL SUMMARY ================\n");
    printf("Total Enrolled Students   : %d (Active: %d | Inactive: %d)\n", totalStu, activeStu, totalStu - activeStu);
    printf("Total Fee Transactions    : %d\n", totalPay);
    printf("--------------------------------------------------------------------\n");
    printf("(+) Total Tuition Revenue : RM %.2f\n", totalAmount);
    printf("(-) Instructor Salaries   : RM %.2f (%d Active x RM 2800.00)\n", totalSalaryExpense, activeInst);
    printf("(-) Equipment Restock Cost: RM %.2f\n", totalrestockcost);
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
    printf("Enter Admin Id (press 0 to exit): ");
    scanf(" %[^\n]", targetId);
    while (getchar() != '\n');

    if (strcmp(targetId, "0") == 0) {
        printf("Exiting Admin Management Menu...\n");
        fclose(fadmin);
        return 0;
    }
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

int manager_Admin_Menu(char currentId[]){
    int choice;
    do {
        printf("\n====================================\n");
        printf("  SDAMS - Admin & Dojo Oversight\n");
        printf("  Manager: %s\n", currentId);
        printf("====================================\n");
        printf("1. View Dojo Executive Summary\n");
        printf("2. View Inactive Personnel\n");
        printf("3. View Payment History\n");
        printf("4. Register New Admin\n");
        printf("5. Fire Admin\n");
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
                view_Dojo_Executive_Summary();
                break;
            case 2:
                view_Inactive_Personnel();
                break;
            case 3:
                view_payment_History();
                break;
            case 4:
                admin_register();
                break;
            case 5:
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
void temp_manager_main_menu(char currentId[]) {
    int choice;
    do {
        printf("\n============================================\n");
        printf("      SDAMS - MANAGER PORTAL (%s)\n", currentId);
        printf("============================================\n");
        printf("1. Facilities & Equipment Oversight\n");
        printf("2. Dojo & Admin Oversight (Summary & Audit)\n");
        printf("0. Logout\n");
        printf("============================================\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            choice = -1;
        } else {
            while (getchar() != '\n');
        }

        switch (choice) {
            case 1:
                // 调用你原本的设施与器材子菜单
                facility_Equipment_Menu(currentId);
                break;
            case 2:
                // 调用你刚写好的全馆总览与停用人员审查子菜单
                manager_Admin_Menu(currentId);
                break;
            case 0:
                printf("\nLogging out Manager [%s]...\n", currentId);
                break;
            default:
                printf("\nInvalid choice! Please select 1, 2, or 0.\n");
        }
    } while (choice != 0);
}
int main(void) {
    int choice;
    char currentId[50];

    do {
        printf("\n============================================\n");
        printf("     SDAMS - MANAGER MODULE (TEST MODE)\n");
        printf("============================================\n");
        printf("1. Manager Login\n");
        printf("2. Register New Manager\n");
        printf("0. Exit Program\n");
        printf("============================================\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            choice = -1;
        } else {
            while (getchar() != '\n');
        }

        switch (choice) {
            case 1:
                if (manager_log_system(currentId) == 1) {
                    temp_manager_main_menu(currentId);
                }
                break;
            case 2:
                manager_register();
                break;
            case 0:
                printf("\nExiting test program. Goodbye!\n");
                break;
            default:
                printf("\nInvalid choice! Please enter 1, 2, or 0.\n");
        }
    } while (choice != 0);

    return 0;
}