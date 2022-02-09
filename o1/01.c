#define __USE_XOPEN ;
#define _GNU_SOURCE ;

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

struct Alarm
{
   time_t end_time;
   int pid;
};

struct Alarm alarms[100];

/* simple helper-function to empty stdin https://stackoverflow.com/a/53059527*/
void empty_stdin(void)
{
   int c = getchar();

   while (c != '\n' && c != EOF)
      c = getchar();
}

void schedule()
{
   //Timezone crap 
   time_t timer = time(NULL);
   struct tm* currentLocalTimeStruct;
   struct tm* currentUnixTimeStruct;
   currentLocalTimeStruct = localtime(&timer);
   currentUnixTimeStruct = gmtime(&timer);
   int daylightFlag = currentLocalTimeStruct->tm_isdst;
   int timeZoneOffset = currentUnixTimeStruct->tm_gmtoff;
   int totalOffset = (daylight?0:3600)+timeZoneOffset;
   int currentUnixTime = mktime(currentLocalTimeStruct);
   int currentLocalTime = currentUnixTime + totalOffset;

   struct Alarm new_alarm;
   char timestring[19];
   struct tm alarmTimeStruct;

   //Load the user input into a time struct
   printf("Schedule alarm at which date and time? ");
   scanf("%19[^\n]", timestring);
   strptime(timestring, "%Y-%m-%d %H:%M:%S", &alarmTimeStruct);

   //calculate the timer length in seconds 
   alarmTimeStruct.tm_gmtoff = 0;
   alarmTimeStruct.tm_isdst = 0;
   time_t alarmUnixTime = mktime(&alarmTimeStruct)-totalOffset;
   long secondsleft = alarmUnixTime - time(NULL);

   printf("Setting an alarm in %ld seconds \n", secondsleft);
   new_alarm.end_time = alarmUnixTime;

   int pid = fork();
   if (pid == 0)
   {
      sleep(secondsleft);
      printf("ALARM!");
      printf("\a");
      exit(0);
   }
   else
   {
      // in parent
      new_alarm.pid = pid;
   }
}

void list()
{
}

void cancel()
{
}

int main()
{
   time_t timer;
   char buffer[26];

   printf("Welcome to the alarm clock!\n");
   while (1)
   {
      timer = time(NULL);
      strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", localtime(&timer));

      printf("It is currently %s \n Please enter \"s\" (schedule), \"l\" (list), \"c\" (cancel), \"x\" (exit)\n", buffer);
      char choice = getchar();
      empty_stdin();
      while (choice != 's' && choice != 'l' && choice != 'c' && choice != 'x')
      {
         printf("Please enter \"s\" (schedule), \"l\" (list), \"c\" (cancel), \"x\" (exit)\n");
         choice = getchar();
         empty_stdin();
      }

      switch (choice)
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
   }

   return 0;
}
