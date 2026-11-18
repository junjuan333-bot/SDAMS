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

int manager_register(void) {
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
int approve_Restock(void){
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
    return 1;
}
void manager_log_menu(char currentId[]) {
    int choice;
    do {
        printf("\n====================================\n");
        printf("   SDAMS - Manager Menu (%s)\n", currentId);
        printf("====================================\n");
        printf("1. Approve Equipment Restock\n");
        printf("2. Change Password\n");
        printf("0. Logout\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            choice = -1;
        }
        switch (choice) {
            case 1:
                approve_Restock();
                break;
            case 2:
                change_password(currentId);
                break;
            case 0:
                printf("Logging out...\n");
                break;
            default:
                printf("Invalid choice! Please enter a valid option.\n");
        }
    } while (choice != 0);
}
int main(void) {
    int choice;
    int loginStatus;

    do {
        printf("\n====================================\n");
        printf("   SDAMS - Manager Portal (Test)    \n");
        printf("====================================\n");
        printf("1. Register Manager\n");
        printf("2. Login as Manager\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");

        // 【优化 2：防止输入字母导致死循环】
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n'); // 把卡在缓冲区里的错误字母吃掉清空
            choice = 0;                // 设成 0，让下面的 switch 走 default 提示
        }

        switch (choice) {
            case 1:
                manager_register();
                break;
            case 2:
                char loggedInId[50];
                loginStatus = manager_log_system(loggedInId);
                if (loginStatus == 1) {
                    printf(">> [Success] Entering Manager Main Menu...\n");
                }
                break;
            case 3:
                printf("Exiting system. Goodbye!\n");
                break;
            default:
                printf("Invalid choice! Please enter 1, 2, or 3.\n");
        }
    } while (choice != 3);

    return 0;
}
