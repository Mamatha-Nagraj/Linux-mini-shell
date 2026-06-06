#include<stdio.h>
#include"header.h"

// Inserts a new job at the beginning of job list
// Stores pid and command string
// Used when a process is stopped (Ctrl+Z)
void insert_first(int pid, char* input_string){
    struct job *new=malloc(sizeof(struct job));
    new->pid=pid;
    strcpy(new->command,input_string);
    new->next=head;
    head=new;
}

// Removes the most recent job from job list
// Used after fg when job finishes
void delete_first(){
    struct job *temp=head;
    head=head->next;
    free(temp);
}

// Prints all jobs in order (oldest → newest)
// Uses recursion to reverse the linked list order
// Displays job number and command
void print_jobs(struct job *t_head, int* count){
    if(t_head==NULL){
        return;
    }
    print_jobs(t_head->next,count);
    (*count)++;
    printf("[%d]      stopped        %s\n",*count,t_head->command);
}

int count_jobs(){
    int count = 0;
    struct job *temp = head;
    while(temp){
        count++;
        temp = temp->next;
    }
    return count;
}
