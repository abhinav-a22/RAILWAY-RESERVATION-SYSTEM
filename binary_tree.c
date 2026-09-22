#include "binary_tree.h"


Coach coaches[MAX_COACHES];


char *berthToString(Berth b)
{
    switch (b) {
        case LB:  return "LB";
        case MB:  return "MB";
        case UB:  return "UB";
        case SL:  return "SL";
        case SU:  return "SU";
        default:  return "UNK";
    }
}

Berth stringToBerth(char *str)
{
    if (strcmp(str, "LB") == 0) return LB;
    if (strcmp(str, "MB") == 0) return MB;
    if (strcmp(str, "UB") == 0) return UB;
    if (strcmp(str, "SL") == 0) return SL;
    if (strcmp(str, "SU") == 0) return SU;
    return LB; 
}

char *getBerthType(int seat_no)
{
    int mod = seat_no % 8;

    if (mod == 1 || mod == 4) return "LB";
    else if (mod == 2 || mod == 5) return "MB";
    else if (mod == 3 || mod == 6) return "UB";
    else if (mod == 7)             return "SL";
    else                           return "SU";
}


BPTreeNode *createNode(int isLeaf)
{
    BPTreeNode *node = (BPTreeNode *)malloc(sizeof(BPTreeNode));

    node->isLeaf  = isLeaf;
    node->numKeys = 0;
    node->next    = NULL;
    node->parent  = NULL;

    for (int i = 0; i < ORDER; i++)
        node->children[i] = NULL;

    for (int i = 0; i < MAX_KEYS; i++)
        node->data[i] = NULL;

    return node;
}

BPTreeNode *findLeaf(BPTreeNode *root, int key)
{
    if (root == NULL) return NULL;

    BPTreeNode *current = root;
    while (!current->isLeaf) {
        int i = 0;
        while (i < current->numKeys && key >= current->keys[i])
            i++;
        current = current->children[i];
    }
    return current;
}

Passenger *search(BPTreeNode *root, int key)
{
    if (root == NULL) return NULL;

    BPTreeNode *leaf = findLeaf(root, key);
    if (leaf == NULL) return NULL;

    for (int i = 0; i < leaf->numKeys; i++)
        if (leaf->keys[i] == key) return leaf->data[i];

    return NULL;
}

void insertIntoLeaf(BPTreeNode *leaf, int key, Passenger *data)
{
    int i = leaf->numKeys - 1;
    while (i >= 0 && leaf->keys[i] > key) {
        leaf->keys[i + 1] = leaf->keys[i];
        leaf->data[i + 1] = leaf->data[i];
        i--;
    }
    leaf->keys[i + 1] = key;
    leaf->data[i + 1] = data;
    leaf->numKeys++;
}

BPTreeNode *splitLeaf(BPTreeNode *leaf, int key, Passenger *data)
{
    int        tempKeys[ORDER];
    Passenger *tempData[ORDER];

    for (int i = 0; i < MAX_KEYS; i++) {
        tempKeys[i] = leaf->keys[i];
        tempData[i] = leaf->data[i];
    }

    int i = MAX_KEYS - 1;
    while (i >= 0 && tempKeys[i] > key) {
        tempKeys[i + 1] = tempKeys[i];
        tempData[i + 1] = tempData[i];
        i--;
    }
    tempKeys[i + 1] = key;
    tempData[i + 1] = data;

    int split = ORDER / 2;

    BPTreeNode *newLeaf = createNode(1);
    leaf->numKeys = 0;

    for (int i = 0; i < split; i++) {
        leaf->keys[i] = tempKeys[i];
        leaf->data[i] = tempData[i];
        leaf->numKeys++;
    }

    for (int i = split; i < ORDER; i++) {
        newLeaf->keys[i - split] = tempKeys[i];
        newLeaf->data[i - split] = tempData[i];
        newLeaf->numKeys++;
    }

    newLeaf->next   = leaf->next;
    leaf->next      = newLeaf;
    newLeaf->parent = leaf->parent;

    return newLeaf;
}


BPTreeNode *insertIntoParent(BPTreeNode *root, BPTreeNode *left,
                             int key, BPTreeNode *right)
{

    if (left->parent == NULL) {
        BPTreeNode *newRoot = createNode(0);

        newRoot->keys[0]     = key;
        newRoot->children[0] = left;
        newRoot->children[1] = right;
        newRoot->numKeys     = 1;
        left->parent         = newRoot;
        right->parent        = newRoot;

        return newRoot;
    }

    BPTreeNode *parent = left->parent;


    int i = 0;
    while (i <= parent->numKeys && parent->children[i] != left) i++;


    if (parent->numKeys < MAX_KEYS) {
        for (int j = parent->numKeys; j > i; j--) {
            parent->keys[j]         = parent->keys[j - 1];
            parent->children[j + 1] = parent->children[j];
        }
        parent->keys[i]         = key;
        parent->children[i + 1] = right;
        parent->numKeys++;
        right->parent = parent;

        return root;
    }

   
    int        tempKeys[ORDER];
    BPTreeNode *tempChildren[ORDER + 1];

    for (int j = 0; j < MAX_KEYS; j++)
        tempKeys[j] = parent->keys[j];
    for (int j = 0; j <= MAX_KEYS; j++)
        tempChildren[j] = parent->children[j];


    for (int j = MAX_KEYS - 1; j >= i; j--)
        tempKeys[j + 1] = tempKeys[j];
    for (int j = MAX_KEYS; j > i; j--)
        tempChildren[j + 1] = tempChildren[j];

    tempKeys[i]         = key;
    tempChildren[i + 1] = right;
    right->parent       = parent;


    int split  = ORDER / 2;
    int upKey  = tempKeys[split];

  
    parent->numKeys = 0;
    for (int j = 0; j < split; j++) {
        parent->keys[j]     = tempKeys[j];
        parent->children[j] = tempChildren[j];

        if (parent->children[j])
            parent->children[j]->parent = parent;
        parent->numKeys++;
    }
    parent->children[split] = tempChildren[split];
    if (parent->children[split])
        parent->children[split]->parent = parent;


    BPTreeNode *newNode = createNode(0);
    for (int j = split + 1; j < ORDER; j++) {
        newNode->keys[j - split - 1]     = tempKeys[j];
        newNode->children[j - split - 1] = tempChildren[j];
        if (newNode->children[j - split - 1])
            newNode->children[j - split - 1]->parent = newNode;
        newNode->numKeys++;
    }
    newNode->children[newNode->numKeys] = tempChildren[ORDER];
    if (newNode->children[newNode->numKeys])
        newNode->children[newNode->numKeys]->parent = newNode;

    newNode->parent = parent->parent;


    return insertIntoParent(root, parent, upKey, newNode);
}


BPTreeNode *splitInternal(BPTreeNode *node, BPTreeNode **root)
{
    int         tempKeys[ORDER];
    BPTreeNode *tempChildren[ORDER + 1];

    for (int i = 0; i < MAX_KEYS; i++)
        tempKeys[i] = node->keys[i];
    for (int i = 0; i <= MAX_KEYS; i++)
        tempChildren[i] = node->children[i];

    int split  = ORDER / 2;
    int upKey  = tempKeys[split];
    BPTreeNode *newNode = createNode(0);
    node->numKeys = 0;

    for (int i = 0; i < split; i++) {
        node->keys[i]     = tempKeys[i];
        node->children[i] = tempChildren[i];
      
        if (node->children[i])
            node->children[i]->parent = node;
        node->numKeys++;
    }
    node->children[split] = tempChildren[split];
    if (node->children[split])
        node->children[split]->parent = node;

    int j = 0;
    for (int i = split + 1; i < ORDER; i++) {
        newNode->keys[j]     = tempKeys[i];
        newNode->children[j] = tempChildren[i];
        if (newNode->children[j])
            newNode->children[j]->parent = newNode;
        newNode->numKeys++;
        j++;
    }
    newNode->children[j] = tempChildren[ORDER];
    if (newNode->children[j])
        newNode->children[j]->parent = newNode;

    newNode->parent = node->parent;

    BPTreeNode *newRoot = insertIntoParent(
        node->parent ? node->parent : node, node, upKey, newNode);

    if (newRoot != *root)
        *root = newRoot;

    return newRoot;
}

BPTreeNode *insert(BPTreeNode *root, int key, Passenger *data)
{

    if (root == NULL) {
        root          = createNode(1);
        root->keys[0] = key;
        root->data[0] = data;
        root->numKeys = 1;
        return root;
    }

    BPTreeNode *leaf = findLeaf(root, key);


    if (leaf->numKeys < MAX_KEYS) {
        insertIntoLeaf(leaf, key, data);
        return root;
    }


    BPTreeNode *newLeaf = splitLeaf(leaf, key, data);
    int         newKey  = newLeaf->keys[0];

    return insertIntoParent(root, leaf, newKey, newLeaf);
}

void rangeSearch(BPTreeNode *root, int start, int end)
{
    if (root == NULL) return;

    BPTreeNode *leaf = findLeaf(root, start);
    while (leaf != NULL) {
        for (int i = 0; i < leaf->numKeys; i++) {
            if (leaf->keys[i] >= start && leaf->keys[i] <= end) {
                Passenger *p = leaf->data[i];
                printf("PNR: %d, Name: %s, Train: %d\n",
                       p->pnr, p->name, p->train_no);
            } else if (leaf->keys[i] > end) {
                return;
            }
        }
        leaf = leaf->next;
    }
}



void displayAll(BPTreeNode *root)
{
    if (root == NULL) return;

    BPTreeNode *current = root;
    while (!current->isLeaf)
        current = current->children[0];

    while (current != NULL) {
        for (int i = 0; i < current->numKeys; i++) {
            Passenger *p = current->data[i];
            printf("PNR: %d | Name: %s | Train: %d | Coach: %d | Seat: %d\n",
                   p->pnr, p->name, p->train_no, p->coach_no, p->seat_no);
        }
        current = current->next;
    }
}

void displayLowerBerth(BPTreeNode *root)
{
    if (root == NULL) return;

    BPTreeNode *current = root;
    while (!current->isLeaf)
        current = current->children[0];

    while (current != NULL) {
        for (int i = 0; i < current->numKeys; i++) {
            Passenger *p = current->data[i];
            if (p->berth == LB)
                printf("PNR: %d | Name: %s | Train: %d | Coach: %d | Seat: %d\n",
                       p->pnr, p->name, p->train_no, p->coach_no, p->seat_no);
        }
        current = current->next;
    }
}

void displaySeniorNoLower(BPTreeNode *root)
{
    if (root == NULL) return;

    BPTreeNode *current = root;
    while (!current->isLeaf)
        current = current->children[0];

    while (current != NULL) {
        for (int i = 0; i < current->numKeys; i++) {
            Passenger *p = current->data[i];
            if (p->age >= 60 && p->berth != LB)
                printf("PNR: %d | Name: %s | Age: %d | Train: %d | Coach: %d | Seat: %d | Berth: %s\n",
                       p->pnr, p->name, p->age,
                       p->train_no, p->coach_no, p->seat_no,
                       berthToString(p->berth));
        }
        current = current->next;
    }
}


void initializeCoaches(void)
{
    for (int i = 0; i < MAX_COACHES; i++) {
        coaches[i].coach_no        = i + 1;
        coaches[i].train_no        = 0;   
        coaches[i].capacity        = 72;
        coaches[i].available_seats = 72;

         for (int j = 1; j <= 72; j++)
            coaches[i].seats[j] = 0;

        
        if (i < 3)
            strcpy(coaches[i].type, "AC");
        else
            strcpy(coaches[i].type, "SL");
    }
}

int findAvailableSeat(Coach *coach, Berth berth)
{
    if (coach->available_seats <= 0) return -1;

    for (int i = 1; i <= coach->capacity; i++) {

        if (coach->seats[i] == 1) continue;

        int   mod = i % 8;
        Berth b;

        if      (mod == 1 || mod == 4) b = LB;
        else if (mod == 2 || mod == 5) b = MB;
        else if (mod == 3 || mod == 6) b = UB;
        else if (mod == 7)             b = SL;
        else                           b = SU;

        if (b == berth) {
            coach->seats[i]=1;
            coach->available_seats--;
            return i;
        }
    }
    return -1;
}



void bookTicket(BPTreeNode **root, Passenger *p,
                Coach coaches[], int total_coaches)
{
    for (int i = 0; i < total_coaches; i++) {
        if (coaches[i].available_seats > 0) {
            int seat = findAvailableSeat(&coaches[i], p->berth);
            if (seat != -1) {
                p->coach_no = coaches[i].coach_no;
                p->seat_no  = seat;
                *root       = insert(*root, p->pnr, p);

                printf("Booking Successful!\n");
                printf("PNR: %d | Coach: %d | Seat: %d\n",
                       p->pnr, p->coach_no, p->seat_no);
                return;
            }
        }
    }
    printf("No seats available!\n");
}

void bookAcrossCoaches(BPTreeNode **root, Passenger *p,
                       Coach coaches[], int total_coaches)
{

    for (int i = 0; i < total_coaches; i++) {
        if (coaches[i].available_seats > 0) {
            int seat = findAvailableSeat(&coaches[i], p->berth);
            if (seat != -1) {
                p->coach_no = coaches[i].coach_no;
                p->seat_no  = seat;
                *root       = insert(*root, p->pnr, p);

                printf("Booking Successful (Preferred Berth)!\n");
                printf("PNR: %d | Coach: %d | Seat: %d\n",
                       p->pnr, p->coach_no, p->seat_no);
                return;
            }
        }
    }


    for (int i = 0; i < total_coaches; i++) {
        if (coaches[i].available_seats > 0) {
            for (int b = LB; b <= SU; b++) {
                int seat = findAvailableSeat(&coaches[i], (Berth)b);
                if (seat != -1) {
                    p->coach_no = coaches[i].coach_no;
                    p->seat_no  = seat;
                    p->berth    = (Berth)b;
                    *root       = insert(*root, p->pnr, p);

                    printf("Booking Successful (Alternate Berth)!\n");
                    printf("PNR: %d | Coach: %d | Seat: %d | Berth: %s\n", p->pnr, p->coach_no, p->seat_no, berthToString(p->berth));
                    return;
                }
            }
        }
    }
    printf("No seats available in any coach!\n");
}

void maxMatchingBooking(BPTreeNode **root, Passenger *p,
                        Coach coaches[], int total_coaches)
{
    int bestCoach = -1;
    int maxSeats  = -1;

    for (int i = 0; i < total_coaches; i++) {
        if (coaches[i].available_seats > maxSeats) {
            maxSeats  = coaches[i].available_seats;
            bestCoach = i;
        }
    }

    if (bestCoach == -1 || maxSeats == 0) {
        printf("No seats available!\n");
        return;
    }

    int seat = findAvailableSeat(&coaches[bestCoach], p->berth);

    if (seat == -1) {
        for (int b = LB; b <= SU; b++) {
            seat = findAvailableSeat(&coaches[bestCoach], (Berth)b);
            if (seat != -1) {
                p->berth = (Berth)b;
                break;
            }
        }
    }

    if (seat != -1) {
        p->coach_no = coaches[bestCoach].coach_no;
        p->seat_no  = seat;
        *root = insert(*root, p->pnr, p);

        printf("Booking Successful (Max Matching Coach)!\n");
        printf("PNR: %d | Coach: %d | Seat: %d | Berth: %s\n",
               p->pnr, p->coach_no, p->seat_no,
               berthToString(p->berth));
        return;
    }
    printf("No suitable seat found!\n");
}



void cancelPassenger(BPTreeNode *root, int pnr)
{
    Passenger *p = search(root, pnr);

    if (p == NULL) {
        printf("Passenger not found!\n");
        return;
    }

    printf("Cancelling Ticket...\n");
    printf("PNR: %d | Name: %s\n", p->pnr, p->name);
    if(p->coach_no > 0 && p->seat_no > 0){
        Coach *c = &coaches[p->coach_no - 1];

        if (c->seats[p->seat_no] == 1) {
            c->seats[p->seat_no] = 0;
            c->available_seats++;
        }
    }
    p->seat_no = -1; 

    printf("Ticket Cancelled Successfully!\n");
}

void processWaitlist(BPTreeNode **root, Passenger *waitlist[],int *waitCount, Coach coaches[], int total_coaches)
{
    int i = 0;
    while (i < *waitCount) {
        Passenger *p= waitlist[i];
        int booked = 0;
 
        for (int j = 0; j < total_coaches; j++) {
            if (coaches[j].available_seats > 0) {
                int seat = findAvailableSeat(&coaches[j], p->berth);
                if (seat != -1) {
                    p->coach_no = coaches[j].coach_no;
                    p->seat_no = seat;
                    *root = insert(*root, p->pnr, p);

                    printf("Waitlist Cleared for PNR: %d\n", p->pnr);

                    for (int k = i; k < *waitCount - 1; k++)
                        waitlist[k] = waitlist[k + 1];

                    (*waitCount)--;
                    booked = 1;
                    break;
                }
            }
        }
        if (!booked) i++;
    }
}

void addToWaitlist(Passenger *waitlist[], int *waitCount, Passenger *p)
{
    if (*waitCount >= MAX_WAITLIST) {
        printf("Waitlist is full!\n");
        return;
    }
    waitlist[*waitCount] = p;
    (*waitCount)++;

    printf("Added to Waitlist!\n");
    printf("PNR: %d | Name: %s | Position: %d\n",
           p->pnr, p->name, *waitCount);
}

void popWaitlist(Passenger *waitlist[], int *waitCount)
{
    if (*waitCount == 0) {
        printf("Waitlist is empty!\n");
        return;
    }

    Passenger *removed = waitlist[0];

    for (int i = 0; i < *waitCount - 1; i++)
        waitlist[i] = waitlist[i + 1];

    (*waitCount)--;

    printf("Removed from Waitlist:\n");
    printf("PNR: %d | Name: %s\n", removed->pnr, removed->name);
}


void saveToFile(BPTreeNode *root, FILE *fp)
{
    if (root == NULL) return;

    BPTreeNode *current = root;
    while (!current->isLeaf)
        current = current->children[0];

    while (current != NULL) {
        for (int i = 0; i < current->numKeys; i++) {
            Passenger *p = current->data[i];

            
            fprintf(fp, "%d %s %d %d %d %d %d %s\n", p->pnr,p->name,p->age,(int)p->gender, p->train_no,p->coach_no,p->seat_no,berthToString(p->berth));   
        }
        current = current->next;
    }
}

void saveAll(BPTreeNode *root)
{
    FILE *fp = fopen("passengers.txt", "w");

    if (fp == NULL) {
        printf("Error opening file!\n");
        return;
    }

    saveToFile(root, fp);
    fclose(fp);

    printf("Data saved successfully!\n");
}

BPTreeNode *loadFromFile(void)
{
    FILE *fp = fopen("passengers.txt", "r");

    if (fp == NULL) {
        printf("No existing data file found.\n");
        return NULL;
    }

    BPTreeNode *root = NULL;

    while (1) {
        Passenger *p = (Passenger *)malloc(sizeof(Passenger));

       
        char berthStr[4];
        int  gender;

        int result = fscanf(fp, "%d %49s %d %d %d %d %d %3s",&p->pnr,p->name, &p->age,&gender,&p->train_no,&p->coach_no,&p->seat_no,berthStr);

        if (result != 8) {
            free(p);
            break;
        }

        p->gender = (Gender)gender;
        p->berth = stringToBerth(berthStr);

        root = insert(root, p->pnr, p);
    }

    fclose(fp);
    printf("Data loaded successfully!\n");

    return root;
}
