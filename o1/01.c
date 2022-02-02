#include<stdio.h>
#include<time.h>

int main() {
   // printf() displays the string inside quotation
   time_t t;   // not a primitive datatype
   time(&t);
   printf("Welcome to the alarm clock! It is currently %l %l Please enter \"s\" (schedule), \"l\" (list), \"c\" (cancel), \"x\" (exit)", t,  t);
   
   char choice;
   scanf("%c", &choice);
   printf("%c \n",choice);
   
   return 0;
}

