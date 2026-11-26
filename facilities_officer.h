#ifndef FACILITIES_OFFICER_H
#define FACILITIES_OFFICER_H

/* Symbolic Constants */
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

typedef union {
    int unitDeficit;
    double itemValuation;
} EquipmentMetric;

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

/* Internal Helper and Validation Functions */
void generateNextEquipmentID(char *newID);
void foClearBuffer(void);
void foReadString(const char *prompt, char *buffer, int size);
int foValidateID(const char *id, const char *prefix);
int isEquipmentIDExists(const char *id);

#endif
