#ifndef BINARY_TREE_H
#define BINARY_TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAX_COACHES  10
#define ORDER        4        
#define MAX_KEYS     (ORDER - 1)
#define MAX_WAITLIST 50


typedef enum {
    MALE,
    FEMALE
} Gender;

typedef enum {
    LB, MB, UB, SL, SU
} Berth;


typedef struct Passenger {
    char name[50];
    int  age;
    Gender gender;
    int  coach_no;
    int  seat_no;
    Berth berth;
    int  pnr;
    int  train_no;
} Passenger;

typedef struct BPTreeNode {
    int isLeaf;
    int numKeys;

    int   keys[MAX_KEYS];
    struct BPTreeNode *children[ORDER];

    Passenger         *data[MAX_KEYS];
    struct BPTreeNode *next;
    struct BPTreeNode *parent;
} BPTreeNode;

typedef struct Coach {
    int  coach_no;
    char type[10];
    int  capacity;
    int  available_seats;
    int  train_no;          

    int seats[73];
} Coach;

typedef struct BPTree {
    BPTreeNode *root;
} BPTree;

typedef struct Train {
    int  train_no;
    char name[50];
    int  total_coaches;
} Train;


extern Coach coaches[MAX_COACHES];


char  *berthToString(Berth b);
Berth  stringToBerth(char *str);
char  *getBerthType(int seat_no);


BPTreeNode *createNode(int isLeaf);
BPTreeNode *findLeaf(BPTreeNode *root, int key);
Passenger  *search(BPTreeNode *root, int key);
void        insertIntoLeaf(BPTreeNode *leaf, int key, Passenger *data);
BPTreeNode *splitLeaf(BPTreeNode *leaf, int key, Passenger *data);
BPTreeNode *insertIntoParent(BPTreeNode *root, BPTreeNode *left,
                             int key, BPTreeNode *right);
BPTreeNode *splitInternal(BPTreeNode *node, BPTreeNode **root);
BPTreeNode *insert(BPTreeNode *root, int key, Passenger *data);
void        rangeSearch(BPTreeNode *root, int start, int end);


void displayAll(BPTreeNode *root);
void displayLowerBerth(BPTreeNode *root);
void displaySeniorNoLower(BPTreeNode *root);


int  findAvailableSeat(Coach *coach, Berth berth);
void initializeCoaches(void);


void bookTicket(BPTreeNode **root, Passenger *p,
                Coach coaches[], int total_coaches);
void bookAcrossCoaches(BPTreeNode **root, Passenger *p,
                       Coach coaches[], int total_coaches);
void maxMatchingBooking(BPTreeNode **root, Passenger *p,
                        Coach coaches[], int total_coaches);


void cancelPassenger(BPTreeNode *root, int pnr);
void processWaitlist(BPTreeNode **root, Passenger *waitlist[],
                     int *waitCount, Coach coaches[], int total_coaches);
void addToWaitlist(Passenger *waitlist[], int *waitCount, Passenger *p);
void popWaitlist(Passenger *waitlist[], int *waitCount);


void        saveToFile(BPTreeNode *root, FILE *fp);
void        saveAll(BPTreeNode *root);
BPTreeNode *loadFromFile(void);

#endif
