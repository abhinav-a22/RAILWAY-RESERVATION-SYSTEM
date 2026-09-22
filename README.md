# Railway Reservation System using B+ Tree

An advanced, high-performance **Railway Reservation and Management System** implemented in **C**, leveraging a **B+ Tree (Order 4)** data structure for passenger record indexing, fast PNR lookups, berth allocation, ticket cancellation, waitlist queuing, and file-based persistence.

Developed as part of the **Data Structures and Program Design - II (DSPD-2)** curriculum.

---

## Table of Contents

- [Overview](#overview)
- [Why B+ Tree?](#why-b-tree)
- [Key Features](#key-features)
- [System Architecture & Data Structures](#system-architecture--data-structures)
- [Seat & Berth Allocation Logic](#seat--berth-allocation-logic)
- [Project Structure](#project-structure)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Compilation](#compilation)
  - [Running the Application](#running-the-application)
- [Menu Interface & Usage](#menu-interface--usage)
- [Complexity Analysis](#complexity-analysis)
- [File Persistence Format](#file-persistence-format)
- [License & Author](#license--author)

---

## Overview

Traditional reservation systems handle thousands of passenger records where fast search, insertion, and range querying are critical. This project demonstrates how database indexing principles can be applied in C using an in-memory **B+ Tree** indexed by **PNR (Passenger Name Record)**.

The system manages 10 coaches (both AC and Sleeper) with 72 seats each, intelligently allocating berths (Lower, Middle, Upper, Side Lower, Side Upper) based on railway coach layouts, handling duplicate PNR checks, waitlist queuing, and file serialization.

---

## Why B+ Tree?

Unlike standard Binary Search Trees (BST) or AVL Trees:
1. **Shallow Depth & High Fanout**: With an order of $M = 4$, tree height remains low, minimizing search steps.
2. **Linked Leaf Nodes**: All data records reside strictly in leaf nodes. Leaves are chained together via `next` pointers, enabling linear $O(K)$ sequential scans and range queries without traversing back up through internal nodes.
3. **Balanced Growth**: Node splitting maintains $O(\log N)$ insertion, deletion, and search performance at all times.

---

## Key Features

- **B+ Tree Indexing**:
  - Order 4 tree with up to 3 keys per node.
  - Automatic node splitting (leaf and internal) with height growth.
  - Fast search by unique PNR key.
  - Range search for retrieving passengers across a range of PNRs.

- **Intelligent Coach & Berth Allocation**:
  - Manages **10 Coaches** (Coaches 1–3: AC, Coaches 4–10: Sleeper), 72 berths per coach.
  - Real-time seat occupancy tracking per coach.
  - Modulo arithmetic mapping seat numbers to standard Indian Railway berth configurations.
  - Fallback booking strategies: preferred berth booking, cross-coach allocation, and maximum matching coach allocation.

- **Waitlist Management (FIFO Queue)**:
  - Supports up to 50 waitlisted passengers when coaches are full.
  - Automated waitlist resolution (`processWaitlist`) to allocate newly freed seats upon ticket cancellation.

- **Passenger Care & Queries**:
  - **Display All Passengers**: In-order traversal across linked leaf nodes.
  - **Display Lower Berth Passengers**: Quick filter for passengers assigned to Lower Berths.
  - **Senior Citizen Priority Query**: Identifies senior citizens ($\text{Age} \ge 60$) who were not assigned Lower Berths (for reallocation priority).

- **Data Persistence**:
  - Serializes all passenger bookings to `passengers.txt`.
  - Reconstructs the B+ Tree from disk on program startup.

---

## System Architecture & Data Structures

```
                 +-------------------+
                 |   Internal Node   |
                 |     [Keys Only]   |
                 +---------+---------+
                           |
            +--------------+--------------+
            |                             |
  +---------v---------+         +---------v---------+
  |     Leaf Node     | ------> |     Leaf Node     | ------> NULL
  | [Keys + Data Ptrs]|  next   | [Keys + Data Ptrs]|  next
  +---------+---------+         +---------+---------+
            |                             |
      +-----v-----+                 +-----v-----+
      | Passenger |                 | Passenger |
      +-----------+                 +-----------+
```

### Core Structures (`binary_tree.h`)

#### Passenger
```c
typedef struct Passenger {
    char   name[50];
    int    age;
    Gender gender;      // MALE (0), FEMALE (1)
    int    coach_no;    // 1 to 10
    int    seat_no;     // 1 to 72
    Berth  berth;       // LB, MB, UB, SL, SU
    int    pnr;         // Unique Primary Key
    int    train_no;
} Passenger;
```

#### B+ Tree Node
```c
typedef struct BPTreeNode {
    int                isLeaf;
    int                numKeys;
    int                keys[MAX_KEYS];              // MAX_KEYS = ORDER - 1 = 3
    struct BPTreeNode *children[ORDER];             // ORDER = 4
    Passenger         *data[MAX_KEYS];              // Stored only at leaves
    struct BPTreeNode *next;                        // Chaining leaves for range queries
    struct BPTreeNode *parent;
} BPTreeNode;
```

#### Coach
```c
typedef struct Coach {
    int  coach_no;
    char type[10];              // "AC" or "SL"
    int  capacity;              // 72 seats
    int  available_seats;
    int  train_no;
    int  seats[73];             // 1-indexed boolean array (0 = free, 1 = booked)
} Coach;
```

---

## Seat & Berth Allocation Logic

A standard 72-berth compartment follows an 8-berth repetitive pattern mapped using modulo arithmetic:

| Seat Number (mod 8) | Berth Type | Abbreviation |
|:-------------------:|:----------:|:------------:|
| 1 or 4              | Lower Berth | `LB` |
| 2 or 5              | Middle Berth| `MB` |
| 3 or 6              | Upper Berth | `UB` |
| 7                   | Side Lower  | `SL` |
| 0                   | Side Upper  | `SU` |

```c
char *getBerthType(int seat_no) {
    int mod = seat_no % 8;
    if (mod == 1 || mod == 4) return "LB";
    else if (mod == 2 || mod == 5) return "MB";
    else if (mod == 3 || mod == 6) return "UB";
    else if (mod == 7)             return "SL";
    else                           return "SU";
}
```

---

## Project Structure

```text
.
├── binary_tree.h       # Struct definitions, macros, and function prototypes
├── binary_tree.c       # B+ Tree implementation, coach algorithms, persistence logic
├── main.c              # Interactive CLI menu-driven interface
├── passengers.txt      # Plaintext database file for persistent records
├── .gitignore          # Ignores build binaries (*.exe, *.out, .vscode)
└── README.md           # Project documentation
```

---

## Getting Started

### Prerequisites

- A C compiler supporting C99 or later (`gcc`, `clang`, or MSVC / MinGW).
- Git (for version control).

### Compilation

Compile the project using `gcc`:

```bash
gcc -Wall -Wextra main.c binary_tree.c -o railway_system
```

### Running the Application

**Windows:**
```cmd
railway_system.exe
```

**Linux / macOS:**
```bash
./railway_system
```

---

## Menu Interface & Usage

Upon launch, you will be presented with the interactive menu:

```text
===== Railway Reservation System =====
1. Book Ticket
2. Cancel Ticket
3. Display All Passengers
4. Display Lower Berth Passengers
5. Display Senior Citizens without LB
6. Add to Waitlist
7. Process Waitlist
8. Save Data
9. Load Data
0. Exit
Enter choice:
```

### Operation Guide:

- **1. Book Ticket**: Input PNR, Name, Age, Gender (0: Male, 1: Female), Train No, and Preferred Berth (0: LB, 1: MB, 2: UB, 3: SL, 4: SU). Duplicate PNRs are rejected.
- **2. Cancel Ticket**: Enter the PNR. Reclaims the seat in the corresponding coach.
- **3. Display All Passengers**: Traverses the leaf nodes sequentially to print all confirmed bookings.
- **4. Display Lower Berth Passengers**: Filters passengers assigned to Lower Berths.
- **5. Display Senior Citizens without LB**: Flags senior citizens ($\text{age} \ge 60$) who do not hold a Lower Berth.
- **6. Add to Waitlist**: Places a passenger into the FIFO waitlist queue when coaches are full.
- **7. Process Waitlist**: Automatically checks available seats and books waitlisted passengers in order.
- **8. Save Data**: Dumps all confirmed bookings from the B+ Tree into `passengers.txt`.
- **9. Load Data**: Reads `passengers.txt` and reconstructs the B+ Tree in memory.

---

## Complexity Analysis

| Operation | Average Case | Worst Case | Space Complexity |
|:---|:---:|:---:|:---:|
| **Search by PNR** | $O(\log N)$ | $O(\log N)$ | $O(1)$ |
| **Insert Passenger** | $O(\log N)$ | $O(\log N)$ | $O(1)$ amortized |
| **Range Search** | $O(\log N + K)$ | $O(\log N + K)$ | $O(1)$ |
| **Display All** | $O(N)$ | $O(N)$ | $O(1)$ |
| **Cancel Ticket** | $O(\log N)$ | $O(\log N)$ | $O(1)$ |
| **File Save / Load** | $O(N)$ | $O(N)$ | $O(N)$ |

*(Where $N$ is the number of passengers and $K$ is the number of matching records in a range.)*

---

## File Persistence Format

Records in `passengers.txt` are serialized in space-separated format:

```text
<PNR> <Name> <Age> <Gender> <TrainNo> <CoachNo> <SeatNo> <Berth>
```

**Example:**
```text
1001 Rahul 28 0 12056 1 1 LB
1002 Priya 62 1 12056 1 2 MB
```

---

## License & Author

- **Course**: Data Structures & Program Design II (DSPD-2)
- **Author**: Abhinav ([@abhinav-a22](https://github.com/abhinav-a22))
