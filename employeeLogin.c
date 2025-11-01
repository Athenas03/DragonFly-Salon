#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <conio.h>
#include "curl/curl.h"
#include <curl/cJSON.h>


HANDLE wHnd;    //Handle to write to the console
HANDLE rHnd;    //Handle to read from the console

void setWin(char consoleTitle[]);
int curlPost(char *json);

int main() {
    char username[20], password[20], ch;//, json[100];
    //SetConsoleTitle("Employee Login");
    setWin("Employee Login");
    printf("\n\nUser Name: ");
    fgets(username, 20, stdin);
    //replacing the new line character taken in by fgets with a null character
    for(int x=0;x<20;x++)
    {
        if (username[x]=='\n')
            username[x]='\0';
    }
    printf("\nPassword: ");
    //variable which keeps track of which index of the password string is being edited
    int i=0;
    //character variable "ch" is assigned to the value of whatever character is entered by the user.
    //using getch() ensures that the character is stored in the variable without being shown on screen
    ch=getch();
    //do the next bit until enter is pressed
    while(ch!='\n'&&ch!='\r')
    {
        //if the user presses backspace
        if (ch=='\b')
        {
            //removes ability to delete characters before entering any
            if (i!=0)
            {
                //moves index to overwrite the backspaced character in the passwd array variable
                i--;
                //moves the cursor to the left one space, prints a white space to eliminate the backspaced character, then
                //moves the cursor to the left one space again(where the white space was printed)
                printf("\b \b");
            }
        }
        else
        {
        //assigns the current index of the password character array variable to the value stored in ch
        password[i]=ch;
        //shows an asterisk instead of the character that the user types in
        printf("*");

        //comment out the above line and un-comment the below line to show the password being typed
        //printf("%c", passwd[i]);

        //increments to the next index in the array
        i++;
        }
        ch=getch();
    }
    //once enter has been pressed, add the null character to indicate the end of the string
    password[i]='\0';



    cJSON *json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "username", username);
    cJSON_AddStringToObject(json, "password", password);
    char *json_str = cJSON_PrintUnformatted(json);

    curlPost(json_str);
    cJSON_free(json_str);
    cJSON_Delete(json);
    return 0;
}

int curlPost(char *json)
{

    CURL *curl; //CURL pointer variable, used like file handle
    CURLcode result; //CURLcode variable used to store return value from one of the curl API functions
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init(); //this function returns a curl handle. Stored in the CURL pointer variable
    if (curl == NULL)
    {
        fprintf(stderr, "HTTP request failed"); //outputting to the standard error that the HTTP request failed
        return -1;
    }
    struct curl_slist *headers = NULL;
    headers=curl_slist_append(headers, "Accept: application/json");
    headers=curl_slist_append(headers, "Content-Type: application/json");
    headers=curl_slist_append(headers, "charset: utf-8");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(curl, CURLOPT_CAINFO, "C:\\Curl\\curl-8.2.1_8-win64-mingw\\bin\\curl-ca-bundle.crt");
    curl_easy_setopt(curl, CURLOPT_CAPATH, "C:\\Curl\\curl-8.2.1_8-win64-mingw\\bin\\curl-ca-bundle.crt");
    curl_easy_setopt(curl, CURLOPT_URL, "https://samantha.cis294.hfcc.edu/api/login"); //This sets the URL for the HTTP request
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json);

    result = curl_easy_perform(curl); //carries out the request, returns curl code value. storing in result variable. Should return CURL_OK if the request worked
    if (result != CURLE_OK)
    {
        fprintf(stderr, "Error: %s\n", curl_easy_strerror(result));
        return -1;
    }

    curl_global_cleanup();
    curl_easy_cleanup(curl); //closes easy API session

    return 0;
}

//This function will be expanded to accept arguments which will dictate window size and placement
void setWin (char consoleTitle[]) {
    SetConsoleTitle(consoleTitle);
    printf("\n[]==[]=======================================================================================[]==[]");
    printf("\n[]  []                                                                                       []  []");
    printf("\n[]==[]=======================================================================================[]==[]");
    printf("\n  []                                                                                           []");
    printf("\n  [] [.....                                                     [.. [..                        []");
    printf("\n  [] [..   [..                                                [.    [..                        []");
    printf("\n  [] [..    [..[. [...   [..       [..      [..    [.. [..  [.[. [. [..[..   [..               []");
    printf("\n  [] [..    [.. [..    [..  [..  [..  [.. [..  [..  [..  [..  [..   [.. [.. [..                []");
    printf("\n  [] [..    [.. [..   [..   [.. [..   [..[..    [.. [..  [..  [..   [..   [...                 []");
    printf("\n  [] [..   [..  [..   [..   [..  [..  [.. [..  [..  [..  [..  [..   [..    [..                 []");
    printf("\n  [] [.....    [...     [.. [...     [..    [..    [...  [..  [..  [...   [..                  []");
    printf("\n  []                              [..                                   [..                    []");
    printf("\n  []   [.. ..             [..                           [.          [.. ..                     []");
    printf("\n  [] [..    [..           [..                         [.. [..     [..    [..                   []");
    printf("\n  []  [..         [..     [..   [..    [.. [..       [..           [..      [. [..     [..     []");
    printf("\n  []    [..     [..  [..  [.. [..  [..  [..  [..      [...           [..    [.  [..  [..  [..  []");
    printf("\n  []       [.. [..   [..  [..[..    [.. [..  [..     [..                [.. [.   [..[..   [..  []");
    printf("\n  [] [..    [..[..   [..  [.. [..  [..  [..  [..      [.. [..     [..    [..[.. [.. [..   [..  []");
    printf("\n  []   [.. ..    [.. [...[...   [..    [...  [..        [.          [.. ..  [..       [.. [... []");
    printf("\n  []                                                                        [..                []");
    printf("\n[]==[]=======================================================================================[]==[]");
    printf("\n[]  []                                                                                       []  []");
    printf("\n[]==[]=======================================================================================[]==[]");
}
