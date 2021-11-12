#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char argv[]){
    char task[30];
    int alarmID;
    int alarmTime;
    char message[128]; 

    char str[] = "Start_Alarm(2345): 50 Will meet you at Grandma’s house at 6 pm"

    sscanf(str, "%[a-zA-Z_](%d):%d %64[^\n]", task, &alarmID, &alarmTime, message)

    printf("%s task has been queued for alarm id %d which will ring at %d with message %128[^\n]", task, alarmID, alarmTime, message);
}