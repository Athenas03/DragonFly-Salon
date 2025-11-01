#include <stdbool.h>
#include <stdio.h>
#include <time.h>

struct Appointment {
    int id;
    unsigned short startTime; //handled internally as numbers from 0000 - 2400 to save some space
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

struct CalDate currentWeek[7];

struct CalDate *getCalDate(int index){
    return &currentWeek[index];
}

void shiftDate(short *month, short *day, short *year, short amount){
    struct tm *newtime;
    time_t t;

    time(&t);
    newtime = localtime(&t);

    newtime -> tm_mday = *day + amount;
    newtime -> tm_mon = *month - 1; // tm stores month as value from 0 - 11
    newtime -> tm_year = *year - 1900; // tm stores year as value starting from 1900

    t = mktime(newtime);

    char timestr[11];
    strftime(timestr, sizeof(char[11]), "%m/%d/%Y", newtime);

    interpretDate(timestr, month, day, year);
}

void interpretDate(char *dateptr, short *month, short *day, short *year){
    *month = strtol(dateptr, &dateptr, 10);

    dateptr++;
    *day = strtol(dateptr, &dateptr, 10);

    dateptr++;
    *year = strtol(dateptr, &dateptr, 10);
}

void addformatdatestr(char *datestr){
    //make sure datestr comes out as mm/dd/yyyy
    //inputs possible are: month/day are 1-9 (single char), year is 2 chars instead of 4, year is absent. default to current year for year cases
    if(datestr[1] == '/'){ //month is single digit
        for(int i = 10; i > 0; i--){
            datestr[i] = datestr[i - 1]; //shift all the characters over by 1
        }
        datestr[0] = '0'; //make first character 0
    }
    if(datestr[4] == '/' || datestr[4] == '\0'){ //day is single digit
        for(int i = 10; i > 3; i--){
            datestr[i] = datestr[i - 1]; //shift
        }
        datestr[3] = '0';
    }

    if(datestr[5] == '\0' || datestr[6] == '\0'){ //if no year provided
        struct tm * timestruct;
        time_t t;
        time(&t);
        timestruct = localtime(&t);
        char yearstr[5];
        sprintf(yearstr, "%d", ((timestruct -> tm_year) + 1900));

        datestr[5] = '/';
        datestr[6] = yearstr[0];
        datestr[7] = yearstr[1];
        datestr[8] = yearstr[2];
        datestr[9] = yearstr[3];
    }
    else if(datestr[8] == '\0'){ //if last two digits of year are provided
        datestr[10] = '\0';
        datestr[9] = datestr[7];
        datestr[8] = datestr[6];
        datestr[7] = '0';
        datestr[6] = '2'; //hardcoded for 20xx
    }
}

char *makedatestr(struct CalDate *cday){
    char *datestr = malloc(sizeof(char[11]));
    sprintf(datestr, "%d/%d/%d", cday -> month, cday -> day, cday -> year);

    addformatdatestr(datestr);

    return datestr;
}

int validdatestr(char *datestr){
    int returnval = 1;
    addformatdatestr(datestr);
    for(int i = 0; i < 11; i++){ //run through each character individually, if one doesn't fit change returnval to 0 because invalid
        switch(i){ // format xx/xx/xxxx with null character terminating
            case 0: case 1: case 3: case 4: case 6: case 7: case 8: case 9: if((datestr[i] >= '0' && datestr[i] <= '9') == 0){returnval = 0;} break;
            case 2: case 5: if(datestr[i] != '/'){returnval = 0;} break;
            case 10: if(datestr[i] != '\0'){returnval = 0;} break;
        }
    }
    return returnval;
}

void datestrcpy(char *in, char* sc){
    if(validdatestr(sc)){
        for(int i = 0; i < 11; i++){
            in[i] = sc[i];
        }
    }
}

void currentSunday(short *month, short *day, short *year){
    struct tm *currenttime;
    time_t t;

    t = time(NULL);
    currenttime = localtime(&t);

    char timestr[11];
    char weekday[4];

    strftime(timestr, sizeof(char[11]), "%m/%d/%Y", currenttime);
    interpretDate(timestr, month, day, year);

    const char *weekdays[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    for(int i = 1; i < 7; i++){
        strftime(weekday, sizeof(char[4]), "%a", currenttime);

        if(strcmp(weekdays[i], weekday) == 0){
            shiftDate(month, day, year, (i * -1));
            break;
        }
    }

}

void setupWeek(){
    struct CalDate *date;

    short month, day, year;

    currentSunday(&month, &day, &year);

    for(int i = 0; i < 7; i++){
        date = malloc(sizeof(*date));
        currentWeek[i] = *date;

        currentWeek[i].month = month;
        currentWeek[i].day = day;
        currentWeek[i].year = year;

        shiftDate(&month, &day, &year, 1);

        currentWeek[i].firstAppointment = NULL;
    }
}

void shiftWeek(short amount){
    short month, day, year;

    month = currentWeek[0].month;
    day = currentWeek[0].day;
    year = currentWeek[0].year;

    shiftDate(&month, &day, &year, amount);
    currentWeek[0].month = month;
    currentWeek[0].day = day;
    currentWeek[0].year = year;

    unloadAppointments(0);

    for(int i = 1; i < 7; i++){
        shiftDate(&month, &day, &year, 1);
        currentWeek[i].month = month;
        currentWeek[i].day = day;
        currentWeek[i].year = year;

        unloadAppointments(i);
    }
}

char* maketimestring(short input){
    char *timestring = malloc(sizeof(char[8]));
    timestring[2] = ':';
    timestring[6] = 'm';
    timestring[7] = '\0';

    //figure out if [0] = '1' or ' '
    if(input < 60 || input > 2159 || (input > 959 && input < 1300)) timestring[0] = '1';
    else timestring[0] = ' ';
    //figure out what [1] is
    if(input < 100) timestring[1] = '2'; //12am
    else if(input > 1259) timestring[1] = ((input / 100) % 10) + 46; //add 48 to turn number into ascii char, subtract 2 to fix 2hr offset from 24hr time
    else timestring[1] = ((input / 100) % 10) + 48; //add 48 to turn into ascii char, no offset because input <= 1259
    //figure out what [3] and [4] are
    timestring[3] = ((input / 10) % 10) + 48;
    timestring[4] = (input % 10) + 48;
    //figure out if [5] = 'a' or 'p'
    if(input < 1200) timestring[5] = 'a';
    else timestring[5] = 'p';

    return timestring;
}

int shortfromtime(char *str){
    char cleanstr[5];
    int index = 0, returnval = 0;
    for(int i = 0; i < 32; i++){
        switch(str[i]){ //if character in str is number, add it to cleanstr
            case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': case '0': cleanstr[index] = str[i]; index++; break;
        }
        if(index > 3) break; //cleanstr can only hold 4 digit number + null char
    }
    cleanstr[4] = '\0';

    returnval = strtol(cleanstr, NULL, 10);

    for(int i = 32; i > 3; i--){ //scan backwards through string
        if((str[i] == 'm' || str[i] == 'M') && (str[i - 1] == 'p' || str[i - 1] == 'P')){ //if there is a "pm"
            if(returnval < 1200) returnval += 1200; //if they didn't enter 12:xx pm then add 12 to make work internally. anything under 12pm ex 11:30pm would be 1130, then make 2330 for internal
            break;
        }
        else if((str[i] == 'm' || str[i] == 'M') && (str[i - 1] == 'a' || str[i - 1] == 'A')){ //detect am for special 12am case
            if(returnval >= 1200) returnval -= 1200;
            break;
        }
    }
    //potentially try to check if time xxXx where capital X should be <= 5 so no weird stuff with like 12:65 pm

    return returnval;
}

void printWeek(){
    printf("                                                                       %d - %d\n", currentWeek[0].year, currentWeek[1].year);
    printf("         Sun                   Mon                   Tue                   Wed                   Thu                   Fri                   Sat\n");
    printf("=------ ");
    if(currentWeek[0].month < 10) printf(" ");
    if(currentWeek[0].day < 10) printf(" "); //if whitespace is needed
    printf("%d/%d ", currentWeek[0].month, currentWeek[0].day);
    for(int i = 1; i < 7; i++){
        printf("-------=------- ");
        if(currentWeek[i].month < 10) printf(" ");
        if(currentWeek[i].day < 10) printf(" "); //print whitespace for either number that is single digit
        printf("%d/%d ", currentWeek[i].month, currentWeek[i].day);
    }
    printf("------=\n");


    struct Appointment *app;
    int numapps = 0;
    for(int r = 0; r < 5; r++){ //5 rows (this value can be changed but others cannot)
        printf("= ");
        for(int i = 0; i < 7; i++){ //7 days per row
                app = currentWeek[i].firstAppointment;
                while(app != NULL && numapps < r){ //cycle through appointment relative to which row
                    app = app -> nextAppointment;
                    numapps++;
                }
                numapps = 0; //reset counter for next day
                if(app != NULL){
                    printf("%s - %s", maketimestring(app -> startTime), maketimestring(app -> endTime));
                }
                else{
                    printf("                 "); //17 chars
                }
                //end of five chars of " --- ", or end of simply " =" if last item in row
                if(i == 6) printf(" =");
                else printf(" --- ");
        }
        printf("\n");
    }

    for(int i = 0; i < 153; i++) printf("-");
    printf("\n");
}
/*
void printAppointments(int index){
    if(currentWeek[index].firstAppointment == NULL){
        printf("No appointments for this date.");
        return;
    }

    struct Appointment *app = currentWeek[index].firstAppointment;
    while(app -> nextAppointment != NULL){
        printf("Time:%d-%d - Customer: %s - Type: %s\n", app -> startTime, app -> endTime, app -> customer, app -> appointmentType);
        app = app -> nextAppointment;
    }
    printf("Time:%d-%d - Customer: %s - Type: %s\n", app -> startTime, app -> endTime, app -> customer, app -> appointmentType);
}
*/
void printAppointmentsCal(struct CalDate *cal){
    if(cal -> firstAppointment == NULL){
        printf("No appointments for this date.");
        return;
    }

    struct Appointment *app = cal -> firstAppointment;
    while(app != NULL){
        printf("Time:%s-%s - Customer: %s - Employee username: %s - Type: %s\n", maketimestring(app -> startTime), maketimestring(app -> endTime), app -> customer, app -> employee, app -> appointmentType);
        app = app -> nextAppointment;
    }
}

int printAppointmentsNumbered(struct Appointment *app){
    int i = 1;
    while(app != NULL){
        printf(" %d | %s - %s | Customer: %s | Employee username: %s | Type: %s\n", i, maketimestring(app -> startTime), maketimestring(app -> endTime), app -> customer, app -> employee, app -> appointmentType);
        i++;
        app = app -> nextAppointment;
    }
    return i - 1; //correct the adding 1 before finding null app
}

void printDayCal(struct CalDate *cal){
    printf("----- %d/%d/%d -----\nAppointments:\n", cal -> month, cal -> day, cal -> year);
    printAppointmentsCal(cal);
    printf("\n---------------------\n\n\n");
}

void printEmployeeWeek(char *emp){
    printf("                                                                       %d - %d", currentWeek[0].year, currentWeek[1].year);
    if(emp != NULL){
        printf(", for employee %s\n", emp);
    }
    else{
        printf(", for all employees\n");
    }
    printf("         Sun                   Mon                   Tue                   Wed                   Thu                   Fri                   Sat\n");
    printf("=------ ");
    if(currentWeek[0].month < 10) printf(" ");
    if(currentWeek[0].day < 10) printf(" "); //if whitespace is needed
    printf("%d/%d ", currentWeek[0].month, currentWeek[0].day);
    for(int i = 1; i < 7; i++){
        printf("-------=------- ");
        if(currentWeek[i].month < 10) printf(" ");
        if(currentWeek[i].day < 10) printf(" "); //print whitespace for either number that is single digit
        printf("%d/%d ", currentWeek[i].month, currentWeek[i].day);
    }
    printf("------=\n");


    struct Appointment *apps[7] = {currentWeek[0].firstAppointment,
    currentWeek[1].firstAppointment,
    currentWeek[2].firstAppointment,
    currentWeek[3].firstAppointment,
    currentWeek[4].firstAppointment,
    currentWeek[5].firstAppointment,
    currentWeek[6].firstAppointment};

    for(int r = 0; r < 5; r++){ //5 rows
        printf("= ");
        for(int i = 0; i < 7; i++){ //7 days per row
            //go until apps[i] holds app with valid employee, print it, then go next.

            while(apps[i] != NULL && emp != NULL){
                if(strcmp(apps[i] -> employee, emp) == 0){
                    break;
                }
                else{
                    apps[i] = apps[i] -> nextAppointment;
                }
            }

            if(apps[i] != NULL && strcmp(apps[i] -> employee, "INVALID") != 0){
                printf("%s - %s", maketimestring(apps[i] -> startTime), maketimestring(apps[i] -> endTime));
            }
            else{
                printf("                 "); //17
            }
            //print separator and end bit
            if(i == 6) printf(" =");
            else printf(" --- ");

            if(apps[i] != NULL) apps[i] = apps[i] -> nextAppointment;
        }
        printf("\n");
    }


/*
    struct Appointment *app;
    int numapps[7] = {-1, -1, -1, -1, -1, -1, -1};
    for(int r = 0; r < 5; r++){ //5 rows (this value can be changed but others cannot)
        printf("= ");
        for(int i = 0; i < 7; i++){ //7 days per row
            app = currentWeek[i].firstAppointment;

            if(emp != NULL && app != NULL){
                if(strcmp(app -> employee, emp) == 0){
                    numapps[i]++;
                }
            }

            while(app != NULL && numapps[i] < r){ //cycle through appointments relative to how many appointments with current employee have been printed
                app = app -> nextAppointment;

                if(emp != NULL && app != NULL){ //test this first to prevent crash from passing null to strcmp()
                    //printf("--");
                    //printf("|%d", numapps[i]);
                    //printf("|%s", emp);
                    //printf("|%s|", app -> employee);
                    if(strcmp(app -> employee, emp) == 0){
                        numapps[i]++;
                    }
                }
                else numapps[i]++; //if emp == null then all employees selected so print all
            }

            numapps[i] = -1;

            if(app != NULL){
                printf("%s - %s", maketimestring(app -> startTime), maketimestring(app -> endTime));
            }
            else{
                printf("                 "); //17
            }
            //print spacers and end bit
            if(i == 6) printf(" =");
            else printf(" --- ");
*/
                /*
                app = currentWeek[i].firstAppointment; // !!!!!! CYCLE THROUGH THESE RELATIVE TO HOW MANY APPOINTMENTS FOUND WITH EMPLOYEE
                while(app != NULL && numapps < r){ //cycle through appointment relative to which row
                    app = app -> nextAppointment;
                    numapps++;
                }
                numapps = 0; //reset counter for next day
                if(app != NULL){
                    if(emp != NULL){ //need to test this first, to prevent crash from passing null to strcmp
                        if(strcmp(app -> employee, emp) == 0){
                            printf("%s - %s", maketimestring(app -> startTime), maketimestring(app -> endTime));
                        }
                        else{
                            printf("                 "); //17
                        }
                    }
                    else{ // if emp == null, all employees is selected, so always print
                        printf("%s - %s", maketimestring(app -> startTime), maketimestring(app -> endTime));
                    }
                }
                else{
                    printf("                 "); //17
                }
                //end of five chars of " --- ", or end of simply " =" if last item in row
                if(i == 6) printf(" =");
                else printf(" --- ");
                */
//        }
//        printf("\n");
//    }

    for(int i = 0; i < 153; i++) printf("-");
    printf("\n");
}

void printScheduleMatchingEmployee(struct CalDate *cal, char *emp){
    printf("----- Scheduled for %d/%d/%d");
}
/*
void printDay(int index){
    printf("----- %d/%d/%d -----\nAppointments:\n", currentWeek[index].month, currentWeek[index].day, currentWeek[index].year);
    printAppointments(index);
    printf("\n---------------------\n\n\n");
}
*/
void insertAppointment(struct Appointment *appointment, char *datestr){
    short month, day, year, cday = -1;
    interpretDate(datestr, &month, &day, &year);

    if(strcmp(appointment->appointmentType, "CANCELLED")==0){
        return;
    }
    for(int i = 0; i < 7; i++){
        if(currentWeek[i].month == month && currentWeek[i].day == day && currentWeek[i].year == year){
            cday = i;
        }
    }

    if(cday == -1){
        //printf("Appointment could not be inserted. You might not be on the correct week.\n", cday);
        return;
    }

    if(currentWeek[cday].firstAppointment == NULL){
        appointment -> prevAppointment = NULL;
        appointment -> nextAppointment = NULL;
        currentWeek[cday].firstAppointment = appointment;
    }
    else{
        struct Appointment *app = currentWeek[cday].firstAppointment, *prevapp;

        if(shouldInsert(appointment, app)){ //check first item

            appointment -> nextAppointment = app;
            appointment -> prevAppointment = NULL;
            app -> prevAppointment = appointment;
            currentWeek[cday].firstAppointment = appointment;

            return; //if we insert we are done
        }
        else if(app -> nextAppointment == NULL){ //if above check fails (inserted comes later) and only one element currently

            app -> nextAppointment = appointment;
            appointment -> prevAppointment = app;
            appointment -> nextAppointment = NULL;

            return; //done
        }
        prevapp = app;
        app = app -> nextAppointment; //go to next appointment to start checking appointments

        while(app != NULL){ //loop through appointments
            if(shouldInsert(appointment, app)){ //if we insert ... change all values appropriately
                appointment -> nextAppointment = app;
                appointment -> prevAppointment = prevapp;
                prevapp -> nextAppointment = appointment;
                app -> prevAppointment = appointment;
                return; //we are done
            }
            prevapp = app;
            app = app -> nextAppointment;
        }
        //if it wasn't inserted within the loop then it belongs at the very end
        app = prevapp;
        app -> nextAppointment = appointment;
        appointment -> prevAppointment = app;
        appointment -> nextAppointment = NULL;
    }
}

//cannot simply make these functions return bools for some reason ...
int shouldInsert(struct Appointment *insert, struct Appointment *current){ //compares based on whether insert should be placed before current. compares start time, then end time, then customer name
    if(insert -> startTime < current -> startTime){
        return 1; }// if insert starts earlier, should be inserted
    else if(insert -> startTime > current -> startTime){
        return 0; }// if insert starts later, should not be inserted
    else if(insert -> endTime < current -> endTime){
        return 1; }// if start time is same ... if insert ends earlier, should be inserted
    else if(insert -> endTime > current -> endTime){
        return 0; }// if insert ends later, should not be inserted

    // compare names alphabetically ... shouldn't be case sensitive ... only return true if insert name comes before current name. otherwise false. no check for == or comes later.
    return cmpnames(insert -> customer, current -> customer);
}

int cmpnames(char* s1, char* s2){ //true only if s1 alphabetically comes before s2. if s1 == s2 returns false
    char *uppers1[32], *uppers2[32];

    for(int i = 0; i < 32; i++){ //copy and convert s1 to uppercase
        uppers1[i] = *s1;
        s1++;
        if(uppers1[i] >= 'a' && uppers1[i] <= 'z') uppers1[i] -= 32;

        if(uppers1[i] == '\0') i = 32; //to break for loop
    }

    for(int i = 0; i < 32; i++){ //copy and convert s2 to uppercase
        uppers2[i] = *s2;
        s2++;
        if(uppers2[i] >= 'a' && uppers2[i] <= 'z') uppers2[i] -= 32;

        if(uppers2[i] == '\0') i = 32; //to break for loop
    }

    int i = 0;
    while(uppers1[i] == uppers2[i] && uppers1[i] != '\0' && uppers2[i] != '\0'){ //look for first different character between strings
        i++;
    }

    if(uppers1[i] < uppers2[i]){
        return 1;
    }
    else{
        return 0;
    }
}
/* //i guess this was only necessary for the file functions lol
void createAppointment(char *inputline){
    short typestart;
    typestart = fourthapos(inputline);

    if(typestart == -1){
        printf("Error, potentially invalid string read into memory\n%s , %d\n", inputline, typestart);
        return NULL;
    }
    struct Appointment *newAppointment;
    newAppointment = malloc(sizeof(*newAppointment));

    char datestr[11], buffer[25];
    strupto(inputline, 0, ',', datestr); // for use later in insertAppointment()

    strupto(inputline, 11, ',', buffer);
    newAppointment -> startTime = strtol(buffer, NULL, 10);
    clearcstring(buffer, 25);

    strupto(inputline, 16, ',', buffer);
    newAppointment -> endTime = strtol(buffer, NULL, 10);
    clearcstring(buffer, 25);

    strupto(inputline, 21, ',', buffer);
    strcpy(newAppointment -> customer, buffer);
    clearcstring(buffer, 25);

    strupto(inputline, typestart + 1, '\0', buffer);
    strcpy(newAppointment -> appointmentType, buffer);

    clearcstring(buffer, 25);

    insertAppointment(newAppointment, datestr);
}

void strupto(char *input, short start, char term, char *output){
    for(int i = 0; i < start; i++)input++;
    while(*input != term){
        *output = *input;
        input++;
        output++;
    }
}

int fourthapos(char *input){
    short icount = 0;
    for(short i = 0; i < 100; i++){
        if(*input == ',') icount++;
        if(icount == 4) return i;
        input++;
    }
    return -1;
}

void clearcstring(char *ptr, short len){
    for(int i = 0; i < len; i++){
        *ptr = '\0';
        ptr++;
    }
}
*/
void removeAppointment(int index, int weekindex){
    //find app based on index
    //remove it by setting prev and next pointers to each other, then free()
    struct Appointment *app = currentWeek[weekindex].firstAppointment;

    for(int i = 0; i < index; i++){
        app = app -> nextAppointment;
    }

    if(app -> prevAppointment == NULL && app -> nextAppointment == NULL){
        currentWeek[weekindex].firstAppointment = NULL;
        free(app);
    }
    else if(app -> prevAppointment == NULL){
        currentWeek[weekindex].firstAppointment = app -> nextAppointment;
        app -> nextAppointment -> prevAppointment = NULL;
        free(app);
    }
    else if(app -> nextAppointment == NULL){
        app -> prevAppointment -> nextAppointment = NULL;
        free(app);
    }
    else{
        app -> prevAppointment -> nextAppointment = app -> nextAppointment;
        app -> nextAppointment -> prevAppointment = app -> prevAppointment;
        free(app);
    }
}

void removethisapp(struct Appointment *app, char *datestr){
    int index = 0, weekindex = 0;
    struct Appointment *findapp;

    short month = 0, day = 0, year = 0;
    interpretDate(datestr, &month, &day, &year);
    for(int i = 0; i < 7; i++){
        if(currentWeek[i].day == day && currentWeek[i].month == month && currentWeek[i].year == year) weekindex = i;
    }
    findapp = currentWeek[weekindex].firstAppointment;
    while(findapp != NULL){
        if(cmpapp(findapp, app) == 1){
            removeAppointment(index, weekindex);
            return;
        }
        index++;
        findapp = findapp -> nextAppointment;
    }
}

int cmpapp(struct Appointment *a1, struct Appointment *a2){
    int returnval = 1;

    if(a1 -> startTime != a2 -> startTime) returnval = 0;
    if(a1 -> endTime != a2 -> endTime) returnval = 0;
    if(strcmp(a1 -> customer, a2 -> customer) != 0) returnval = 0;
    if(strcmp(a1 -> appointmentType, a2 -> appointmentType) != 0) returnval = 0;
    if(strcmp(a1 -> employee, a2 -> employee) != 0) returnval = 0;
    return returnval;
}

struct Appointment *cloneapp(struct Appointment *app){
    struct Appointment *newapp;
    newapp = malloc(sizeof(*newapp));
    newapp ->id = app -> id;
    newapp -> startTime = app -> startTime;
    newapp -> endTime = app -> endTime;
    strcpy(newapp -> customer, app -> customer);
    strcpy(newapp -> appointmentType, app -> appointmentType);
    strcpy(newapp -> employee, app -> employee);

    return newapp;
}
/* //file stuff can be removed. left in just in case for testing purposes but you do need a properly formatted 'testfile.csv' which is a little complicated
void bufferFromFile(char *str){
    FILE *readfile;
    readfile = fopen("testfile.csv", "r");
    if(readfile == 0){
        printf("broke\n");
    }
    else{
        int c = fgetc(readfile);
        while(c != EOF){
            *str++ = c;
            c = fgetc(readfile);
        }
        fclose(readfile);
    }
}

void loadAppointments(char *in){
    char *inputstr = in;

    char inputline[100] = "";
    int index = 0;

    while(*inputstr != '\0'){
        inputline[index] = *inputstr;
        inputstr++;
        index++;

        if(*inputstr == '\n'){
            //create and insert appointment here before all that stuff
            createAppointment(inputline);

            while(index > 0){
                inputline[index] = 0;
                index--;
            }
            inputstr++;
        }
    }
    //printf("\nfinalline: %s", inputline);
}
*/
void unloadAppointments(int index){
    if(currentWeek[index].firstAppointment == NULL) return;
    struct Appointment *app = currentWeek[index].firstAppointment, *nextapp;
    currentWeek[index].firstAppointment = NULL;

    nextapp = app -> nextAppointment;
    while(nextapp != NULL){
        free(app);
        app = nextapp;
        nextapp = app -> nextAppointment;
    }
    free(app);
}
/*
void filetest(){
    char filestr[800];

    bufferFromFile(filestr);

    loadAppointments(filestr);
}
*/
