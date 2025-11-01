#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <cJSON.h>
#include <curl.h>
#include <windows.h>
#include <time.h>

struct memory {
        char *response;
        size_t size;
};

struct token{
    char *token;
    time_t tokenTime;
};

static size_t cb(void *data, size_t size, size_t nmemb, void *clientp)
    {
        size_t realsize = size * nmemb;
        struct memory *mem = (struct memory *)clientp;

        char *ptr = realloc(mem->response, mem->size + realsize + 1);
        if (ptr == NULL)
        {
            return 0;
        }
        mem->response = ptr;
        memcpy(&(mem->response[mem->size]), data, realsize);
        mem->size += realsize;
        mem->response[mem->size] = 0;

        return realsize;
    }

void displayMenu(cJSON *admin, cJSON *employee){
    if(cJSON_IsTrue(employee)){
        printf("\n(S)chedule an appointment");
        printf("\n(E)mployee schedule");
        printf("\n(F)eedback");
        printf("\n(C)ustomer information");
        if(cJSON_IsTrue(admin)){
            printf("\n(U)ser management");
        }
    }
    else{
        printf("\nYou do not work here. Please make your way to the other side of the counter and wait for assistance.");
    }
    printf("\n(L)ogout\n");
}
//This function will be expanded to accept arguments which will dictate window size and placement
void setWin (char consoleTitle[]) {

//! Make this more readable

    SetConsoleTitle(consoleTitle);
    system("cls");
    printf("\n[]==[]=======================================================================================[]==[]");
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
    printf("\n[]==[]=======================================================================================[]==[]");
}

cJSON *getCustomerList(cJSON *userList){
    cJSON *customerList=cJSON_CreateArray();
    cJSON *arrayItem=NULL;
    cJSON_ArrayForEach(arrayItem, userList){
        if (cJSON_IsFalse(cJSON_GetObjectItemCaseSensitive(arrayItem, "employee"))){
            cJSON_AddItemReferenceToArray(customerList, arrayItem);
        }
    }
    return customerList;
}


//!Fix this so it handles just the cJSON object stored in thisUser.token, not the whole structure
cJSON* curlGet(char *token, char *urlTag){
    struct memory chunk = {0};
    char *tokenString=token;
    char tokenHeader[strlen(tokenString)+22];
    char getURL[32];
    strcpy(getURL, "https://dev.cis294.hfcc.edu/api/");
    strcat(getURL, urlTag);
    strcpy(tokenHeader, "Authorization: Bearer ");
    strcat(tokenHeader, tokenString);
    CURL *curl;
    CURLcode result;
    curl_global_init(CURL_GLOBAL_ALL);
    curl=curl_easy_init();
    if(curl==NULL){
        fprintf(stderr, "HTTP request failed");
    }
    struct curl_slist *headers=NULL;
    headers=curl_slist_append(headers, "Accept: application/json");
    headers=curl_slist_append(headers, "Content-Type: application/json");
    headers=curl_slist_append(headers, "charset: utf-8");
    headers=curl_slist_append(headers, tokenHeader);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(curl, CURLOPT_CAINFO, "Curl\\curl-8.2.1_8-win64-mingw\\bin\\curl-ca-bundle.crt");
    curl_easy_setopt(curl, CURLOPT_CAPATH, "Curl\\curl-8.2.1_8-win64-mingw\\bin\\curl-ca-bundle.crt");
    curl_easy_setopt(curl, CURLOPT_URL, getURL);
    curl_easy_setopt(curl, CURLOPT_XOAUTH2_BEARER, &tokenString);
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
    result=curl_easy_perform(curl);

    cJSON *rawJSON=cJSON_Parse(chunk.response);
    return cJSON_GetObjectItem(rawJSON, "data");
}

char* trim(char *str){
    size_t len = strlen(str);   //gets the size in bytes of the length of the string
    memmove(str, str+1, len-2); //starting at the index 1 of str (str+1), copy the
    str[len-2]='\0';            //next number of bytes that a string 2 characters
    return str;                 //fewer than len (len-2)uses, and store that in
                                //the variable str
}

cJSON *getUserByField(char* field, char* value, cJSON *userList){
    for(int x=0;x<cJSON_GetArraySize(userList);x++){
        cJSON *arrayItem = cJSON_GetArrayItem(userList, x);
        cJSON *arrayUsername = cJSON_GetObjectItemCaseSensitive(arrayItem, field);
        char *arrayUserString = trim(cJSON_Print(arrayUsername));
        if(strcmp(value, arrayUserString)==0){
            return arrayItem;
        }
    }
}


struct token getToken(char *username, char *password){
    struct token newToken;
    cJSON *json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "username", username);
    cJSON_AddStringToObject(json, "password", password);
    //storing the json object as a string so it can be used for the curl post fields function
    char *json_str = cJSON_PrintUnformatted(json);

    struct memory chunk = {0};
    CURL *curl; //CURL pointer variable, used like file handle
    CURLcode result; //CURLcode variable used to store return value from one of the curl API functions
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init(); //this function returns a curl handle. Stored in the CURL pointer variable
    if (curl == NULL)
    {
        fprintf(stderr, "HTTP request failed"); //outputting to the standard error that the HTTP request failed
    }
    struct curl_slist *headers = NULL;
    headers=curl_slist_append(headers, "Accept: application/json");
    headers=curl_slist_append(headers, "Content-Type: application/json");
    headers=curl_slist_append(headers, "charset: utf-8");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    //!figure out how to store in settings
    curl_easy_setopt(curl, CURLOPT_CAINFO, "Curl\\curl-8.2.1_8-win64-mingw\\bin\\curl-ca-bundle.crt");
    curl_easy_setopt(curl, CURLOPT_CAPATH, "Curl\\curl-8.2.1_8-win64-mingw\\bin\\curl-ca-bundle.crt");
    curl_easy_setopt(curl, CURLOPT_URL, "https://dev.cis294.hfcc.edu/api/login"); //This sets the URL for the HTTP request
    //!

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_str);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
    result = curl_easy_perform(curl);

    cJSON *rawJSON = cJSON_Parse(chunk.response);
    cJSON *buffer=cJSON_GetObjectItemCaseSensitive(rawJSON, "data");
    newToken.token=cJSON_GetObjectItemCaseSensitive(buffer, "token");
    newToken.tokenTime=time(NULL);
    free(chunk.response);
    if (result != CURLE_OK)
    {
        fprintf(stderr, "Error: %s\n", curl_easy_strerror(result));
    }
    curl_global_cleanup();
    curl_easy_cleanup(curl);
    return newToken;
}

int checkTokenExp(time_t tokenTime){
    if(difftime(time(NULL), tokenTime)>13800){
        return 1;
    }
    else{
        return 0;
    }
}


