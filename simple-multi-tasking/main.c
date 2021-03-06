/*
    Yevhenii Maliavka
    BAIN5
    Hochschule Merseburg
*/

#define TASK_HIGH_PRIO 4
#define TASK_STACK_SIZE 512
#define N_TASKS_AMOUNT 10
#define BUFFER 100
#define CLOCK_POS_X 0
#define CLOCK_POS_Y 20
#define STATS_POS_X 0
#define STATS_POS_Y 21
#define UI_BAR_LENGTH 60
#define UI_BAR_SYMBOL 223
#define UI_BAR_POS_Y 5
#define INPUT_POS_Y 22
#define INT_SIZE 16

#include "../common.h"

OS_STK tasksStack[N_TASKS_AMOUNT][TASK_STACK_SIZE];
OS_STK otherTasks[2][TASK_STACK_SIZE];
OS_STK initialTaskStack[TASK_STACK_SIZE];
int tasksData[N_TASKS_AMOUNT];
byte freePrio = TASK_HIGH_PRIO + 1;

void uiBarTask(void* data){
    int number = *((int*)data);
    char msg[BUFFER];
    sprintf(msg, "T%d:", number);
    print(0, UI_BAR_POS_Y + number, msg);
    int counter = 1;
    int step = 1;
    while(TRUE){
        print(5, UI_BAR_POS_Y + number, EMPTY_STRING);
        sprintf(msg, "%0*d",counter, 0);
        print(5, UI_BAR_POS_Y + number, msg);
        if(counter >= UI_BAR_LENGTH){
            step *= -1;
        }
        if(counter <= 0){
            step *= -1;
        }
        counter += step;
        tick(number + 2);
    }
}

void clockTask(void* data){
    char s[BUFFER];
    print(CLOCK_POS_X, CLOCK_POS_Y, (char*) data);
    int tab = strlen((char*)data);
    while(TRUE){
        PC_GetDateTime(s);
        print(CLOCK_POS_X + tab, CLOCK_POS_Y, EMPTY_STRING);
        print(CLOCK_POS_X + tab, CLOCK_POS_Y, s);
        wait(1);
    }
}

void statsTask(void* data){
    char s[BUFFER];
    print(STATS_POS_X, STATS_POS_Y, (char*) data);
    int tab = strlen((char*)data);
    while(TRUE){
        sprintf(s, "%d tasks, %d CPU, %d switches", OSTaskCtr, OSCPUUsage, OSCtxSwCtr);
        print(STATS_POS_X + tab, STATS_POS_Y, s);
        OSCtxSwCtr = 0;
        wait(1);
    }
}

void initialTask(void* data){
    print(26,  0, "uC/OS-II, The Real-Time Kernel");
    print(33,  1, "Simple multi-tasking");
    print(34,  2, "Yevhenii Maliavka");
    print(33,  3, "(press ESC to exit)");
    
    char keyBuffer[INT_SIZE];
    char msg[] = "Last key pressed: ";
    int tab;
    INT16S key;

    OSStatInit();

    int i = 0;
    char buf[6];
    int status;
    for (i = 0; i < N_TASKS_AMOUNT; i++) {
        tasksData[i] = i;                        
        createTask(uiBarTask, (void *)&tasksData[i], (void *)&tasksStack[i][TASK_STACK_SIZE - 1], freePrio + i);
    }
    createTask(clockTask, (void*)"Time:", (void*)&otherTasks[0][TASK_STACK_SIZE - 1], freePrio + i);
    createTask(statsTask, (void*)"Statistics:", (void*)&otherTasks[1][TASK_STACK_SIZE - 1], freePrio + 1 + i);

    print(0, INPUT_POS_Y, msg);
    while(1){
        if(PC_GetKey(&key)){
            if(key == KEY_ESC){
                exit(0);
            }
            tab = strlen(msg);
            sprintf(keyBuffer, "%c", key);
            print(tab, INPUT_POS_Y, EMPTY_STRING);
            print(tab, INPUT_POS_Y, keyBuffer);
        }
        tick(1);
    }
}

int main(void){
    setbuf(stdout, NULL);
    PC_DispClrScr(DISP_FGND_WHITE + DISP_BGND_GRAY);
    OSInit();
    createTask(initialTask, (void*)0, &initialTaskStack[TASK_STACK_SIZE - 1], freePrio++);
    OSStart();
    return 0;
}