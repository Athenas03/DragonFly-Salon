#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <cJSON.h>
#include <curl.h>
#include <windows.h>
#include <time.h>


struct token{
    char *token;
    time_t tokenTime;
};

cJSON *curlPostToken(char *token, char *json, char *urlTag);
cJSON* curlGet(char *token, char *urlTag);
cJSON *GetArrayItemByIntegerValue(char* field, int value, cJSON *sourceArray);
cJSON *getArray (cJSON *sourceArray, char* field, int value);
cJSON *getFalseEntries(cJSON *sourceArray, char* field );
void printReviews(cJSON *reviewList);
void printContactForm(cJSON *contactFormList);



void printReviewsByRating(cJSON *reviewList, int rating){
char choice;
do {
        printf("Do you want to filter through reviews by rating? \n(n)o will take you straight to messages (y/n): ");
        scanf(" %c", &choice);

        if (choice == 'n' || choice == 'N') {
            break;
        } else if (choice != 'y' && choice != 'Y') {
            printf("Invalid choice. Please enter 'y' or 'n'.\n");
            continue;
        }

        printf("Enter the rating to filter through reviews: ");
        scanf("%d", &rating);

        if (rating == -1) {
            break;
        }

cJSON *reviewOne;
cJSON *customer;
int found=0;
for (int x=0; x<cJSON_GetArraySize(reviewList); x++ ){
    reviewOne=cJSON_GetArrayItem(reviewList, x);
    customer=cJSON_GetObjectItemCaseSensitive(reviewOne,"customer");

    if (cJSON_GetObjectItemCaseSensitive(reviewOne, "rating")-> valueint== rating){
    found = 1;
    printf("\n id: %d\n Customer ID: %d\n Rating: %d\n Comments: %s\n Date: %s \n",
           cJSON_GetObjectItemCaseSensitive(reviewOne, "id") ->valueint,
           cJSON_GetObjectItemCaseSensitive(customer,"id") ->valueint,
           cJSON_GetObjectItemCaseSensitive(reviewOne,"rating") ->valueint,
           cJSON_GetObjectItemCaseSensitive(reviewOne,"comments") ->valuestring,
           cJSON_GetObjectItemCaseSensitive(reviewOne,"date")->valuestring);

    }
}
if (!found){
    printf("\n No reviews found for rating %d, enter different rating",rating);
    }

    }while(1);

}




void printReviews(cJSON  *reviewList){


cJSON *reviewOne;
cJSON *customer;
for (int x=0; x<cJSON_GetArraySize(reviewList); x++ ){
    reviewOne=cJSON_GetArrayItem(reviewList, x);
    customer=cJSON_GetObjectItemCaseSensitive(reviewOne,"customer");

    printf("\n id: %d\n Customer ID: %d\n Rating: %d\n Comments: %s\n Date: %s \n",
           cJSON_GetObjectItemCaseSensitive(reviewOne, "id") ->valueint,
           cJSON_GetObjectItemCaseSensitive(customer,"id") ->valueint,
           cJSON_GetObjectItemCaseSensitive(reviewOne,"rating") ->valueint,
           cJSON_GetObjectItemCaseSensitive(reviewOne,"comments") ->valuestring,
           cJSON_GetObjectItemCaseSensitive(reviewOne,"date")->valuestring);

    }

}



void unReadMessages(cJSON *contactFormList,int *shouldContinue) {
    cJSON *contactOne;
    cJSON *contact;
    char choice;

    do {
        printf("\nDo you want to view unread comments? (y/n): ");
        scanf(" %c", &choice);

        for (int x = 0; x < cJSON_GetArraySize(contactFormList); x++) {
            contact = cJSON_GetArrayItem(contactFormList, x);
            cJSON *isReadField = cJSON_GetObjectItemCaseSensitive(contact, "isread");

            if (cJSON_IsFalse(isReadField)) {
                updateMessage(cJSON_GetObjectItemCaseSensitive(contact, "id")->valueint);
                printf("\n id: %d\n first Name: %s\n last name: %s\n phone number: %s\n email: %s\n message: %s\n date: %s\n isread: false \n",
                       cJSON_GetObjectItemCaseSensitive(contact, "id")->valueint,
                       cJSON_GetObjectItemCaseSensitive(contact, "firstName")->valuestring,
                       cJSON_GetObjectItemCaseSensitive(contact, "lastName")->valuestring,
                       cJSON_GetObjectItemCaseSensitive(contact, "phoneNumber")->valuestring,
                       cJSON_GetObjectItemCaseSensitive(contact, "email")->valuestring,
                       cJSON_GetObjectItemCaseSensitive(contact, "message")->valuestring,
                       cJSON_GetObjectItemCaseSensitive(contact, "date")->valuestring);
            }
        }

        printf("\nDo you want to return to the main menu? (y): ");
        scanf(" %c", &choice);

        if (choice == 'y' || choice == 'Y') {
            printf("Press enter to return to the main menu\n");
            *shouldContinue = 0;
        }

    } while (*shouldContinue);
}

void updateMessage(int id){

    cJSON *istrue=cJSON_CreateBool("true");
    cJSON *read=cJSON_CreateObject();
    cJSON_AddBoolToObject(read,"isread",istrue);

    char *jsonString=cJSON_PrintUnformatted(read);
    char updateURL [30];
    char idString[5];
    strcpy(updateURL,"contactform/");
    sprintf(idString,"%d",id);

    strcat(updateURL,idString);
    strcat(updateURL,"/update");
    struct token *thisToken=getThisToken();
    curlPostToken(thisToken->token,jsonString,updateURL);




}



void printContactForm(cJSON *contactFormList) {
    cJSON *contactOne;
    cJSON *contact;

    for (int x = 0; x < cJSON_GetArraySize(contactFormList); x++) {
        contactOne = cJSON_GetArrayItem(contactFormList, x);
        contact = cJSON_GetObjectItemCaseSensitive(contactOne, "contact");

        printf("\n id: %d\n first Name: %s\n last name: %s\n phone number: %s\n email: %s\n message: %s\n date: %s\n isread: %s \n ",
               cJSON_GetObjectItemCaseSensitive(contactOne, "id")->valueint,
               cJSON_GetObjectItemCaseSensitive(contactOne, "firstName")->valuestring,
               cJSON_GetObjectItemCaseSensitive(contactOne, "lastName")->valuestring,
               cJSON_GetObjectItemCaseSensitive(contactOne, "phoneNumber")->valuestring,
               cJSON_GetObjectItemCaseSensitive(contactOne, "email")->valuestring,
               cJSON_GetObjectItemCaseSensitive(contactOne, "message")->valuestring,
               cJSON_GetObjectItemCaseSensitive(contactOne, "date")->valuestring,
               cJSON_Print(cJSON_GetObjectItemCaseSensitive(contactOne,"isread")));
    }
}




void viewFeedback(char *token){

  int shouldContinue = 1;

    do {

        cJSON *reviewList = curlGet(token, "reviews");
        printReviews(reviewList);

        printf("\n\n\n");
        printReviewsByRating(reviewList, 5);

        cJSON *contactFormList = curlGet(token, "contactform");

        printContactForm(contactFormList);
        unReadMessages(contactFormList, &shouldContinue);

    } while (shouldContinue);

}




cJSON *GetArrayItemByIntegerValue(char* field, int value, cJSON *sourceArray){
    for(int x=0;x<cJSON_GetArraySize(sourceArray);x++){
        cJSON *arrayItem = cJSON_GetArrayItem(sourceArray, x);
        cJSON *itemObject = cJSON_GetObjectItemCaseSensitive(arrayItem, field);
        printf("\n integer value = %d ", itemObject->valueint);
        if(value == itemObject->valueint){

            return arrayItem;
        }
    }
}

cJSON *getArray (cJSON *sourceArray, char* field, int value){
    cJSON *resultArray= cJSON_CreateArray();

    for( int x=0;x<cJSON_GetArraySize(sourceArray);x++){
        cJSON *arrayItem = cJSON_GetArrayItem(sourceArray, x);
        cJSON *itemObject = cJSON_GetObjectItemCaseSensitive(arrayItem, field);
        printf("\n integer value = %d ", itemObject->valueint);
        if(value == itemObject->valueint){
            cJSON_AddItemReferenceToArray(resultArray, arrayItem);
        }
    }
    return resultArray;
}


cJSON *getFalseEntries(cJSON *sourceArray, char* field ){
    cJSON *falseEntries=cJSON_CreateArray();
    cJSON *arrayItem=NULL;
    cJSON_ArrayForEach(arrayItem, sourceArray){

        if (cJSON_IsFalse(cJSON_GetObjectItemCaseSensitive(arrayItem, field))){
            cJSON_AddItemReferenceToArray(falseEntries, arrayItem);
        }
    }

    return falseEntries;
}












