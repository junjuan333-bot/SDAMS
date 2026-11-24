#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "admin.h"
#include "facilities_officer.h"
#include "manager.h"
#include "student.h"
#include "instructor.h"

// Main Menu
int main() {
    
    int choice;
    char currentId[50];
    do {
        printf("\n=========================================\n");
        printf("|  SELF DEFENCE ACADEMY SYSTEM (SDAMS)  |\n");
        printf("=========================================\n");
        printf("| 1. Manager Menu                       |\n");
        printf("| 2. Administrator Menu                 |\n");
        printf("| 3. Student Menu                       |\n");
        printf("| 4. Instructor Menu                    |\n");
        printf("| 5. Facilities Officer Menu            |\n");
        printf("| 6. Exit Application                   |\n");
        printf("=========================================\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                if (manager_log_system(currentId) == 1) { 
                    manager_main_menu(currentId);
                }
                break;
            case 2:
                if (adminLogin()) {
                    adminMenu();
                }
                break;
            case 3:
                studentMenu();
                break;
            case 4:
                instructor_login_portal();

                break;
            case 5:
                if (facilitiesOfficerLogin()) {
                    facilitiesOfficerMenu();
                }
                break;
            case 6:
                printf("\nExiting SDAMS Application. Goodbye!\n");
                break;
            default:
                printf("\nInvalid choice! Please select between 1 and 6.\n");
        }
    } while (choice != 6);
    return 0;
}
