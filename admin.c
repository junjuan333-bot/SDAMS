#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "admin.h"

// Structure Definitions
struct Instructor {
    int id;
    char name[50];
    char role[30];
    char contact[20];
    char status[15]; // "Active" or "Not_Active"
} inst;

struct Student {
    int id;
    char name[50];
    char info[50];
    char contact[20];
    char status[15]; // "Active" or "Not_Active"
    char password[30];
} std;

struct Payment {
    int paymentId;
    int studentId;
    float amount;
    char date[20];
} pay;

// Function Prototypes
int adminMenu();
int adminLogin();

// Manage Instructor Functions
int manageInstructorsMenu();
int addInstructor();
int viewInstructors();
int searchInstructor();
int updateInstructor();

// Manage Student Functions
int manageStudentsMenu();
int addStudent();
int viewStudents();
int searchStudent();
int updateStudent();

// Payment Records Functions
int paymentRecordsMenu();
int viewPayments();
int searchPayment();

// Administrator Login and Logout System
int adminLogin() {
    char username[50];
    char password[50];
    char id[50], name[50], role[50], pass[50], status[20];
    int found = 0;
    
    printf("\n========================================\n");
    printf("|             ADMIN LOGIN              |\n");
    printf("========================================\n");
    printf("Enter Username: ");
    if (scanf(" %49[^\n]", username) != 1) {
        printf("Error reading username.\n");
        return 0;
    }
    printf("Enter Password: ");
    if (scanf(" %49[^\n]", password) != 1) {
        printf("Error reading password.\n");
        return 0;
    }

    FILE *fp;
    fp = fopen("admin.txt", "r");
    if (fp == NULL) {
        printf("\nError: Could not open admin.txt!\n");
        return 0;
    }

    while (fscanf(fp, "%49[^|]|%49[^|]|%49[^|]|%49[^|]|%19[^\n]\n", id, name, role, pass, status) == 5) {
        if (strcmp(username, name) == 0 && strcmp(password, pass) == 0) {
            if (strcmp(status, "Active") == 0) {
                found = 1;
                break;
            } else {
                fclose(fp);
                printf("\nAccount is %s! Access Denied.\n", status);
                return 0;
            }
        }
    }

    fclose(fp);

    if (found) {
        printf("\nLogin Successful! Welcome, %s (%s).\n", name, role);
        return 1;
    } else {
        printf("\nInvalid Credentials! Access Denied.\n");
        return 0;
    }
}

// Administrator Menu
int adminMenu() {
    int choice;
    do {
        printf("\n========================================\n");
        printf("|          ADMINISTRATOR MENU          |\n");
        printf("========================================\n");
        printf("| 1. Manage Instructors                |\n");
        printf("| 2. Manage Students                   |\n");
        printf("| 3. Payment Records                   |\n");
        printf("| 4. Logout                            |\n");
        printf("========================================\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                manageInstructorsMenu();
                break;
            case 2:
                manageStudentsMenu();
                break;
            case 3:
                paymentRecordsMenu();
                break;
            case 4:
                printf("\nLogging out from Administrator session...\n");
                break;
            default:
                printf("\nInvalid choice! Please select between 1 and 4.\n");
        }
    } while (choice != 4);
    return 1;
}

// Manage Instructor Functions
int manageInstructorsMenu() {
    int choice;
    do {
        printf("\n========================================\n");
        printf("|        MANAGE INSTRUCTOR MENU        |\n");
        printf("========================================\n");
        printf("| 1. Add Instructor                    |\n");
        printf("| 2. View Instructors                  |\n");
        printf("| 3. Search Instructor                 |\n");
        printf("| 4. Update Instructor Status          |\n");
        printf("| 5. Back to Admin Menu                |\n");
        printf("========================================\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                addInstructor();
                break;
            case 2:
                viewInstructors();
                break;
            case 3:
                searchInstructor();
                break;
            case 4:
                updateInstructor();
                break;
            case 5:
                printf("\nReturning to Admin Menu...\n");
                break;
            default:
                printf("\nInvalid choice! Please select between 1 and 5.\n");
        }
    } while (choice != 5);
    return 1;
}

int addInstructor() {
    FILE *fp;
    
    fp = fopen("instructors.txt", "a");
    if (fp == NULL) {
        printf("\nError opening file!\n");
        return 0;
    }
    
    printf("\n--- Add New Instructor ---\n");
    printf("Enter Instructor ID (e.g., 101): ");
    if (scanf("%d", &inst.id) != 1) {
        printf("\nInvalid ID format.\n");
        fclose(fp);
        return 0;
    }
    printf("Enter Name: ");
    if (scanf("%s", inst.name) != 1) {
        printf("\nInvalid name format.\n");
        fclose(fp);
        return 0;
    }
    printf("Enter Martial Arts Role (e.g. Karate): ");
    if (scanf("%s", inst.role) != 1) {
        printf("\nInvalid role format.\n");
        fclose(fp);
        return 0;
    }
    printf("Enter Contact Number (e.g., 0123456789): ");
    if (scanf("%s", inst.contact) != 1) {
        printf("\nInvalid contact format.\n");
        fclose(fp);
        return 0;
    }
    strcpy(inst.status, "Active");
    
    fprintf(fp, "%d %s %s %s %s\n", inst.id, inst.name, inst.role, inst.contact, inst.status);
    fclose(fp);
    printf("\nInstructor added successfully!\n");
    return 1;
}

int viewInstructors() {
    FILE *fp;
    
    fp = fopen("instructors.txt", "r");
    if (fp == NULL) {
        printf("\nNo instructor records found. Please add records first.\n");
        return 0;
    }
    
    printf("\n-------------------------------------------------------------\n");
    printf("%-5s %-15s %-15s %-15s %-10s\n", "ID", "Name", "Role", "Contact", "Status");
    printf("-------------------------------------------------------------\n");
    
    while (fscanf(fp, "%d %s %s %s %s", &inst.id, inst.name, inst.role, inst.contact, inst.status) != EOF) {
        printf("%-5d %-15s %-15s %-15s %-10s\n", inst.id, inst.name, inst.role, inst.contact, inst.status);
    }
    
    fclose(fp);
    return 1;
}

int searchInstructor() {
    FILE *fp;
    int choice, searchId, found = 0;
    char searchStr[100];

    fp = fopen("instructors.txt", "r");
    if (fp == NULL) {
        printf("\nNo instructor records found.\n");
        return 0;
    }

    printf("\nSearch Instructor by:\n");
    printf("1. ID\n");
    printf("2. Name\n");
    printf("3. Role\n");
    printf("4. Contact\n");
    printf("Enter choice: ");
    if (scanf("%d", &choice) != 1) {
        printf("\nInvalid input.\n");
        fclose(fp);
        return 0;
    }

    if (choice == 1) {
        printf("Enter Instructor ID: ");
        if (scanf("%d", &searchId) != 1) {
            printf("\nInvalid ID format.\n");
            fclose(fp);
            return 0;
        }
    } else if (choice >= 2 && choice <= 4) {
        printf("Enter search term: ");
        if (scanf(" %99s", searchStr) != 1) {
            printf("\nInvalid search term format.\n");
            fclose(fp);
            return 0;
        }
    } else {
        printf("\nInvalid search option.\n");
        fclose(fp);
        return 0;
    }

    while (fscanf(fp, "%d %s %s %s %s", &inst.id, inst.name, inst.role, inst.contact, inst.status) == 5) {
        int match = 0;

        if (choice == 1 && inst.id == searchId) {
            match = 1;
        } else if (choice == 2 && strcmp(inst.name, searchStr) == 0) {
            match = 1;
        } else if (choice == 3 && strcmp(inst.role, searchStr) == 0) {
            match = 1;
        } else if (choice == 4 && strcmp(inst.contact, searchStr) == 0) {
            match = 1;
        }

        if (match) {
            printf("\n--- Instructor Record Found ---\n");
            printf("ID      : %d\nName    : %s\nRole    : %s\nContact : %s\nStatus  : %s\n",
                   inst.id, inst.name, inst.role, inst.contact, inst.status);
            found = 1;
            
            if (choice == 1) {
                break;
            }
        }
    }

    if (!found) {
        printf("\nNo matching instructor record found.\n");
    }

    fclose(fp);
    return 1;
}

int updateInstructor() {
    FILE *fp, *tempFp;
    int searchId, found = 0;
    
    fp = fopen("instructors.txt", "r");
    if (fp == NULL) {
        printf("\nNo instructor records found.\n");
        return 0;
    }
    
    tempFp = fopen("temp.txt", "w");
    if (tempFp == NULL) {
        printf("\nError opening temporary file.\n");
        fclose(fp);
        return 0;
    }
    
    printf("\nEnter Instructor ID to update status: ");
    if (scanf("%d", &searchId) != 1) {
        printf("\nInvalid ID format.\n");
        fclose(fp);
        fclose(tempFp);
        return 0;
    }

    while (fscanf(fp, "%d %s %s %s %s", &inst.id, inst.name, inst.role, inst.contact, inst.status) != EOF) {
        if (inst.id == searchId) {
            found = 1;
            printf("Current Status for %s: %s\n", inst.name, inst.status);
            printf("Enter new status (Active / Not_Active): ");
            if (scanf("%s", inst.status) != 1) {
                printf("\nInvalid status format.\n");
                fclose(fp);
                fclose(tempFp);
                return 0;
            }
        }
        fprintf(tempFp, "%d %s %s %s %s\n", inst.id, inst.name, inst.role, inst.contact, inst.status);
    }
    
    fclose(fp);
    fclose(tempFp);
    
    remove("instructors.txt");
    rename("temp.txt", "instructors.txt");
    
    if (found == 1) {
        printf("\nInstructor status updated successfully!\n");
    } else {
        printf("\nInstructor ID not found.\n");
    }
    return 1;
}

// Manage Student Functions
int manageStudentsMenu() {
    int choice;
    do {
        printf("\n========================================\n");
        printf("|         MANAGE STUDENT MENU          |\n");
        printf("========================================\n");
        printf("| 1. Add Student                       |\n");
        printf("| 2. View Students                     |\n");
        printf("| 3. Search Student                    |\n");
        printf("| 4. Update Student Status             |\n");
        printf("| 5. Back to Admin Menu                |\n");
        printf("========================================\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                addStudent();
                break;
            case 2:
                viewStudents();
                break;
            case 3:
                searchStudent();
                break;
            case 4:
                updateStudent();
                break;
            case 5:
                printf("\nReturning to Admin Menu...\n");
                break;
            default:
                printf("\nInvalid choice! Please select between 1 and 5.\n");
        }
    } while (choice != 5);
    return 1;
}

int addStudent() {
    FILE *fp;
    
    fp = fopen("students.txt", "a");
    if (fp == NULL) {
        printf("\nError opening file!\n");
        return 0;
    }
    
    printf("\n--- Add New Student ---\n");
    printf("Enter Student ID: ");
    if (scanf("%d", &std.id) != 1) {
        printf("\nInvalid ID format.\n");
        fclose(fp);
        return 0;
    }
    printf("Enter Name: ");
    if (scanf("%s", std.name) != 1) {
        printf("\nInvalid name format.\n");
        fclose(fp);
        return 0;
    }
    printf("Enter Course/Info: ");
    if (scanf("%s", std.info) != 1) {
        printf("\nInvalid info format.\n");
        fclose(fp);
        return 0;
    }
    printf("Enter Contact Number: ");
    if (scanf("%s", std.contact) != 1) {
        printf("\nInvalid contact format.\n");
        fclose(fp);
        return 0;
    }
    printf("Enter Password: ");
    if (scanf("%s", std.password) != 1) {
        printf("\nInvalid password format.\n");
        fclose(fp);
        return 0;
    }
    strcpy(std.status, "Active");

    fprintf(fp, "%d %s %s %s %s %s\n", std.id, std.name, std.info, std.contact, std.status, std.password);
    fclose(fp);
    printf("\nStudent added successfully!\n");
    return 1;
}

int viewStudents() {
    FILE *fp;
    
    fp = fopen("students.txt", "r");
    if (fp == NULL) {
        printf("\nNo student records found.\n");
        return 0;
    }
    
    printf("\n---------------------------------------------------------------------\n");
    printf("%-5s %-15s %-15s %-10s %-10s %-15s\n", "ID", "Name", "Info", "Contact", "Status", "Password");
    printf("---------------------------------------------------------------------\n");
    
    while (fscanf(fp, "%d %s %s %s %s %s", &std.id, std.name, std.info, std.contact, std.status, std.password) != EOF) {
        printf("%-5d %-15s %-15s %-10s %-10s %-15s\n", std.id, std.name, std.info, std.contact, std.status, std.password);
    }
    
    fclose(fp);
    return 1;
}

int searchStudent() {
    FILE *fp;
    int choice, searchId, found = 0;
    char searchStr[100];

    fp = fopen("students.txt", "r");
    if (fp == NULL) {
        printf("\nNo student records found.\n");
        return 0;
    }

    printf("\nSearch Student by:\n");
    printf("1. ID\n");
    printf("2. Name\n");
    printf("3. Info\n");
    printf("4. Contact\n");
    printf("Enter choice: ");
    if (scanf("%d", &choice) != 1) {
        printf("\nInvalid input.\n");
        fclose(fp);
        return 0;
    }

    if (choice == 1) {
        printf("Enter Student ID: ");
        if (scanf("%d", &searchId) != 1) {
            printf("\nInvalid ID format.\n");
            fclose(fp);
            return 0;
        }
    } else if (choice >= 2 && choice <= 4) {
        printf("Enter search term: ");
        if (scanf(" %99s", searchStr) != 1) {
            printf("\nInvalid search term format.\n");
            fclose(fp);
            return 0;
        }
    } else {
        printf("\nInvalid search option.\n");
        fclose(fp);
        return 0;
    }

    while (fscanf(fp, "%d %s %s %s %s %s", &std.id, std.name, std.info, std.contact, std.status, std.password) == 6) {
        int match = 0;

        if (choice == 1 && std.id == searchId) {
            match = 1;
        } else if (choice == 2 && strcmp(std.name, searchStr) == 0) {
            match = 1;
        } else if (choice == 3 && strcmp(std.info, searchStr) == 0) {
            match = 1;
        } else if (choice == 4 && strcmp(std.contact, searchStr) == 0) {
            match = 1;
        }

        if (match) {
            printf("\n--- Student Record Found ---\n");
            printf("ID     : %d\nName   : %s\nInfo   : %s\nContact: %s\nStatus : %s\nPassword : %s\n",
                   std.id, std.name, std.info, std.contact, std.status, std.password);
            found = 1;

            if (choice == 1) {
                break;
            }
        }
    }

    if (!found) {
        printf("\nNo matching student record found.\n");
    }

    fclose(fp);
    return 1;
}

int updateStudent() {
    FILE *fp, *tempFp;
    int searchId, found = 0;
    
    fp = fopen("students.txt", "r");
    if (fp == NULL) {
        printf("\nNo student records found.\n");
        return 0;
    }
    
    tempFp = fopen("temp.txt", "w");
    if (tempFp == NULL) {
        printf("\nError opening temporary file.\n");
        fclose(fp);
        return 0;
    }
    
    printf("\nEnter Student ID to update status: ");
    if (scanf("%d", &searchId) != 1) {
        printf("\nInvalid ID format.\n");
        fclose(fp);
        fclose(tempFp);
        return 0;
    }

    while (fscanf(fp, "%d %49s %49s %19s %14s %29s", &std.id, std.name, std.info, std.contact, std.status, std.password) == 6) {
        if (std.id == searchId) {
            found = 1;
            printf("Current Status for %s: %s\n", std.name, std.status);
            printf("Enter new status (Active / Not_Active): ");
            if (scanf("%14s", std.status) != 1) {
                printf("\nInvalid status format.\n");
                fclose(fp);
                fclose(tempFp);
                return 0;
            }
        }
        fprintf(tempFp, "%d %s %s %s %s %s\n", std.id, std.name, std.info, std.contact, std.status, std.password);
    }
    
    fclose(fp);
    fclose(tempFp);
    
    remove("students.txt");
    rename("temp.txt", "students.txt");
    
    if (found == 1) {
        printf("\nStudent status updated successfully!\n");
    } else {
        printf("\nStudent ID not found.\n");
    }
    return 1;
}

// Payment Records Functions
int paymentRecordsMenu() {
    int choice;
    do {
        printf("\n==========================================\n");
        printf("|         PAYMENT RECORDS MENU           |\n");
        printf("==========================================\n");
        printf("| 1. View All Payment Records            |\n");
        printf("| 2. Search Payment Record by Student ID |\n");
        printf("| 3. Back to Admin Menu                  |\n");
        printf("==========================================\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                viewPayments();
                break;
            case 2:
                searchPayment();
                break;
            case 3:
                printf("\nReturning to Admin Menu...\n");
                break;
            default:
                printf("\nInvalid choice! Please select between 1 and 3.\n");
        }
    } while (choice != 3);
    return 1;
}

int viewPayments() {
    FILE *fp;
    
    fp = fopen("payments.txt", "r");
    if (fp == NULL) {
        printf("\nNo payment records found.\n");
        return 0;
    }
    
    printf("\n-------------------------------------------------\n");
    printf("%-12s %-12s %-10s %-15s\n", "Payment ID", "Student ID", "Amount", "Date");
    printf("-------------------------------------------------\n");
    
    while (fscanf(fp, "%d %d %f %s", &pay.paymentId, &pay.studentId, &pay.amount, pay.date) != EOF) {
        printf("%-12d %-12d $%-9.2f %-15s\n", pay.paymentId, pay.studentId, pay.amount, pay.date);
    }
    
    fclose(fp);
    return 1;
}

int searchPayment() {
    FILE *fp;
    int choice, searchId, found = 0;
    float searchAmount;
    char searchDate[50];

    fp = fopen("payments.txt", "r");
    if (fp == NULL) {
        printf("\nNo payment records found.\n");
        return 0;
    }

    printf("\nSearch Payment by:\n");
    printf("1. Payment ID\n");
    printf("2. Student ID\n");
    printf("3. Amount\n");
    printf("4. Date\n");
    printf("Enter choice: ");
    if (scanf("%d", &choice) != 1) {
        printf("\nInvalid input.\n");
        fclose(fp);
        return 0;
    }

    if (choice == 1) {
        printf("Enter Payment ID: ");
        if (scanf("%d", &searchId) != 1) {
            printf("\nInvalid ID format.\n");
            fclose(fp);
            return 0;
        }
    } else if (choice == 2) {
        printf("Enter Student ID: ");
        if (scanf("%d", &searchId) != 1) {
            printf("\nInvalid ID format.\n");
            fclose(fp);
            return 0;
        }
    } else if (choice == 3) {
        printf("Enter Payment Amount: ");
        if (scanf("%f", &searchAmount) != 1) {
            printf("\nInvalid amount format.\n");
            fclose(fp);
            return 0;
        }
    } else if (choice == 4) {
        printf("Enter Payment Date: ");
        if (scanf(" %49s", searchDate) != 1) {
            printf("\nInvalid date format.\n");
            fclose(fp);
            return 0;
        }
    } else {
        printf("\nInvalid search option.\n");
        fclose(fp);
        return 0;
    }

    printf("\n--- Matching Payment Records ---\n");
    printf("-----------------------------------------------------\n");
    printf("%-12s %-12s %-10s %-15s\n", "Payment ID", "Student ID", "Amount", "Date");
    printf("-----------------------------------------------------\n");

    while (fscanf(fp, "%d %d %f %s", &pay.paymentId, &pay.studentId, &pay.amount, pay.date) == 4) {
        int match = 0;

        if (choice == 1 && pay.paymentId == searchId) {
            match = 1;
        } else if (choice == 2 && pay.studentId == searchId) {
            match = 1;
        } else if (choice == 3 && (pay.amount - searchAmount < 0.01f && searchAmount - pay.amount < 0.01f)) {
            match = 1;
        } else if (choice == 4 && strcmp(pay.date, searchDate) == 0) {
            match = 1;
        }

        if (match) {
            printf("%-12d %-12d $%-9.2f %-15s\n", pay.paymentId, pay.studentId, pay.amount, pay.date);
            found = 1;

            if (choice == 1) {
                break;
            }
        }
    }

    if (!found) {
        printf("No matching payment records found.\n");
    }
    printf("-----------------------------------------------------\n");

    fclose(fp);
    return 1;
}