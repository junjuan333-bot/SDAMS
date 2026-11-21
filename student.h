#ifndef STUDENT_H
#define STUDENT_H

int studentLogin(char *studentIdInput, char *outStudentId, char *outName,
                 char *outStyle, char *outContact, char *outStatus);

void studentMenu(void);
void studentPortal(char *studentId, char *studentName);

void manageBookings(char *studentId);
void bookClass(char *studentId);
void viewMyBookings(char *studentId);
void rescheduleBooking(char *studentId);
void cancelBooking(char *studentId);

void paymentManagement(char *studentId, char *studentName);
void makePayment(char *studentId);
void viewPaymentHistory(char *studentId);
void requestRefund(char *studentId, char *studentName);
void viewRefundStatus(char *studentName);
void viewInstructorAverages(void);
void instructorRatingMenu(char *studentId);
void rateInstructor(char *studentId);
void viewInstructorRatings(void);

#endif