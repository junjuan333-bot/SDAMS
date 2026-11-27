#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "student.h"

#define MAX_ID 20
#define MAX_NAME 50
#define MAX_STYLE 30
#define MAX_PHONE 30
#define MAX_STATUS 20
#define MAX_DATE 20
#define MAX_REASON 100
#define MAX_LINE 256
#define MAX_PASS 30

#define STUDENT_FILE "students.txt"
#define INSTRUCTOR_FILE "instructors.txt"
#define CLASS_FILE "classes.txt"
#define BOOKING_FILE "bookings.txt"
#define PAYMENT_FILE "payments.txt"
#define REFUND_FILE "refund_requests.txt"
#define RATING_FILE "ratings.txt"

struct checkbuff {
    int charactrer;
};

union LoginInput {
    char inputId[MAX_ID];
    int tempCode; 
};

static void clearInputBuffer(void) {
    struct checkbuff buffercase;
    while ((buffercase.charactrer = getchar()) != '\n' && buffercase.charactrer != EOF) {
    }
}

static int readIntInRange(int minVal, int maxVal, int *outVal) {
    if (scanf("%d", outVal) != 1) {
        clearInputBuffer();
        printf("Invalid input! Please enter a valid number.\n");
        return 0;
    }
    clearInputBuffer();
    if (*outVal < minVal || *outVal > maxVal) {
        printf("Choice out of range (%d - %d). Please try again.\n", minVal, maxVal);
        return 0;
    }
    return 1;
}

static int equalsIgnoreCase(const char *a, const char *b) {

    while (*a != '\0' && *b != '\0') {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) {
            return 0;
        }
        a++;
        b++;
    }
    return (*a == '\0' && *b == '\0');
}

static int isActiveBookingStatus(int status) {
    return (status == 1 || status == 2);
}

static const char *bookingStatusLabel(int status) {
    if (status == 1) {
        return "Pending";
    }
    if (status == 2) {
        return "Booked";
    }
    return "Cancelled";
}

static int hasActiveBookings(const char *studentId) {
    FILE *fp = fopen(BOOKING_FILE, "r");
    if (fp == NULL) {
        return 0;
    }

    char line[MAX_LINE];
    int activeCount = 0;
    while (fgets(line, sizeof(line), fp) != NULL) {
        char sId[MAX_ID], cId[MAX_ID];
        int status;
        if (sscanf(line, "%19[^,],%19[^,],%d", sId, cId, &status) == 3) {
            if (strcmp(sId, studentId) == 0 && isActiveBookingStatus(status)) {
                activeCount++;
            }
        }
    }
    fclose(fp);
    return activeCount > 0;
}

static void displayActiveBookings(const char *studentId) {
    FILE *fp = fopen(BOOKING_FILE, "r");
    if (fp == NULL) {
        printf("No booking records found in '%s' yet.\n", BOOKING_FILE);
        return;
    }

    printf("\n--- Active Bookings (%s) ---\n", studentId);
    char line[MAX_LINE];
    int found = 0;
    while (fgets(line, sizeof(line), fp) != NULL) {
        char sId[MAX_ID], cId[MAX_ID];
        int status;
        if (sscanf(line, "%19[^,],%19[^,],%d", sId, cId, &status) == 3) {
            if (strcmp(sId, studentId) == 0 && isActiveBookingStatus(status)) {
                printf("Class ID: %-10s | Status: %s\n", cId, bookingStatusLabel(status));
                found = 1;
            }
        }
    }
    fclose(fp);

    if (!found) {
        printf("You have no active bookings.\n");
        return;
    }
}

static int displayAvailableClasses(void) {
    FILE *classFp = fopen(CLASS_FILE, "r");
    if (classFp == NULL) {
        printf("Notice: '%s' not found yet. Creating an empty file for setup.\n", CLASS_FILE);
        return 0;
    }

    printf("\n--- Available Classes ---\n");
    char line[MAX_LINE];
    int hasClass = 0;
    while (fgets(line, sizeof(line), classFp) != NULL) {
        printf("%s", line);
        hasClass = 1;
    }
    fclose(classFp);

    if (!hasClass) {
        printf("No classes available currently.\n");
        return 0;
    }
    return 1;
}

static int hasAnyInstructors(void) {
    FILE *fp = fopen(INSTRUCTOR_FILE, "r");
    if (fp == NULL) {
        printf("No instructors available currently.\n");
        return 0;
    }

    char line[MAX_LINE];
    int count = 0;
    while (fgets(line, sizeof(line), fp) != NULL) {
        char id[MAX_ID], name[MAX_NAME], style[MAX_STYLE], contact[MAX_PHONE], status[MAX_STATUS];
        if (sscanf(line, "%19s %49s %29s %29s %19s", id, name, style, contact, status) == 5) {
            count++;
        }
    }
    fclose(fp);

    if (count == 0) {
        printf("No instructors available currently.\n");
    }

    return count > 0;
}
static void displayAllInstructors(void) {
    FILE *fp = fopen(INSTRUCTOR_FILE, "r");
    if (fp == NULL) {
        printf("Notice: '%s' not found.\n", INSTRUCTOR_FILE);
        return;
    }

    printf("\n--- Available Instructors ---\n");
    char line[MAX_LINE];
    int count = 0;
    while (fgets(line, sizeof(line), fp) != NULL) {
        char id[MAX_ID], name[MAX_NAME], style[MAX_STYLE], contact[MAX_PHONE], status[MAX_STATUS];
        if (sscanf(line, "%19s %49s %29s %29s %19s", id, name, style, contact, status) == 5) {
            printf("ID: %-6s | Name: %-15s | Style: %-12s | Status: %s\n",
                   id, name, style, status);
            count++;
        }
    }
    fclose(fp);

}

static int lookupInstructorInfo(const char *targetInstructorId,
                                char *outName,
                                char *outStyle,
                                char *outStatus) {
    FILE *fp = fopen(INSTRUCTOR_FILE, "r");
    if (fp == NULL) {
        if (outName != NULL) strcpy(outName, "Unknown");
        if (outStyle != NULL) strcpy(outStyle, "Unknown");
        if (outStatus != NULL) strcpy(outStatus, "Unknown");
        return 0;
    }

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), fp) != NULL) {
        char id[MAX_ID], name[MAX_NAME], style[MAX_STYLE], contact[MAX_PHONE], status[MAX_STATUS];
        if (sscanf(line, "%19s %49s %29s %29s %19s", id, name, style, contact, status) == 5) {
            if (strcmp(id, targetInstructorId) == 0) {
                fclose(fp);
                if (outName != NULL) strcpy(outName, name);
                if (outStyle != NULL) strcpy(outStyle, style);
                if (outStatus != NULL) strcpy(outStatus, status);
                return 1;
            }
        }
    }

    fclose(fp);
    if (outName != NULL) strcpy(outName, "Unknown");
    if (outStyle != NULL) strcpy(outStyle, "Unknown");
    if (outStatus != NULL) strcpy(outStatus, "Unknown");
    return 0;
}

static float calcInstructorAvg(const char *targetInstructorId, int *outCount) {
    FILE *fp = fopen(RATING_FILE, "r");
    *outCount = 0;
    if (fp == NULL) {
        return -1.0f;
    }

    char line[MAX_LINE];
    double sum = 0.0;
    int count = 0;

    while (fgets(line, sizeof(line), fp) != NULL) {
        char sId[MAX_ID], instId[MAX_ID];
        float score;
        if (sscanf(line, "%19[^,],%19[^,],%f", sId, instId, &score) == 3) {
            if (strcmp(instId, targetInstructorId) == 0) {
                sum += score;
                count++;
            }
        }
    }
    fclose(fp);

    *outCount = count;
    if (count == 0) {
        return -1.0f;
    }
    return (float)(sum / count);
}

static int generateNextPaymentId(char *outId, size_t outSize) {
    int maxNum = 5000;
    FILE *fp = fopen(PAYMENT_FILE, "r");
    if (fp == NULL) {
        printf("Error: '%s' not found. Cannot generate Payment ID.\n", PAYMENT_FILE);
        return 0; 
    }

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), fp) != NULL) {
        char pId[MAX_ID], sId[MAX_ID], date[MAX_DATE];
        double amount;
        if (sscanf(line, "%19s %19s %lf %19s", pId, sId, &amount, date) == 4) {
            int allDigits = 1;
            size_t len = strlen(pId);
            if (len == 0) {
                allDigits = 0;
            }
            for (size_t i = 0; i < len; i++) {
                if (pId[i] < '0' || pId[i] > '9') {
                    allDigits = 0;
                    break;
                }
            }
            if (allDigits) {
                int num = atoi(pId);
                if (num > maxNum) {
                    maxNum = num;
                }
            }
        }
    }
    fclose(fp);

    snprintf(outId, outSize, "%d", maxNum + 1);
    return 1;
}

static void generateNextRefundId(char *outId, size_t outSize) {
    int maxNum = 0;
    FILE *fp = fopen(REFUND_FILE, "r");
    if (fp != NULL) {
        char line[MAX_LINE];
        while (fgets(line, sizeof(line), fp) != NULL) {
            char rId[MAX_ID], pId[MAX_ID], name[MAX_NAME], reason[MAX_REASON], status[MAX_STATUS];
            double amount;
            sscanf(line, "%19[^|]|%19[^|]|%49[^|]|%lf|%99[^|]|%19[^\r\n]",
                       rId, pId, name, &amount, reason, status); 
                    
            int num = atoi(rId + 1);
            if (num > maxNum) {
                maxNum = num;}
                    
        }
        fclose(fp);
    }
    snprintf(outId, outSize, "R%03d", maxNum + 1);
}


int studentLogin(const char *studentIdInput, const char *passwordInput,
                 char *outStudentId, char *outName,
                 char *outStyle, char *outContact, char *outStatus) {
    FILE *fp = fopen(STUDENT_FILE, "r");
    if (fp == NULL) {
        printf("Error: '%s' not found.\n", STUDENT_FILE);
        return 0;
    }

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), fp) != NULL) {
        char id[MAX_ID], name[MAX_NAME], style[MAX_STYLE],
             contact[MAX_PHONE], status[MAX_STATUS], password[MAX_PASS];

        if (sscanf(line, "%19s %49s %29s %29s %19s %29s",
                   id, name, style, contact, status, password) == 6) {
            if (strcmp(id, studentIdInput) == 0) {
                fclose(fp);

                if (strcmp(password, passwordInput) != 0) {
                    printf("Invalid password for Student ID '%s'.\n", id);
                    return 0;
                }

                if (strcmp(status, "Active") != 0) {
                    printf("Account for Student ID '%s' is not active (Status: %s).\n", id, status);
                    return 0;
                }

                if (outStudentId != NULL) strcpy(outStudentId, id);
                if (outName != NULL) strcpy(outName, name);
                if (outStyle != NULL) strcpy(outStyle, style);
                if (outContact != NULL) strcpy(outContact, contact);
                if (outStatus != NULL) strcpy(outStatus, status);

                printf("\nLogin successful! Welcome, %s.\n", name);
                return 1;
            }
        }
    }


    fclose(fp);
    printf("Student ID '%s' not found in '%s'.\n", studentIdInput, STUDENT_FILE);
    return 0;
}

void studentMenu(void) {
    union LoginInput user;
    char passwordInput[MAX_PASS];
    char studentId[MAX_ID], studentName[MAX_NAME], studentStyle[MAX_STYLE],
         studentContact[MAX_PHONE], studentStatus[MAX_STATUS];

    while (1) {
        printf("\nEnter Student ID (or 'q' to quit): ");
        if (scanf("%19s", user.inputId) != 1) {
            clearInputBuffer();
            return;
        }
        clearInputBuffer();

        if (strcmp(user.inputId, "q") == 0 || strcmp(user.inputId, "Q") == 0) {
            return;
        }

        printf("Enter Password: ");
        if (scanf("%29s", passwordInput) != 1) {
            clearInputBuffer();
            return;
        }
        clearInputBuffer();

        if (studentLogin(user.inputId, passwordInput,
                         studentId, studentName, studentStyle,
                         studentContact, studentStatus) == 1) {
            studentPortal(studentId, studentName);
            return;
        }

        printf("Please try again.\n");
    }
}

void studentPortal(char *studentId, char *studentName) {
    int choice;
    do {
        printf("\n========================================\n");
        printf("       STUDENT PORTAL (%s)\n", studentName);
        printf("========================================\n");
        printf("1. Book Class Management (Book/View/Reschedule/Cancel)\n");
        printf("2. Payment & Refund Management\n");
        printf("3. Instructor Rating (Rate/View)\n");
        printf("4. Logout\n");
        printf("Enter your choice (1-4): ");

        if (!readIntInRange(1, 4, &choice)) {
            continue;
        }

        switch (choice) {
            case 1:
                manageBookings(studentId);
                break;
            case 2:
                paymentManagement(studentId, studentName);
                break;
            case 3:
                instructorRatingMenu(studentId);
                break;
            case 4:
                printf("Logging out from student portal...\n");
                break;
        }
    } while (choice != 4);
}

void manageBookings(char *studentId) {
    int choice;
    do {
        printf("\n--- Book Class Management ---\n");
        printf("1. Book a Class\n");
        printf("2. View My Bookings\n");
        printf("3. Reschedule a Booking\n");
        printf("4. Cancel a Booking\n");
        printf("5. Back to Student Menu\n");
        printf("Enter your choice (1-5): ");

        if (!readIntInRange(1, 5, &choice)) {
            continue;
        }

        switch (choice) {
            case 1:
                bookClass(studentId);
                break;
            case 2:
                viewMyBookings(studentId);
                break;
            case 3:
                rescheduleBooking(studentId);
                break;
            case 4:
                cancelBooking(studentId);
                break;
            case 5:
                break;
        }
    } while (choice != 5);
}

void bookClass(char *studentId) {
    

    if (displayAvailableClasses() == 0) {
    return; 
}

    char targetClass[MAX_ID];
    printf("Enter Class ID to book : ");
    scanf("%19s", targetClass);
    clearInputBuffer();
    for (size_t i = 0; targetClass[i] != '\0'; i++) {
        targetClass[i] = (char)toupper((unsigned char)targetClass[i]);
    }

    FILE *bookFp = fopen(BOOKING_FILE, "a");
    if (bookFp == NULL) {
        printf("Error: Unable to write to '%s'.\n", BOOKING_FILE);
        return;
    }

    fprintf(bookFp, "%s,%s,1\n", studentId, targetClass);
    fclose(bookFp);

    printf("Booking request submitted for class '%s' (Status: Pending)!\n", targetClass);
}

void viewMyBookings(char *studentId) {
    FILE *fp = fopen(BOOKING_FILE, "r");
    if (fp == NULL) {
        printf("No booking records found in '%s' yet.\n", BOOKING_FILE);
        return;
    }

    printf("\n--- My Bookings (%s) ---\n", studentId);
    char line[MAX_LINE];
    int found = 0;
    while (fgets(line, sizeof(line), fp) != NULL) {
        char sId[MAX_ID], cId[MAX_ID];
        int status;
        if (sscanf(line, "%19[^,],%19[^,],%d", sId, cId, &status) == 3) {
            if (strcmp(sId, studentId) == 0) {
                printf("Class ID: %-10s | Status: %s\n", cId, bookingStatusLabel(status));
                found = 1;
            }
        }
    }
    fclose(fp);

    if (!found) {
        printf("You have no bookings yet.\n");
    }
}

void rescheduleBooking(char *studentId) {
    char oldClassId[MAX_ID], newClassId[MAX_ID];
    printf("\n--- Reschedule Booking ---\n");
    displayActiveBookings(studentId);

    if (!hasActiveBookings(studentId)) {
        printf("No class to reschedule.\n");
        return;
    }

    printf("\nEnter current booked Class ID to reschedule: ");
    scanf("%19s", oldClassId);
    clearInputBuffer();

    displayAvailableClasses();
    printf("\nEnter new Class ID: ");
    scanf("%19s", newClassId);
    clearInputBuffer();

    for (size_t i = 0; oldClassId[i] != '\0'; i++) {
        oldClassId[i] = (char)toupper((unsigned char)oldClassId[i]);
    }
    for (size_t i = 0; newClassId[i] != '\0'; i++) {
        newClassId[i] = (char)toupper((unsigned char)newClassId[i]);
    }

    FILE *fp = fopen(BOOKING_FILE, "r");
    if (fp == NULL) {
        printf("No booking records found in '%s'.\n", BOOKING_FILE);
        return;
    }

    FILE *tempFp = fopen("temp_bookings.txt", "w");
    if (tempFp == NULL) {
        printf("Error creating temporary file.\n");
        fclose(fp);
        return;
    }

    char line[MAX_LINE];
    int updated = 0;
    while (fgets(line, sizeof(line), fp) != NULL) {
        char sId[MAX_ID], cId[MAX_ID];
        int status;
        if (sscanf(line, "%19[^,],%19[^,],%d", sId, cId, &status) == 3) {
            if (!updated && strcmp(sId, studentId) == 0 &&
                equalsIgnoreCase(cId, oldClassId) && isActiveBookingStatus(status)) {
                fprintf(tempFp, "%s,%s,0\n", sId, cId);
                fprintf(tempFp, "%s,%s,1\n", studentId, newClassId);
                updated = 1;
                continue;
            }
        }
        fputs(line, tempFp);
    }

    fclose(fp);
    fclose(tempFp);

    remove(BOOKING_FILE);
    rename("temp_bookings.txt", BOOKING_FILE);

    if (updated) {
        printf("Rescheduled from '%s' to '%s' successfully (Status: Pending)!\n", oldClassId, newClassId);
    } else {
        printf("Active booking for Class ID '%s' not found.\n", oldClassId);
    }
}

void cancelBooking(char *studentId) {
    char targetClassId[MAX_ID];
    printf("\n--- Cancel Booking ---\n");
    displayActiveBookings(studentId);

    if (!hasActiveBookings(studentId)) {
        printf("No class to cancel.\n");
        return;
    }

    printf("\nEnter Class ID to cancel: ");
    scanf("%19s", targetClassId);
    clearInputBuffer();
    
 

    FILE *fp = fopen(BOOKING_FILE, "r");
    if (fp == NULL) {
        printf("No booking records found in '%s'.\n", BOOKING_FILE);
        return;
    }

    FILE *tempFp = fopen("temp_bookings.txt", "w");
    if (tempFp == NULL) {
        printf("Error creating temporary file.\n");
        fclose(fp);
        return;
    }

    char line[MAX_LINE];
    int cancelled = 0;
    while (fgets(line, sizeof(line), fp) != NULL) {
        char sId[MAX_ID], cId[MAX_ID];
        int status;
        if (sscanf(line, "%19[^,],%19[^,],%d", sId, cId, &status) == 3) {
            if (!cancelled && strcmp(sId, studentId) == 0 &&
                equalsIgnoreCase(cId, targetClassId) && isActiveBookingStatus(status)) {
                fprintf(tempFp, "%s,%s,0\n", sId, cId);
                cancelled = 1;
                continue;
            }
        }
        fputs(line, tempFp);
    }

    fclose(fp);
    fclose(tempFp);

    remove(BOOKING_FILE);
    rename("temp_bookings.txt", BOOKING_FILE);

    if (cancelled) {
        printf("Booking for Class ID '%s' has been cancelled.\n", targetClassId);
    } else {
        printf("Active booking for Class ID '%s' not found.\n", targetClassId);
    }
}

void paymentManagement(char *studentId, char *studentName) {
    int choice;
    do {
        printf("\n--- Payment & Refund Management ---\n");
        printf("1. Make a Payment\n");
        printf("2. View Payment History\n");
        printf("3. Request a Refund\n");
        printf("4. View Refund Requests\n");
        printf("5. Back to Student Menu\n");
        printf("Enter your choice (1-5): ");

        if (!readIntInRange(1, 5, &choice)) {
            continue;
        }

        switch (choice) {
            case 1:
                makePayment(studentId);
                break;
            case 2:
                viewPaymentHistory(studentId);
                break;
            case 3:
                requestRefund(studentId, studentName);
                break;
            case 4:
                viewRefundStatus(studentName);
                break;
            case 5:
                break;
        }
    } while (choice != 5);
}

void makePayment(char *studentId) {
    char paymentId[MAX_ID];
    double amount;
    char date[MAX_DATE];

    if (!generateNextPaymentId(paymentId, sizeof(paymentId))) {
        return;
    }

    printf("\n--- Make Payment ---\n");
    printf("Generated Payment ID: %s\n", paymentId);

    printf("Enter payment amount: ");
    if (scanf("%lf", &amount) != 1 || amount <= 0.0) {
        clearInputBuffer();
        printf("Invalid amount entered. Please enter a positive number.\n");
        return;
    }
    clearInputBuffer();

    time_t now = time(NULL);
    struct tm *localTime = localtime(&now);
    if (localTime == NULL) {
        printf("Error: Unable to get current system date.\n");
        return;
    }
    strftime(date, sizeof(date), "%Y-%m-%d", localTime);

    FILE *payFp = fopen(PAYMENT_FILE, "a");
    if (payFp == NULL) {
        printf("Error: Unable to save payment record to '%s'.\n", PAYMENT_FILE);
        return;
    }

    fprintf(payFp, "%s %s %.2f %s\n", paymentId, studentId, amount, date);
    fclose(payFp);

    printf("Payment %s (%.2f on %s) recorded successfully!\n", paymentId, amount, date);
}

void viewPaymentHistory(char *studentId) {
    FILE *fp = fopen(PAYMENT_FILE, "r");
    if (fp == NULL) {
        printf("No payment records found in '%s' yet.\n", PAYMENT_FILE);
        return;
    }

    printf("\n--- Payment History (Student ID: %s) ---\n", studentId);
    char line[MAX_LINE];
    int found = 0;
    double totalAmount = 0.0;

    while (fgets(line, sizeof(line), fp) != NULL) {
        char pId[MAX_ID], sId[MAX_ID], date[MAX_DATE];
        double amount;
        if (sscanf(line, "%19s %19s %lf %19s", pId, sId, &amount, date) == 4) {
            if (strcmp(sId, studentId) == 0) {
                printf("Payment ID: %-8s | Amount: %8.2f | Date: %s\n", pId, amount, date);
                totalAmount += amount;
                found = 1;
            }
        }
    }
    fclose(fp);

    if (!found) {
        printf("No payment records found for Student ID '%s' yet.\n", studentId);
    } else {
        printf("------------------------------------------------\n");
        printf("Total Paid: %.2f\n", totalAmount);
    }
}

void requestRefund(char *studentId, char *studentName) {
    char paymentId[MAX_ID], refundId[MAX_ID], reason[MAX_REASON];
    printf("\n--- Request a Refund ---\n");
    viewPaymentHistory(studentId);
    printf("\nEnter Payment/Bill ID to refund (e.g. 5001): ");
    scanf("%19s", paymentId);
    
    clearInputBuffer();

    FILE *payFp = fopen(PAYMENT_FILE, "r");
    if (payFp == NULL) {
        printf("Error: '%s' not found. Cannot verify payment record.\n", PAYMENT_FILE);
        return;
    }

    char line[MAX_LINE];
    int paymentFound = 0;
    double paidAmount = 0.0;
    while (fgets(line, sizeof(line), payFp) != NULL) {
        char pId[MAX_ID], sId[MAX_ID], date[MAX_DATE];
        double amount;
        if (sscanf(line, "%19s %19s %lf %19s", pId, sId, &amount, date) == 4) {
            if (strcmp(pId, paymentId) == 0 && strcmp(sId, studentId) == 0) {
                paymentFound = 1;
                paidAmount = amount;
                break;
            }
        }
    }
    fclose(payFp);

    if (!paymentFound) {
        printf("Payment ID '%s' not found under your Student ID (%s).\n", paymentId, studentId);
        return;
    }

    generateNexrtRefundId(refundId, sizeof(refundId));
    printf("Generated Refund ID: %s\n", refundId);

    printf("Enter refund reason: ");
    if (fgets(reason, sizeof(reason), stdin) == NULL) {
        return;
    }
    reason[strcspn(reason, "\r\n")] = '\0';

    FILE *refFp = fopen(REFUND_FILE, "a");
    if (refFp == NULL) {
        printf("Error: Unable to save refund request to '%s'.\n", REFUND_FILE);
        return;
    }

    fprintf(refFp, "%s|%s|%s|%.2f|%s|Pending\n",
            refundId, paymentId, studentName, paidAmount, reason);
    fclose(refFp);

    printf("Refund request %s for Payment %s (%.2f) saved to '%s' with status 'Pending'.\n",
           refundId, paymentId, paidAmount, REFUND_FILE);
}

void viewRefundStatus(char *studentName) {
    FILE *fp = fopen(REFUND_FILE, "r");
    if (fp == NULL) {
        printf("No refund records found in '%s' yet.\n", REFUND_FILE);
        return;
    }

    printf("\n--- My Refund Requests (%s) ---\n", studentName);
    char line[MAX_LINE];
    int found = 0;

    while (fgets(line, sizeof(line), fp) != NULL) {
        char rId[MAX_ID], pId[MAX_ID], name[MAX_NAME], reason[MAX_REASON], status[MAX_STATUS];
        double amount;
        if (sscanf(line, "%19[^|]|%19[^|]|%49[^|]|%lf|%99[^|]|%19[^\r\n]",
                   rId, pId, name, &amount, reason, status) == 6) {
            if (strcmp(name, studentName) == 0) {
                printf("RefundID: %-6s | Bill: %-6s | Amt: %7.2f | Reason: %-22s | Status: %s\n",
                       rId, pId, amount, reason, status);
                found = 1;
            }
        }
    }
    fclose(fp);

    if (!found) {
        printf("No refund requests found for '%s' in '%s'.\n", studentName, REFUND_FILE);
    }
}

void instructorRatingMenu(char *studentId) {
    int choice;
    do {
        printf("\n--- Instructor Rating ---\n");
        printf("1. Rate an Instructor\n");
        printf("2. View Instructor Average Ratings\n");
        printf("3. Back to Student Menu\n");
        printf("Enter your choice (1-3): ");

        if (!readIntInRange(1, 3, &choice)) {
            continue;
        }

        switch (choice) {
            case 1:
                rateInstructor(studentId);
                break;
            case 2:
                viewInstructorAverages();
                break;
            case 3:
                break;
        }
    } while (choice != 3);
}

void rateInstructor(char *studentId) {
    char instructorId[MAX_ID];
    char instName[MAX_NAME], instStyle[MAX_STYLE], instStatus[MAX_STATUS];
    float ratingScore;

    printf("\n--- Rate Instructor ---\n");
    displayAllInstructors();

    if (!hasAnyInstructors()) {
        return;
    }

    printf("\nEnter Instructor ID: ");
    if (scanf("%19s", instructorId) != 1) {
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    if (!lookupInstructorInfo(instructorId, instName, instStyle, instStatus)) {
        printf("Instructor ID '%s' not found in '%s'.\n", instructorId, INSTRUCTOR_FILE);
        return;
    }

    if (strcmp(instStatus, "Active") != 0) {
        printf("Notice: Instructor %s (%s) is currently '%s'.\n",
               instructorId, instName, instStatus);

        return;
    }

    printf("Enter rating score for %s (%s) [1.0 to 5.0]: ", instName, instStyle);
    if (scanf("%f", &ratingScore) != 1 || ratingScore < 1.0f || ratingScore > 5.0f) {
        clearInputBuffer();
        printf("Invalid score. Please enter a value between 1.0 and 5.0.\n");
        return;
    }
    clearInputBuffer();

    FILE *rateFp = fopen(RATING_FILE, "a");
    if (rateFp == NULL) {
        printf("Error: Unable to save rating record to '%s'.\n", RATING_FILE);
        return;
    }

    fprintf(rateFp, "%s,%s,%.1f\n", studentId, instructorId, ratingScore);
    fclose(rateFp);

    printf("Thank you! Rating %.1f for %s (%s) submitted.\n",
           ratingScore, instName, instructorId);
}

void viewInstructorAverages(void) {
    FILE *fp = fopen(RATING_FILE, "r");
    if (fp == NULL) {
        printf("No rating records found in '%s' yet.\n", RATING_FILE);
        return;
    }

    char seenInstructors[200][MAX_ID];
    int seenCount = 0;
    char line[MAX_LINE];

    while (fgets(line, sizeof(line), fp) != NULL) {
        char sId[MAX_ID], instId[MAX_ID];
        float score;
        if (sscanf(line, "%19[^,],%19[^,],%f", sId, instId, &score) == 3) {
            int alreadySeen = 0;
            for (int i = 0; i < seenCount; i++) {
                if (strcmp(seenInstructors[i], instId) == 0) {
                    alreadySeen = 1;
                    break;
                }
            }
            if (!alreadySeen && seenCount < 200) {
                strcpy(seenInstructors[seenCount], instId);
                seenCount++;
            }
        }
    }
    fclose(fp);

    if (seenCount == 0) {
        printf("No ratings available yet.\n");
        return;
    }

    printf("\n--- Instructor Average Ratings ---\n");
    for (int i = 0; i < seenCount; i++) {
        int count = 0;
        float avg = calcInstructorAvg(seenInstructors[i], &count);
        if (avg >= 0.0f) {
            char instName[MAX_NAME], instStyle[MAX_STYLE], instStatus[MAX_STATUS];
            lookupInstructorInfo(seenInstructors[i], instName, instStyle, instStatus);
            printf("Instructor: %-5s (%-10s | %-10s) | Average: %.2f / 5.0 | Total Ratings: %d\n",
                   seenInstructors[i], instName, instStyle, avg, count);
        }
    }
}
