#ifndef FACILITIES_OFFICER_H
#define FACILITIES_OFFICER_H

/* Main Role Functions */
int facilitiesOfficerLogin(void);
void facilitiesOfficerMenu(void);

/* Module A: Equipment Inventory Management */
void addEquipment(void);
void viewEquipment(void);
void searchEquipment(void);
void updateEquipment(void);

/* Module B: Facility Problem Management */
void viewFacilityProblems(void);
void searchFacilityProblems(void);
void updateProblemStatus(void);

/* Module C: Inventory Stock Reports */
void generateStockReport(void);     

/* Internal Helper and Input Validation Utility Functions */
void foClearBuffer(void);
void foReadString(const char *prompt, char *buffer, int size);
int foValidateID(const char *id, const char *prefix);
int isEquipmentIDExists(const char *id);

#endif
