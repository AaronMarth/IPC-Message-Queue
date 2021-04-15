#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MINUTES 10

struct customer {
    unsigned int custNo;
    unsigned int arriveTime;
    unsigned int serviceTime;
};

#define CUST_SIZE sizeof(struct customer)

struct msg_struct {
    long mtype;
    struct customer data;
};

int main (int argc, char* argv[]) {
    char buff[120];
    int q_id;
    int minutes = 0;
    int queue_length = 0;
    int cust_no = 0;
    int departs = 0;
    int *at_head = NULL;
    int seconds = time(NULL);
    srand(seconds);
    struct customer customers;

    int next_arrival = rand() % 4 + 1 + minutes;
    printf("Next arrival at %d\n", next_arrival);

    if((q_id = msgget(IPC_PRIVATE, IPC_CREAT|0644)) == -1) {
        perror("msgget");
        exit(1);
    }
   
    while (minutes < MINUTES) {
        printf("\nMinute: %d\n", minutes);
        printf("Queue Length: %d\n", queue_length);
        if (next_arrival == minutes) {
            customers.custNo = cust_no;
            cust_no++;
            customers.arriveTime = minutes;
            customers.serviceTime = rand() % 4 + 1;
            printf("Enqueue new customer %d with service time %d\n", customers.custNo, customers.serviceTime);
            next_arrival = rand() % 4 + 1 + minutes;
            printf("Next arrival at %d\n", next_arrival);
            msgsnd(q_id, &customers, CUST_SIZE, 0);
            queue_length++;
        }

        if (queue_length > 0 && at_head == NULL) {
            msgrcv(q_id, (void *) &customers, CUST_SIZE, 0, 0);
            //dequeue customer here
            at_head = (int *) &customers;
            departs = minutes + customers.serviceTime ; // + customer service time
            printf("%d\n", departs);
        }
        
        if (departs == minutes && minutes != 0) {
            queue_length--;
            at_head = NULL;
        }
        minutes++;
    }
}