#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct Volunteer {
    char name[50];
    char skill[50];
    int available;
    struct Volunteer* next;
} Volunteer;


typedef struct Task {
    char name[50];
    char requiredSkill[50];
    int urgency; 
    struct Task* next;
} Task;


Volunteer* volunteerHead = NULL;
Task* taskHead = NULL;


void addVolunteer() {
    Volunteer* v = (Volunteer*)malloc(sizeof(Volunteer));
    printf("Enter Volunteer Name: ");
    scanf(" %[^\n]", v->name);
    printf("Enter Skill: ");
    scanf(" %[^\n]", v->skill);
    printf("Available? (1 = yes, 0 = no): ");
    scanf("%d", &v->available);
    v->next = volunteerHead;
    volunteerHead = v;
    printf("Volunteer Added!\n");
}


void addTask() {
    Task* t = (Task*)malloc(sizeof(Task));
    printf("Enter Task Name: ");
    scanf(" %[^\n]", t->name);
    printf("Enter Required Skill: ");
    scanf(" %[^\n]", t->requiredSkill);
    printf("Enter Urgency (1-5): ");
    scanf("%d", &t->urgency);
    t->next = taskHead;
    taskHead = t;
    printf("Task Added!\n");
}


void displayVolunteers() {
    Volunteer* temp = volunteerHead;
    printf("\n--- Volunteers ---\n");
    while(temp) {
        printf("Name: %s | Skill: %s | Available: %s\n", temp->name, temp->skill, temp->available ? "Yes" : "No");
        temp = temp->next;
    }
}


void displayTasks() {
    Task* temp = taskHead;
    printf("\n--- Tasks ---\n");
    while(temp) {
        printf("Task: %s | Skill: %s | Urgency: %d\n", temp->name, temp->requiredSkill, temp->urgency);
        temp = temp->next;
    }
}


void matchVolunteers() {
    Task* t = taskHead;
    printf("\n--- Volunteer Matches ---\n");
    while(t) {
        Volunteer* v = volunteerHead;
        int found = 0;
        printf("Task: %s (Skill: %s, Urgency: %d)\n", t->name, t->requiredSkill, t->urgency);
        while(v) {
            if(v->available && strcmp(v->skill, t->requiredSkill) == 0) {
                printf("-> %s is suitable for this task.\n", v->name);
                found = 1;
            }
            v = v->next;
        }
        if(!found) printf("No volunteer available for this task.\n");
        t = t->next;
        printf("\n");
    }
}


int main() {
    int choice;
    do {
        printf("\n=== Volunteer Skill Matcher ===\n");
        printf("1. Add Volunteer\n2. Add Task\n3. Display Volunteers\n4. Display Tasks\n5. Match Volunteers\n6. Exit\nEnter choice: ");
        scanf("%d", &choice);
        switch(choice) {
            case 1: addVolunteer(); break;
            case 2: addTask(); break;
            case 3: displayVolunteers(); break;
            case 4: displayTasks(); break;
            case 5: matchVolunteers(); break;
            case 6: printf("Exiting...\n"); break;
            default: printf("Invalid choice!\n");
        }
    } while(choice != 6);

    return 0;
}
