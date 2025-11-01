#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <windows.h>
#include <conio.h>
#include <curl.h>
#include <cJSON.h>
struct userDetails{
cJSON*id;
cJSON*user;//!
cJSON*password;//!
cJSON*roles;//
cJSON*address;//
cJSON*dateJoined;
cJSON*email;//!
cJSON*firstName;//!
cJSON*lastName;//!
cJSON*discountCode;//
cJSON*reviews;
cJSON*isAdmin;
cJSON*isEmployee;
cJSON*token;
};

int addUser();
int editUser();
void removeUser();
struct memory {
        char *response;
        size_t size;
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

int addUser(){
    struct userDetails newUser;
    char userName[20];
    char email [30];
    char password[20];
    char firstName[15];
    char lastName[15];

    printf("Enter user: ");
    scanf("%s",&userName);
    printf("Enter email: ");
    scanf("%s",&email);
    printf("Enter password: ");
    scanf("%s",&password);
    printf("Enter FirstName: ");
    scanf("%s",&firstName);
    printf("Enter LastName: ");
    scanf("%s",&lastName);

    cJSON *JSON = cJSON_CreateObject();
    cJSON_AddStringToObject(JSON,"username",userName);
    cJSON_AddStringToObject(JSON,"password",password);
    cJSON_AddStringToObject(JSON,"firstName",firstName);
    cJSON_AddStringToObject(JSON,"lastName",lastName);
    cJSON_AddStringToObject(JSON,"email",email);

    char *jsonString = cJSON_PrintUnformatted(JSON);
    postUser(jsonString);
    getchar();
    printf("user added successfully.\n");
    return;
}

void postUser(char *json){
    struct memory chunk = {0};
    char *tokenString="eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJzdWIiOjEsIm5hbWUiOiJhZG1pbiIsImlhdCI6MTY5OTEzNDUxNywiZXhwIjoxNjk5MTQ4OTE3fQ.nLai_7h57v2SJ19a1usk5ZRHV5NrCCziHEYKdVGcwGo";
    char tokenHeader[strlen(tokenString)+22];
    strcpy(tokenHeader, "Authorization: Bearer ");
    strcat(tokenHeader, tokenString);
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
    headers=curl_slist_append(headers,tokenHeader);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    //!figure out how to store in settings
    curl_easy_setopt(curl, CURLOPT_CAINFO, "Curl\\curl-8.2.1_8-win64-mingw\\bin\\curl-ca-bundle.crt");
    curl_easy_setopt(curl, CURLOPT_CAPATH, "Curl\\curl-8.2.1_8-win64-mingw\\bin\\curl-ca-bundle.crt");
    curl_easy_setopt(curl, CURLOPT_URL, "https://dev.cis294.hfcc.edu/api/user/add"); //This sets the URL for the HTTP request
    //!

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json);
    result = curl_easy_perform(curl); //carries out the request, returns curl code value. storing in result variable. Should return CURL_OK if the request worked
    if (result==0)
    {
        printf("successfully added user");
    }
    else
    {
        printf("failed to add user");
    }



}


  int editUser() {
    struct userDetails editUserDetails;
    char email[30];
    char newUserName[20];
    char newPassword[20];
    char newFirstName[15];
    char newLastName[15];

    printf("Enter email of user to edit: ");
    scanf("%s", email);
    printf("Enter new username: ");
    scanf("%s", newUserName);
    printf("Enter new password: ");
    scanf("%s", newPassword);
    printf("Enter new first name: ");
    scanf("%s", newFirstName);
    printf("Enter new last name: ");
    scanf("%s", newLastName);

    cJSON *JSON = cJSON_CreateObject();
    cJSON_AddStringToObject(JSON, "email", email);
    cJSON_AddStringToObject(JSON, "newUsername", newUserName);
    cJSON_AddStringToObject(JSON, "newPassword", newPassword);
    cJSON_AddStringToObject(JSON, "newFirstName", newFirstName);
    cJSON_AddStringToObject(JSON, "newLastName", newLastName);

    char *jsonString = cJSON_PrintUnformatted(JSON);
    putUser(jsonString); // Assuming you'll create a similar function for PUT request
    getchar();
    printf("User edited successfully.\n");
    return;
}


void putUser(char *json) {
    struct memory chunk = {0};
    char *tokenString = "TOKEN HERE"; //TOKEN ADD HERE
    char tokenHeader[strlen(tokenString) + 22];
    strcpy(tokenHeader, "Authorization: Bearer ");
    strcat(tokenHeader, tokenString);

    CURL *curl;
    CURLcode result;
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (curl == NULL) {
        fprintf(stderr, "Initialization failed\n");
        return;
    }
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "charset: utf-8");
    headers = curl_slist_append(headers, tokenHeader);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);


    curl_easy_setopt(curl, CURLOPT_CAINFO, "PATH NEEDED");
    curl_easy_setopt(curl, CURLOPT_CAPATH, "PATH NEEDED");
    curl_easy_setopt(curl, CURLOPT_URL, "URL HERE");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");


    result = curl_easy_perform(curl);
    if (result == 0) {
        printf("User edited successfully.\n");
    } else {
        fprintf(stderr, "Failed to edit user: %s\n", curl_easy_strerror(result));
    }
}








