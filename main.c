#include "binary_tree.h"
void clearInputBuffer() {
    while (getchar() != '\n');
}
int main() {


    
    BPTreeNode *root = NULL;

    Passenger *waitlist[MAX_WAITLIST];
    int waitCount = 0;


    initializeCoaches();

    int choice;

    while (1) {
        printf("\n===== Railway Reservation System =====\n");
        printf("1. Book Ticket\n");
        printf("2. Cancel Ticket\n");
        printf("3. Display All Passengers\n");
        printf("4. Display Lower Berth Passengers\n");
        printf("5. Display Senior Citizens without LB\n");
        printf("6. Add to Waitlist\n");
        printf("7. Process Waitlist\n");
        printf("8. Save Data\n");
        printf("9. Load Data\n");
        printf("0. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        clearInputBuffer();

        if (choice == 0) break;

        if (choice == 1) {
            Passenger *p = (Passenger *)malloc(sizeof(Passenger));

            printf("Enter PNR: ");
            scanf("%d", &p->pnr);
            clearInputBuffer();

            printf("Enter Name: ");
            scanf("%s", p->name);

            printf("Enter Age: ");
            scanf("%d", &p->age);
            clearInputBuffer();

            int gender;
            printf("Enter Gender (0=Male, 1=Female): ");
            scanf("%d", &gender);
            clearInputBuffer();
            p->gender = (Gender)gender;

            printf("Enter Train No: ");
            scanf("%d", &p->train_no);
            clearInputBuffer();

            int berth;
            printf("Enter Berth (0=LB,1=MB,2=UB,3=SL,4=SU): ");
            scanf("%d", &berth);
            clearInputBuffer();
            p->berth = (Berth)berth;
            if (search(root, p->pnr) != NULL) {
            printf("PNR already exists!\n");
            free(p);
            continue;
}
            bookTicket(&root, p, coaches, MAX_COACHES);
        }

        else if (choice == 2) {
            int pnr;
            printf("Enter PNR to cancel: ");
            scanf("%d", &pnr);
            clearInputBuffer();

            cancelPassenger(root, pnr);
        }

        else if (choice == 3) {
            displayAll(root);
        }

        else if (choice == 4) {
            displayLowerBerth(root);
        }

        else if (choice == 5) {
            displaySeniorNoLower(root);
        }

        else if (choice == 6) {
            Passenger *p = (Passenger *)malloc(sizeof(Passenger));

            printf("Enter PNR: ");
            scanf("%d", &p->pnr);
            clearInputBuffer();

            printf("Enter Name: ");
            scanf("%s", p->name);

            printf("Enter Age: ");
            scanf("%d", &p->age);
            clearInputBuffer();

            int gender;
            printf("Enter Gender (0=Male, 1=Female): ");
            scanf("%d", &gender);
            clearInputBuffer();
            p->gender = (Gender)gender;

            printf("Enter Train No: ");
            scanf("%d", &p->train_no);
            clearInputBuffer();
            int berth;
            printf("Enter Berth (0=LB,1=MB,2=UB,3=SL,4=SU): ");
            scanf("%d", &berth);
            clearInputBuffer();
            p->berth = (Berth)berth;

            addToWaitlist(waitlist, &waitCount, p);
        }

        else if (choice == 7) {
            processWaitlist(&root, waitlist, &waitCount, coaches, MAX_COACHES);
        }

        else if (choice == 8) {
            saveAll(root);
        }

        else if (choice == 9) {
            root = loadFromFile();
        }

        else {
            printf("Invalid choice!\n");
        }
    }

    printf("Exiting...\n");
    return 0;
}