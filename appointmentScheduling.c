#include <stdio.h>
#include <windows.h>
#include <stdbool.h>
#include <cJSON.h>
/*!
    This portion initially assigned to Jacob
*/

//have to include these in this file, but can't #include <calendar.c> because of a LOT of strange problems
struct Appointment {
    int id;
    unsigned short startTime;
    unsigned short endTime;
    char customer[32];
    char appointmentType[32];
    char employee[32];

    struct Appointment *prevAppointment;
    struct Appointment *nextAppointment;
};

struct CalDate {
    unsigned short year;
    unsigned short month;
    unsigned short day;

    struct Appointment *firstAppointment;
};


void appointmentScheduling(){
    int inputchar = 1;
    setupWeek();
    SetConsoleTitle("Customer Scheduling");

    system("cls");

    /*
    char datestr1[11] = "12/12/2023";
    char datestr2[11] = "1/23/2024";
    char datestr3[11] = "11/7/2024";
    char datestr4[11] = "1/3/2024";
    char datestr5[11] = "12/12/23";
    char datestr6[11] = "12/24";
    char datestr7[11] = "1/2";
    char datestr8[11] = "4/5/25";

    addformatdatestr(datestr1);
    addformatdatestr(datestr2);
    addformatdatestr(datestr3);
    addformatdatestr(datestr4);
    addformatdatestr(datestr5);
    addformatdatestr(datestr6);
    addformatdatestr(datestr7);
    addformatdatestr(datestr8);

    printf("|%s|\n|%s|\n|%s|\n|%s|\n|%s|\n|%s|\n|%s|\n|%s|\n", datestr1, datestr2, datestr3, datestr4, datestr5, datestr6, datestr7, datestr8); getch();
*/



    loadAppListUnordered();
    loadAppsIntoMemory();

    while(inputchar != 'e'){
        printf("\n");


        printWeek();

        printf("\n\n(F)orward a week\n(B)ackward a week\n(V)iew a single day (to add/remove/modify individual appointments)\n(E)xit\n\nMake a selection: ");
        inputchar = getch();
        if(inputchar >= 'A' && inputchar <= 'Z') inputchar += 32; //handles input in lowercase, so change uppercase to lowercase if needed

        switch(inputchar){
            case 'f': shiftWeek(7); loadAppsIntoMemory(); break;
            case 'b': shiftWeek(-7); loadAppsIntoMemory(); break;
            case 'e': break;
            //case 'i': filetest(); break; //this is temporary. will only work with a file 'testfile.csv' formatted in a very specific way
            case 'v': appointmentLoop(); break;
        }
        system("cls");
    }
}

void appointmentLoop(){
    int inputchar = 0;
    struct CalDate *date;

    printf("Please enter day of week to edit (1-7 for Sun-Sat) : ");
    inputchar = getch();

    while(inputchar < '1' || inputchar > '7'){
        printf("Invalid input error\nPlease enter day of week to edit (1-7 for Sun-Sat) : ");
    inputchar = getch();
    }
    printf("\n");
    inputchar -= 49;

    date = getCalDate(inputchar);

    system("cls");

    while(inputchar != 'e'){
        printDayCal(date);

        printf("(A)dd an appointment\n(M)odify an appointment\n(R)emove an appointment\n(E)xit\n\nMake a selection: ");
        inputchar = getch();
        if(inputchar >= 'A' && inputchar <= 'Z') inputchar += 32;

        switch(inputchar){
            case 'e': break;
            case 'a': printf("add\n"); addAppointment(date); break;
            case 'm': printf("edit\n"); editAppointment(date); break;
            case 'r': printf("remove\n"); deleteAppointment(date); break;
            default: system("cls"); break;
        }
    }
}

void addAppointment(struct CalDate *date){
    struct Appointment *app;
    app = malloc(sizeof(*app));

    app -> startTime = NULL;
    app -> endTime = NULL;
    clearappstr(app -> customer);
    clearappstr(app -> appointmentType);
    strcpy(app -> employee, currentemployeename());

    char datestr[11];
    strcpy(datestr, makedatestr(date));
    editing(app, datestr);
    if(confirmapp(app, datestr)){
        insertAppointment(app, datestr);
        printf("\nname=%s start=%d end=%d", app->customer, app->startTime, app->endTime);
        addAppToDatabase(app, datestr);
    }
}

void clearappstr(char *str){
    for(int i = 0; i < 32; i++){
        str[i] = '\0';
    }
}

void deleteAppointment(struct CalDate *date){
    system("cls");
    int inputnum = 0, counter;
    char inputstr[32];
    struct Appointment *app;
    app = date -> firstAppointment;

    if(app == NULL){
        printf("Error: no appointments exist for this date. Press any key to continue");
        getch();
        system("cls");
        return;
    }

    counter = printAppointmentsNumbered(app);

    printf("Please select an appointment from the above list to delete (1 - %d): ", counter);
    fgets(inputstr, 32, stdin);
    inputnum = strtol(inputstr, NULL, 10);
    while(inputnum == 0 || inputnum > counter){
        system("cls");
        printAppointmentsNumbered(app);
        printf("Error, invalid number entered\nPlease select an appointment from the above list to delete (1 - %d): ", counter);
        fgets(inputstr, 32, stdin);
        inputnum = strtol(inputstr, NULL, 10);
    }
    inputnum--; //code starts at 0, humans start at 1

    app = date -> firstAppointment;
    while(inputnum > 0){
        app = app -> nextAppointment;
        inputnum--;
    }
    int inputchar = 0;
    printf("Time: %s - %s\nCustomer: %s\nEmployee username:%s\nType: %s\n\n", maketimestring(app -> startTime), maketimestring(app -> endTime), app -> customer, app -> employee, app -> appointmentType);
    printf("Are you sure you want to remove this appointment? This is PERMANENT! [(Y)es/(N)o]: ");
    while(inputchar != 'y' && inputchar != 'n' && inputchar != 'Y' && inputchar != 'N'){
        inputchar = getch();
    }

    if(inputchar == 'y' ||inputchar == 'Y'){
        removeAppFromDatabase(app);
        removethisapp(app, makedatestr(date));
        printf("\nid outside=%d", app->id);

    }
    system("cls");
}

void editAppointment(struct CalDate *date){
    system("cls");
    int inputnum = 0, counter;
    char inputstr[32];
    struct Appointment *app;
    app = date -> firstAppointment;

    if(app == NULL){
        printf("Error: no appointments exist for this date. Press any key to continue");
        getch();
        system("cls");
        return;
    }

    counter = printAppointmentsNumbered(app);

    printf("Please select an appointment from the above list to edit (1 - %d): ", counter);
    fgets(inputstr, 32, stdin);
    inputnum = strtol(inputstr, NULL, 10);
    while(inputnum == 0 || inputnum > counter){
        system("cls");
        printAppointmentsNumbered(app);
        printf("Error, invalid number entered\nPlease select an appointment from the above list to edit (1 - %d): ", counter);
        fgets(inputstr, 32, stdin);
        inputnum = strtol(inputstr, NULL, 10);
    }
    inputnum--;


    app = date -> firstAppointment;
    while(inputnum > 0){
        app = app -> nextAppointment;
        inputnum--;
    }

    struct Appointment *oldapp = app;
    app = cloneapp(app);

    char datestr[11];
    strcpy(datestr, makedatestr(date));
    editing(app, datestr);

    if(confirmapp(app, datestr)){
        insertAppointment(app, datestr);
        printf("\nid from here: %d", app->id);
        editAppInDatabase(app, datestr);
        removethisapp(oldapp, makedatestr(date));
    }
}

void editing(struct Appointment *app, char *datestr){
    char inputstr[32];
    int inputchar, editingvalue = 7, strindex = 0;
    bool exit = false, typing, editvalue = false;

    int howmany = 0; //LITERALLY HOW DOES SIMPLY DEFINING THIS FIX A BUG
    /*not sure why this happens but without defining this variable (and nothing else) the code will get completely stuck...
    you won't be able to navigate through fields with the arrow keys, and typing 'exit' does not work. ??????*/

    while(exit != true){
        system("cls");

        printf("Date: %s\nTime: %s - %s\nCustomer: %s\nEmployee username: %s\nType: %s\n\n", datestr, maketimestring(app -> startTime), maketimestring(app -> endTime), app -> customer, app -> employee, app -> appointmentType);
        printf("(Type a value to enter it in current field, or navigate between fields with the up and down arrow keys.)\n\nCurrently editing:\n");

        switch(editingvalue){
            case 1: printf("Date: "); break;
            case 2: printf("Start time: "); break;
            case 3: printf("End time: "); break;
            case 4: printf("Customer name: "); break;
            case 5: printf("Employee username or ID: "); break;
            case 6: printf("Appointment type: "); break;
            case 0: case 7: printf("No value selected, type 'exit' to exit: ");
        }
        typing = true;
        while(typing == true){
                inputchar = getch();
                if(inputchar == 224){ //weird code for detecting arrow keys
                    inputchar = getch(); //actual key pressed comes after the code
                    if(inputchar == 72){ //up arrow code
                        if(editingvalue > 0) editingvalue--;
                        else if(editingvalue == 0) editingvalue = 6; //pressing up arrow while at upper limit will take you to bottom value
                        typing = false;
                        editvalue = false; //editvalue false so no changes to data made
                    }
                    else if(inputchar == 80){ //down arrow code
                        if(editingvalue < 7) editingvalue++;
                        else if(editingvalue == 7) editingvalue = 1; //pressing down arrow while at lower limit will take you to upper value
                        typing = false;
                        editvalue = false;
                    }
                }
                else if(inputchar == '\b'){ //detect backspace
                    if(strindex != 0){ //make sure you can't backspace when string is empty
                        printf("\b \b");
                        strindex--;
                        if(strindex == 0) editvalue = false; //if string is empty after pressing delete then don't let it overwrite data
                    }
                }
                else if((inputchar == '\n' || inputchar == '\r') && strindex < 32){ //detect enter (checks for space for null character)
                    inputstr[strindex] = '\0';
                    typing = false;
                }
                else if(strindex < 31){ //any other character should be added to the string (as long as it fits)
                    printf("%c", inputchar);
                    inputstr[strindex] = inputchar;
                    strindex++;
                    editvalue = true; //editvalue true because at least 1 character in string, can attempt to change data
                }
        }
        if(editvalue == true){ //to make sure empty/strange strings are not saved when pressing arrows or entering blank string
            switch(editingvalue){
                case 0: case 7: strtolower(inputstr); if(strcmp(inputstr, "exit") == 0){exit = true;} break;
                case 1: datestrcpy(datestr, inputstr); editvalue = false; break;
                case 2: app -> startTime = shortfromtime(inputstr); editvalue = false; break;
                case 3: app -> endTime = shortfromtime(inputstr); editvalue = false; break;
                case 4: printf("\n"); customerstrcpy(app -> customer, inputstr); editvalue = false; break;
                case 5: printf("\n"); empstrcpy(app -> employee, inputstr); editvalue = false; break;
                case 6: strcpy(app -> appointmentType, inputstr); editvalue = false; break;
            } //editvalue changed to false so pressing enter twice doesn't enter data then clear it
        }
        cleareditstring(inputstr);
        strindex = 0;
        if(exit == true){
            if((app -> endTime <= app -> startTime) || (strcmp(app -> customer, "") == 0) || (strcmp(app -> appointmentType, "") == 0)){
                //checks if the appointment ends earlier/at the same time it starts, and if strings are blank
                exit = false;
                editvalue = false;
                printf("\nError, some values are invalid. Please make sure that no data fields are blank and that the appointment ends after it starts.\nPress any key to continue");
                getch();
            }
        }
    }
    system("cls");
}

int confirmapp(struct Appointment *app, char *datestr){
    int inputchar;

    printf("New appointment:\nDate: %s\nTime: %s - %s\nCustomer: %s\nEmployee: %s\nType: %s\n\n", datestr, maketimestring(app -> startTime), maketimestring(app -> endTime), app -> customer, app -> employee, app -> appointmentType);
    printf("\n\nIs this appointment okay? [(Y)es/(N)o]: ");

    while(1){
        inputchar = getch();

        if(inputchar == 'y' || inputchar == 'Y') return 1;
        else if(inputchar == 'n' || inputchar == 'N') return 0;
    }
}

void cleareditstring(char *str){
    for(int i = 0; i < 50; i++){
        str[i] = '\0';
    }
}

void strtolower(char *str){
    while(*str != '\0'){
        if(*str >= 'A' && *str <= 'Z') *str += 32;
        str++;
    }
}
