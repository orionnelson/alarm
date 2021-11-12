/*
 * new_alarm_mutex.c
 *
 * This is an enhancement to the alarm_mutex.c program, which created an "alarm thread" for each alarm command. This new
 * version adds new functionality on top of the old alarm_mutex.c program. It somewhat revamps the entire program to include 
 * 4 functions of start, change, cancel and view alarms. The program now runs by creating display threads that each take 2 
 * alarms and executes them.
 */
#include <pthread.h>
#include <time.h>
#include "errors.h"
#define TRUE 1
#define FALSE 0

/*
 * The "alarm" structure now contains multiple extra variables to account for all the added functions
 */
typedef struct alarm_tag
{
    struct alarm_tag *link; //linked list of type alarm_tag
    int seconds;
    time_t time; /* seconds from EPOCH */
    char message[128];
    unsigned int alarmID; // store alarmID that the user will input
    int assigned;         // this works like a boolean showing if the alarm has been assigned to a dispthread or not
    int change;           // for the change function, it tells you if the alarms requires change or not
    int terminate;        // for alarm delete
    int threadID;         //stores the display thread ID of the alarm
    int assTime;          // stores the time the alarm was assigned to the display thread
} alarm_t;

pthread_mutex_t alarm_mutex = PTHREAD_MUTEX_INITIALIZER;
alarm_t *alarm_list = NULL; //alarm list is null
int numAlarms = 0;          // keeps track of number of alarms in the system

/*
* This Function is run by the tertiary ringthread that basically runs the alarm for the display threads and manages changes in the alarm
* like terminate and change_alarm. The ringThread sleeps in 1 second iterations till the actual sleep_time and rings the alarm. The 1 second
* iteration sleep pattern heps it realize certain changes in the current alarm that it is trying to execute.
*/
void *alarm_display(void *arg)
{
    int sleep_time;
    int sleepCounter = 0; //used for running while loop
    sleep_time = ((alarm_t *)arg)->time - time(NULL);
    while (sleepCounter <= sleep_time)
    {
        sleep(1);                             //sleep each second and wake to check if anything changed
        if (((alarm_t *)arg)->change == TRUE) //if alarm has changed reset the change property and compute new sleep_time
        {
            ((alarm_t *)arg)->change == FALSE;
            sleep_time = ((alarm_t *)arg)->time - time(NULL);
            sleepCounter = 0;
        }

        if (((alarm_t *)arg)->terminate == TRUE) // if alarm has terminated or been cancelled then abort sleep and exit
        {
            printf("Alarm %u Canceled at %d : %d %s \n",
                   ((alarm_t *)arg)->alarmID, time(NULL), ((alarm_t *)arg)->seconds, ((alarm_t *)arg)->message);
            //free(arg);
            pthread_exit(NULL);
        }

        sleepCounter++;
    }
    /*
    * If sleepCounter reaches sleep_time then execute alarm and exit.
    */
    printf("The alarm with ID %d set at %d secs is ringing with message %s \n",
           ((alarm_t *)arg)->alarmID, ((alarm_t *)arg)->seconds, ((alarm_t *)arg)->message);
    ((alarm_t *)arg)->terminate = TRUE;
    //free((alarm_t *)arg);
    pthread_exit(NULL);
}

/*
 * This function assigns alarms to display threads which then create their own separate thread to sleep on the alarm. We start by 
 * declaring 3 alarms, a generic pointer then alarm1 and alarm2, the last 2 alarms are the ones that get a=assigned to the display thread.
 * The display loops through the alarm looking for unassigned alarms and once it finds one it assigns the alarm as alarm1 to it. Then it 
 * waits for another alarm and takes it as alarm2. After which the display thread terminates. It is important to note that the display thread
 * itself is not responsible for ringing the alarm, each display thread creates 2 ringthreads to handle the alarm.
 */
void *start_alarm(void *arg)
{
    alarm_t *alarm, *alarm1, *alarm2; //creating pointers to alarms
    int sleep_time;
    time_t now;
    int status;
    pthread_t ringThread;

    now = time(NULL); // now is the current time
    while (alarm2 == NULL) //Keep looping until alarm2 has been assigned, once 2 alarms have been assigned the display thread can stop.
    {
        status = pthread_mutex_lock(&alarm_mutex); //locking the mutex because we are going to change the alarm list
        if (status != 0)                           //error checking
            err_abort(status, "Lock mutex");
        alarm = alarm_list; //can change alarm list now since the mutex is locked

        /*
        * Loop through alarm_list to check if the current alarm is assigned or not, if the alarm is unassigned, take the alarm
        * and assign it to itself as alarm1. Then the second part of the if waits for a second unassigned alarm which is then assigned as 
        * alarm2.
        */
        while (alarm != NULL)
        {
            if (alarm->assigned != TRUE)
            { //goes through loop setting alarm1 and alarm2 for each display thread
                alarm->assigned = TRUE;
                if (alarm1 != NULL)
                {
                    printf("Display Thread %d Assigned Alarm %u at %d: %d %s \n",
                           pthread_self(), alarm->alarmID, time(NULL), alarm->time, alarm->message);
                    alarm->threadID = pthread_self();
                    alarm->assTime = time(NULL);
                    alarm2 = alarm;
                    pthread_create(&ringThread, NULL, alarm_display, (void *)alarm2); //creates ringthread which passes alarm2 as an argument
                    break;
                }
                else
                {
                    printf("Display Thread %d Assigned Alarm %u at %d: %d %s \n",
                           pthread_self(), alarm->alarmID, time(NULL), alarm->time, alarm->message);
                    alarm->threadID = pthread_self();
                    alarm->assTime = time(NULL);
                    alarm1 = alarm;
                    status = pthread_create(&ringThread, NULL, alarm_display, (void *)alarm1); //creates ringthread which passes alarm2 as an argument
                    if (status != 0) //error checking
                        err_abort(status, "Create alarm thread");
                }
            }
            alarm = alarm->link;
        }
        status = pthread_mutex_unlock(&alarm_mutex);
        if (status != 0)
            err_abort(status, "Unlock mutex");
    }
    printf("Display Alarm Thread %d Exiting at %d\n", pthread_self(), time(NULL));
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    int status; // for error checking purposes
    char line[128]; //input from user
    char task[30]; //storing the task from user input
    alarm_t *alarm, **last, *next, *cAlarm; //creating 4 pointers (double asterisk means a pointer of pointer)
    pthread_t thread; //this is the displaythread variable

    while (1)
    {
        printf("alarm> ");
        if (fgets(line, sizeof(line), stdin) == NULL)
            exit(0);
        if (strlen(line) <= 1)
            continue;                               
        alarm = (alarm_t *)malloc(sizeof(alarm_t)); 
                                                    
        if (alarm == NULL)                          
            errno_abort("Allocate alarm");
        // Initialize added variables to FALSE when creaing alarm
        alarm->assigned = FALSE;
        alarm->terminate = FALSE;
        alarm->change = FALSE;
        /*
         * Parse input line into task (%s), alarm ID (%u), alarm second (%d) and a 128char message (%128[^\n]),
         */
        if (sscanf(line, "%[a-zA-Z_](%u):%d%128[^\n]", task, &alarm->alarmID, // parses line for integer and 64 charecters
                   &alarm->seconds, alarm->message))
        {
            /*
            * In this section we check to see what kind of task the user instructed and depending on that task the program follows
            * a certain procedure. If the user has not inputted a valid command the else statement in this loop will print out 
            * bad command. 
            */

            if (strcmp(task, "Start_Alarm") == 0)
            {
                /*
                * This part of the code is run when the user inserts Start_Alarm, it processes the user input and create an alarm which
                * is then stored inside the alarm_list and the list is sorted according to alarm time. It also increments a variable 
                * called numAlarms which helps count the number alarms in the system and enables this function to create the right number
                * of display threads as you will see later.
                */
                numAlarms++;
                status = pthread_mutex_lock(&alarm_mutex); // lock mutex because we will be editing alarm_list
                if (status != 0)                           //error checking
                    err_abort(status, "Lock mutex");
                alarm->time = time(NULL) + alarm->seconds; 

                /*
                * Insert the new alarm into the list of alarms,
                * sorted by expiration time.
                */
                last = &alarm_list; 
                next = *last;    
                while (next != NULL)
                { 
                    if (next->time >= alarm->time)
                    {                      
                        alarm->link = next; 
                        *last = alarm;      
                        break;
                    }
                    last = &next->link; 
                    next = next->link;
                }
                /*
                * If we reached the end of the list, insert the new
                * alarm there. ("next" is NULL, and "last" points
                * to the link field of the last item, or to the
                * list header).
                */
                if (next == NULL)
                {
                    *last = alarm;
                    alarm->link = NULL;
                }
                printf("Alarm %u Inserted Into Alarm List at %d: %d %s \n",
                       alarm->alarmID, time(NULL), alarm->time, alarm->message); //print statement after successfully inserting alarm

#ifdef DEBUG //for the debug sessions use this while compiling, cc alarm_mutex.c -DDEBUG -D_POSIX_PTHREAD_SEMANTICS -lpthread
                printf("[list: ");
                for (next = alarm_list; next != NULL; next = next->link)
                    printf("%d(%d)[\"%s\"] ", next->time,
                           next->time - time(NULL), next->message);
                printf("]\n");
#endif
                status = pthread_mutex_unlock(&alarm_mutex); // unlock mutex when done modifying alarm_list
                if (status != 0)
                    err_abort(status, "Unlock mutex");

                if (numAlarms % 2 != 0 || numAlarms == 1) //if number of alarms is odd create a new display thread
                {
                    status = pthread_create(&thread, NULL, start_alarm, NULL);
                    if (status != 0)
                        err_abort(status, "create display thread");

                    printf("New Display Alarm Thread %d Created at %d: %d %s \n", thread, time(NULL), alarm->time, alarm->message);
                }
            }

            /*
            * This part executes if the user inputted Change_Alarm, this process goes through alarm list to find the corresponding
            * alarm that is requesting change. Once it finds that alarm it reassigns the alarm properties to match the change. It also
            * changes the value of alarm->change to TRUE, this allows the ringThread to know that a change has been issued and it re-
            * adjusts accordingly.
            */
            else if (strcmp(task, "Change_Alarm") == 0)
            {
                status = pthread_mutex_lock(&alarm_mutex); //locks mutex because modifying alarm_list
                if (status != 0) //error checking
                    err_abort(status, "Lock mutex");
                cAlarm = alarm_list;
                while (cAlarm != NULL)
                {
                    if (cAlarm->alarmID == alarm->alarmID)
                    {
                        cAlarm->time = time(NULL) + alarm->seconds;
                        cAlarm->seconds = alarm->seconds;
                        strcpy(cAlarm->message, alarm->message);
                        cAlarm->change = TRUE; //assigns change varible to true so that ringThread can utilize to wake up and readjust.
                        printf("Alarm(%u) Changed at %d: %d %s \n",
                               cAlarm->alarmID, time(NULL), cAlarm->time, cAlarm->message);
                        break;
                    }

                    cAlarm = cAlarm->link;
                }

                status = pthread_mutex_unlock(&alarm_mutex);
                if (status != 0) //error checking
                    err_abort(status, "unlock mutex");
            }
            /*
            * This process is executed when the user requests cancel alarm, similar to change alarm it iterates through the 
            * alarm_list to find the corresponding alarm requiring termination. It also sets the alarm->terminate to TRUE
            * this allows the ringThread to know that the alarm has been terminated and it exits.
            */
            else if (strcmp(task, "Cancel_Alarm") == 0)
            {
                status = pthread_mutex_lock(&alarm_mutex); // lock mutex becasue it is modifying alarm in alarm_list
                if (status != 0) //error checking
                    err_abort(status, "Lock mutex");
                cAlarm = alarm_list;
                while (cAlarm != NULL)
                {
                    if (cAlarm->alarmID == alarm->alarmID)
                    {
                        cAlarm->terminate = TRUE; //setting terminate to true so that ringThread stops sleeping on this alarm
                        break;
                    }
                    cAlarm = cAlarm->link;
                }
                status = pthread_mutex_unlock(&alarm_mutex);
                if (status != 0) //error checking
                    err_abort(status, "unlock mutex");
            }
            /*
            * This code executes when the user requests View_alarms, this process goes through the alarm list printing out
            * all the alarm information that is required by the user.
            */
            else if (strcmp(task, "View_Alarms") == 0)
            {
                cAlarm = alarm_list;
                int alarm_createdtime = 0;
                printf("View Alarms at %d:\n", time(NULL));
                while (cAlarm != NULL)
                {
                    alarm_createdtime = (cAlarm->time) - (cAlarm->seconds);
                    printf("Display Thread %d Assigned:  Alarm(%u): Created at %d Assigned at %d <%d %s>\n",
                           cAlarm->threadID, cAlarm->alarmID, alarm_createdtime, cAlarm->assTime, cAlarm->time, cAlarm->message);
                    cAlarm = cAlarm->link;
                }
            }
            /*
            * If task was none of the above then the terminal prints bad command.
            */
            else
            {
                fprintf(stderr, "Bad command\n"); 
                free(alarm);                      //free the allocated memory
            }
        }
        /*
        * if for some odd reason the sscanf does not run, the terminal prints bad command as well.
        */
        else
        {
            fprintf(stderr, "Bad command\n"); 
            free(alarm);                      //free the allocated memory
        }
    }
}