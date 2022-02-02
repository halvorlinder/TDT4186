#include<stdlib.h>
#include<stdio.h>
#include<time.h>

/* simple helper-function to empty stdin https://stackoverflow.com/a/53059527*/
void empty_stdin (void) 
{
    int c = getchar();

    while (c != '\n' && c != EOF)
        c = getchar();
}

void schedule() {

}

void list() {

}

void cancel() {

}

int main() {
   time_t timer;
   char buffer[26];
   struct tm* tm_info;

   timer = time(NULL);
   tm_info = localtime(&timer);

   strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
   printf("Welcome to the alarm clock! It is currently %s Please enter \"s\" (schedule), \"l\" (list), \"c\" (cancel), \"x\" (exit)\n", buffer);
   
   char choice = getchar();
   empty_stdin();
   while (choice != 's' && choice != 'l' && choice != 'c' && choice != 'x')
   {
      printf("Please enter \"s\" (schedule), \"l\" (list), \"c\" (cancel), \"x\" (exit)\n" );
      choice = getchar();
      empty_stdin();
   }
   
   switch(choice)
   {
      case 's':
         schedule();
         break;
      case 'l':
         list();
         break;
      case 'c':
         cancel();
         break;
      case 'x':
         exit(0);
         break;
   }
   
   return 0;
}

