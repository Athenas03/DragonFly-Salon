#include <stdio.h>
#include <stdlib.h>
#include<string.h>
/*!
    This portion initially assigned to Zainab
*/

void viewCustomerInfo(){

    char customerName[30];

    printf("\nEnter Customers Name: ");
    fgets(customerName, 30, stdin);
    customerName[strlen(customerName) - 1] = '\0';

    printf("\nAddress: ");
    printf("\nEmail: ");
    printf("\nDiscount Amount: ");
    //printf("\nUser Name: ");
    //printf("\nDate Joined: ");
}

