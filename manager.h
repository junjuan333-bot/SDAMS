#ifndef MANAGER_H
#define MANAGER_H

int manager_log_system(char loggedInId[]);
int manager_register(void);
void manager_main_menu(char currentId[]);
int search_admin(void);
int staff_report(void);
int revenue_report(void);
#endif
