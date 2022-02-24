#define __USE_XOPEN ;
#define _GNU_SOURCE ;
#define MAX_ALARMS 5

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

struct Alarm alarms[MAX_ALARMS];
int active_alarms = 0;

void error_message(char *message)
{
   printf("\033[1;31m"); // Red
   printf("%s", message);
   printf("\033[0m"); // Black
}

void remove_alarm(int index)
{
   // Check if input is in range
   if (index < active_alarms)
   {
      kill(alarms[index].pid, 0);
      // Remove the element from the alarms array
      for (size_t j = index; j < active_alarms; j++)
      {
         alarms[j] = alarms[j + 1];
      }
      active_alarms--;
   }
}

void handle_ended_alarm()
{
   int pid = wait(NULL);
   for (size_t i = 0; i < active_alarms; i++)
   {
      if (alarms[i].pid == pid)
      {
         remove_alarm(i);
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
   if (active_alarms == MAX_ALARMS)
   {
      error_message("Too many alarms! Remove an alarm by typing \"c\" \n");
      return;
   }
   // Adjust according to timezones
   time_t timer = time(NULL);
   struct tm *current_local_time_struct;
   struct tm *current_unix_time_struct;
   current_local_time_struct = localtime(&timer);
   current_unix_time_struct = gmtime(&timer);
   int daylight_flag = current_local_time_struct->tm_isdst;
   int timezone_offset = current_unix_time_struct->tm_gmtoff;
   int total_offset = (daylight_flag ? 3600 : 0) + timezone_offset;
   int current_unix_time = mktime(current_local_time_struct);
   int current_local_time = current_unix_time + total_offset;

   struct Alarm new_alarm;
   printf("Schedule alarm at which date and time? (yyyy-mm-dd HH:MM:SS)\n> ");
   char time_s[19];
   struct tm alarm_time_struct;

   // Load the user input into a time struct
   printf("Schedule alarm at which date and time? ");
   scanf("%19[^\n]", time_s);
   empty_stdin();
   strptime(time_s, "%Y-%m-%d %H:%M:%S", &alarm_time_struct);

   // Calculate the timer length in seconds
   alarm_time_struct.tm_gmtoff = 0;
   alarm_time_struct.tm_isdst = 0;
   time_t alarm_unix_time = mktime(&alarm_time_struct) - total_offset;
   long seconds_left = alarm_unix_time - time(NULL);

   if (seconds_left < 0)
   {
      error_message("Scheduling failed! Make sure the time is in a valid format and not in the past.\n");
      return;
   }

   printf("Setting an alarm in %ld seconds \n", seconds_left);
   new_alarm.end_time = alarm_unix_time;

   int pid = fork();
   if (pid == 0)
   {
      sleep(seconds_left);
      // Notify user of alarm depending on their OS
#ifdef __unix__
      execlp("mpg123", "mpg123", "-q", "alarm.mp3", NULL);
#elif __APPLE__
      execlp("afplay", "afplay", "alarm.mp3", NULL);
#else
      printf("RING!");
#endif
   }

   else
   {
      // Stop zombies (We ignore exit codes, but oh well)
      signal(SIGCHLD, handle_ended_alarm);
      new_alarm.pid = pid;
      alarms[active_alarms] = new_alarm;
      active_alarms++;
   }
}

void list()
{
   if (!active_alarms)
   {
      error_message("No alarms to show!\n");
      return;
   }

   for (size_t i = 0; i < active_alarms; i++)
   {
      char time_s[30];
      strftime(time_s, 26, "%Y-%m-%d %H:%M:%S", localtime(&alarms[i].end_time));
      printf("Alarm %ld at %s\n", i + 1, time_s);
   }
}

void cancel()
{
   if (!active_alarms)
   {
      error_message("No alarms to cancel!\n");
      return;
   }
   int cancelled_alarm;
   printf("Cancel which alarm?\n> ");
   scanf("%d", &cancelled_alarm);
   empty_stdin();
   remove_alarm(cancelled_alarm - 1);
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
