#define __USE_XOPEN ;
#define _GNU_SOURCE ;

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

struct Alarm
{
   time_t end_time;
   int pid;
};

/* Enum members are constant expressions, thus anonymous enums will work as constants */
enum
{
   MAX_ALARMS = 5,
};

struct Alarm alarms[MAX_ALARMS];
int amountOfAlarms = 0;

void errorMessage(char *message)
{
   printf("\033[1;31m"); // Red
   printf("%s", message);
   printf("\033[0m"); // Black
}

void removeAlarmFromArray(int index)
{
   // Check if input is in range
   if (index < amountOfAlarms)
   {
      kill(alarms[index].pid, 0);
      // Remove the element from the alarms array
      for (size_t j = index; j < amountOfAlarms; j++)
      {
         alarms[j] = alarms[j + 1];
      }
      amountOfAlarms--;
   }
}

void handleEndedAlarm(int signal)
{
   int pid = wait(NULL);
   for (size_t i = 0; i < amountOfAlarms; i++)
   {
      if (alarms[i].pid == pid)
      {
         removeAlarmFromArray(i);
      }
   }
}

/* Simple helper-function to empty stdin: https://stackoverflow.com/a/53059527 */
void empty_stdin(void)
{
   int c = getchar();

   while (c != '\n' && c != EOF)
      c = getchar();
}

void schedule()
{
   if (amountOfAlarms == MAX_ALARMS)
   {
      errorMessage("Too many alarms! Remove an alarm by typing \"c\" \n");
      return;
   }
   // Adjust according to timezones
   time_t timer = time(NULL);
   struct tm *currentLocalTimeStruct;
   struct tm *currentUnixTimeStruct;
   currentLocalTimeStruct = localtime(&timer);
   currentUnixTimeStruct = gmtime(&timer);
   int daylightFlag = currentLocalTimeStruct->tm_isdst;
   int timeZoneOffset = currentUnixTimeStruct->tm_gmtoff;
   int totalOffset = (daylightFlag ? 3600 : 0) + timeZoneOffset;
   int currentUnixTime = mktime(currentLocalTimeStruct);
   int currentLocalTime = currentUnixTime + totalOffset;

   struct Alarm new_alarm;
   printf("Schedule alarm at which date and time? (yyyy-mm-dd HH:MM:SS)\n> ");
   char timestring[19];
   struct tm alarmTimeStruct;

   // Load the user input into a time struct
   printf("Schedule alarm at which date and time? ");
   scanf("%19[^\n]", timestring);
   empty_stdin();
   strptime(timestring, "%Y-%m-%d %H:%M:%S", &alarmTimeStruct);

   // Calculate the timer length in seconds
   alarmTimeStruct.tm_gmtoff = 0;
   alarmTimeStruct.tm_isdst = 0;
   time_t alarmUnixTime = mktime(&alarmTimeStruct) - totalOffset;
   long secondsleft = alarmUnixTime - time(NULL);

   if (secondsleft < 0)
   {
      errorMessage("You cannot go back in time!\n");
      return;
   }

   printf("Setting an alarm in %ld seconds \n", secondsleft);
   new_alarm.end_time = alarmUnixTime;

   int pid = fork();
   if (pid == 0)
   {
      sleep(secondsleft);
      execlp("mpg123", "mpg123", "-q", "alarm.mp3", NULL);
   }
   else
   {
      // Stop zombies (We ignore exit codes, but oh well)
      signal(SIGCHLD, handleEndedAlarm);
      new_alarm.pid = pid;
      alarms[amountOfAlarms] = new_alarm;
      amountOfAlarms++;
   }
}

void list()
{
   if (!amountOfAlarms)
   {
      errorMessage("No alarms to show!\n");
      return;
   }

   for (size_t i = 0; i < amountOfAlarms; i++)
   {
      char timestring[30];
      strftime(timestring, 26, "%Y-%m-%d %H:%M:%S", localtime(&alarms[i].end_time));
      printf("Alarm %ld at %s\n", i + 1, timestring);
   }
}

void cancel()
{
   if (!amountOfAlarms)
   {
      errorMessage("No alarms to cancel!\n");
      return;
   }
   int cancelledAlarm;
   printf("Cancel which alarm?\n> ");
   scanf("%d", &cancelledAlarm);
   empty_stdin();
   removeAlarmFromArray(cancelledAlarm - 1);
}

int main()
{
   time_t timer;
   char buffer[26];

   printf("\033[1;33m"); // Yellow
   printf("Welcome to the alarm clock!\n");
   printf("\033[0m"); // Black
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
         _exit(0);
         break;
      }
   }

   return 0;
}
