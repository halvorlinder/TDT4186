#define __USE_XOPEN ;
#define _GNU_SOURCE ;

#include <signal.h>
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
int nextAlarmIndex = 0;

/* simple helper-function to empty stdin https://stackoverflow.com/a/53059527*/
void empty_stdin(void)
{
   int c = getchar();

   while (c != '\n' && c != EOF)
      c = getchar();
}

void schedule()
{
   struct Alarm new_alarm;
   printf("Schedule alarm at which date and time? (yyyy-mm-dd HH:MM:SS)\n> ");
   char timestring[19];
   struct tm timestruct;
   scanf("%19[^\n]", timestring);
   empty_stdin();
   strptime(timestring, "%Y-%m-%d %H:%M:%S", &timestruct);
   time_t unixtimestamp = mktime(&timestruct);
   printf("%ld - %ld + %ld \n", unixtimestamp, time(NULL), timestruct.tm_gmtoff);
   printf("Without tm_gmtoff: %ld \n", unixtimestamp - time(NULL));
   long secondsleft = unixtimestamp - time(NULL) + timestruct.tm_gmtoff;
   printf("Setting an alarm in %ld seconds \n", secondsleft);
   new_alarm.end_time = unixtimestamp;
   int pid = fork();
   if (pid == 0)
   {
      char *programName = "mpg123";
      char *arg1 = "alarm.mp3";
 
      execlp(programName, programName, arg1, NULL);
      sleep(secondsleft);
      
      exit(0);
   }
   else
   {
      // in parent
      new_alarm.pid = pid;
      alarms[nextAlarmIndex] = new_alarm;
      nextAlarmIndex++;
   }
}

void list()
{
   for (size_t i = 0; i < nextAlarmIndex; i++)
   {
      char timestring[30];
      strftime(timestring, 26, "%Y-%m-%d %H:%M:%S", localtime(&alarms[i].end_time));
      printf("Alarm %ld at %s\n", i+1, timestring);
   }
}

void cancel()
{
   int cancelledAlarm;
   printf("Cancel which alarm?\n> ");
   scanf("%d", &cancelledAlarm);
   empty_stdin();
   // Checking if input is in range
   if (cancelledAlarm < nextAlarmIndex)
   {
      kill(alarms[cancelledAlarm].pid, 0);
      // Removing the element from the alarms array
      for (int i = cancelledAlarm - 1; i < nextAlarmIndex - 1; i++)
      {
         alarms[i] = alarms[i + 1]; // assign arr[i+1] to arr[i]
      }
      nextAlarmIndex--;
   }
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

      printf("It is currently %s \nPlease enter \"s\" (schedule), \"l\" (list), \"c\" (cancel), \"x\" (exit)\n> ", buffer);
      char choice = getchar();
      empty_stdin();
      while (choice != 's' && choice != 'l' && choice != 'c' && choice != 'x')
      {
         printf("Please enter \"s\" (schedule), \"l\" (list), \"c\" (cancel), \"x\" (exit)\n> ");
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
         printf("Goodbye!\n");
         exit(0);
         break;
      }
   }

   return 0;
}
