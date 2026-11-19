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
} std;

struct Payment {
    int paymentId;
    int studentId;
    float amount;
    char date[20];
} pay;

// Function Prototypes
int main();
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
    scanf(" %49[^\n]", username);
    printf("Enter Password: ");
    scanf(" %49[^\n]", password);

    FILE *file = fopen("admin.txt", "r");
    if (file == NULL) {
        printf("\nError: Could not open admin.txt!\n");
        return 0;
    }

    while (fscanf(file, "%49[^|]|%49[^|]|%49[^|]|%49[^|]|%19[^\n]\n",
                  id, name, role, pass, status) == 5) {
        if (strcmp(username, name) == 0 && strcmp(password, pass) == 0) {
            if (strcmp(status, "Active") == 0) {
                found = 1;
                break;
            } else {
                fclose(file);
                printf("\nAccount is %s! Access Denied.\n", status);
                return 0;
            }
        }
    }

    fclose(file);

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
    printf("Enter Instructor ID: ");
    scanf("%d", &inst.id);
    printf("Enter Name: ");
    scanf("%s", inst.name);
    printf("Enter Martial Arts Role (e.g. Karate): ");
    scanf("%s", inst.role);
    printf("Enter Contact Number: ");
    scanf("%s", inst.contact);
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
    int searchId, found = 0;
    
    fp = fopen("instructors.txt", "r");
    if (fp == NULL) {
        printf("\nNo instructor records found.\n");
        return 0;
    }
    
    printf("\nEnter Instructor ID to search: ");
    scanf("%d", &searchId);
    
    while (fscanf(fp, "%d %s %s %s %s", &inst.id, inst.name, inst.role, inst.contact, inst.status) != EOF) {
        if (inst.id == searchId) {
            printf("\n--- Instructor Record Found ---\n");
            printf("ID      : %d\nName    : %s\nRole    : %s\nContact : %s\nStatus  : %s\n", 
                   inst.id, inst.name, inst.role, inst.contact, inst.status);
            found = 1;
            break;
        }
    }
    
    if (found == 0) {
        printf("\nInstructor with ID %d not found.\n", searchId);
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
    scanf("%d", &searchId);
    
    while (fscanf(fp, "%d %s %s %s %s", &inst.id, inst.name, inst.role, inst.contact, inst.status) != EOF) {
        if (inst.id == searchId) {
            found = 1;
            printf("Current Status for %s: %s\n", inst.name, inst.status);
            printf("Enter new status (Active / Not_Active): ");
            scanf("%s", inst.status);
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
    scanf("%d", &std.id);
    printf("Enter Name: ");
    scanf("%s", std.name);
    printf("Enter Course/Info: ");
    scanf("%s", std.info);
    printf("Enter Contact Number: ");
    scanf("%s", std.contact);
    strcpy(std.status, "Active");
    
    fprintf(fp, "%d %s %s %s %s\n", std.id, std.name, std.info, std.contact, std.status);
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
    
    printf("\n--------------------------------------------------------\n");
    printf("%-5s %-15s %-15s %-10s %-10s\n", "ID", "Name", "Info", "Contact", "Status");
    printf("--------------------------------------------------------\n");
    
    while (fscanf(fp, "%d %s %s %s %s", &std.id, std.name, std.info, std.contact, std.status) != EOF) {
        printf("%-5d %-15s %-15s %-10s %-10s\n", std.id, std.name, std.info, std.contact, std.status);
    }
    
    fclose(fp);
    return 1;
}

int searchStudent() {
    FILE *fp;
    int searchId, found = 0;
    
    fp = fopen("students.txt", "r");
    if (fp == NULL) {
        printf("\nNo student records found.\n");
        return 0;
    }
    
    printf("\nEnter Student ID to search: ");
    scanf("%d", &searchId);
    
    while (fscanf(fp, "%d %s %s %s %s", &std.id, std.name, std.info, std.contact, std.status) != EOF) {
        if (std.id == searchId) {
            printf("\n--- Student Record Found ---\n");
            printf("ID     : %d\nName   : %s\nInfo   : %s\nContact: %s\nStatus : %s\n", 
                   std.id, std.name, std.info, std.contact, std.status);
            found = 1;
            break;
        }
    }
    
    if (found == 0) {
        printf("\nStudent with ID %d not found.\n", searchId);
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
    scanf("%d", &searchId);
    
    while (fscanf(fp, "%d %s %s %s %s", &std.id, std.name, std.info, std.contact, std.status) != EOF) {
        if (std.id == searchId) {
            found = 1;
            printf("Current Status for %s: %s\n", std.name, std.status);
            printf("Enter new status (Active / Not_Active): ");
            scanf("%s", std.status);
        }
        fprintf(tempFp, "%d %s %s %s %s\n", std.id, std.name, std.info, std.contact, std.status);
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
    int searchId, found = 0;
    
    fp = fopen("payments.txt", "r");
    if (fp == NULL) {
        printf("\nNo payment records found.\n");
        return 0;
    }
    
    printf("\nEnter Student ID to filter payment records: ");
    scanf("%d", &searchId);
    
    printf("\n--- Payment Records for Student ID %d ---\n", searchId);
    printf("----------------------------------------\n");
    printf("%-12s %-10s %-15s\n", "Payment ID", "Amount", "Date");
    printf("----------------------------------------\n");
    
    while (fscanf(fp, "%d %d %f %s", &pay.paymentId, &pay.studentId, &pay.amount, pay.date) != EOF) {
        if (pay.studentId == searchId) {
            printf("%-12d $%-9.2f %-15s\n", pay.paymentId, pay.amount, pay.date);
            found = 1;
        }
    }
    
    if (found == 0) {
        printf("\nNo payment records found for Student ID %d.\n", searchId);
    }
    
    fclose(fp);
    return 1;
}