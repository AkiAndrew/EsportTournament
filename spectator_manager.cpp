#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include "spectator_manager.h"
using namespace std;

const int MAX_SPECTATORS = 200;

struct Spectator {
    string id;
    string firstName;
    string lastName;
    string type;
    int priority;
    string reservedSlot;
    string status;
    string assignedSlot;
};

// Global array for spectators
Spectator spectators[MAX_SPECTATORS];
int spectatorCount = 0;

// --- CSV Handling ---
void loadSpectatorsFromCSV(const string& filename) {
    spectatorCount = 0;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Cannot open " << filename << endl;
        return;
    }
    string line;
    getline(file, line); // Skip header
    while (getline(file, line) && spectatorCount < MAX_SPECTATORS) {
        stringstream ss(line);
        string field[8];
        for (int i = 0; i < 8; ++i) getline(ss, field[i], ',');
        Spectator s;
        s.id = field[0];
        s.firstName = field[1];
        s.lastName = field[2];
        s.type = field[3];
        s.priority = stoi(field[4]);
        s.reservedSlot = field[5];
        s.status = field[6];
        s.assignedSlot = field[7];
        spectators[spectatorCount++] = s;
    }
    file.close();
}

void saveSpectatorsToCSV(const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Cannot write to " << filename << endl;
        return;
    }
    // Write header
    file << "Spectator ID,First Name,Last Name,Type,Priority,ReservedSlot,Status,AssignedSlot\n";
    for (int i = 0; i < spectatorCount; ++i) {
        file << spectators[i].id << "," << spectators[i].firstName << "," << spectators[i].lastName << "," 
             << spectators[i].type << "," << spectators[i].priority << "," << spectators[i].reservedSlot << ","
             << spectators[i].status << "," << spectators[i].assignedSlot << "\n";
    }
    file.close();
}

// --- Display Functions ---
void displayAllSpectators() {
    cout << "\n--- All Spectators ---\n";
    cout << left << setw(8) << "ID" << setw(12) << "First" << setw(12) << "Last"
         << setw(12) << "Type" << setw(8) << "Priority" << setw(12) << "ResvdSlot"
         << setw(12) << "Status" << setw(10) << "Slot" << endl;
    for (int i = 0; i < spectatorCount; ++i) {
        cout << left << setw(8) << spectators[i].id << setw(12) << spectators[i].firstName 
             << setw(12) << spectators[i].lastName << setw(12) << spectators[i].type 
             << setw(8) << spectators[i].priority << setw(12) << spectators[i].reservedSlot
             << setw(12) << spectators[i].status << setw(10) << spectators[i].assignedSlot << endl;
    }
}

void displayStreamingSlots() {
    cout << "\n--- Streaming Slots ---\n";
    cout << left << setw(8) << "ID" << setw(12) << "Name" << setw(10) << "Slot" << endl;
    for (int i = 0; i < spectatorCount; ++i) {
        if (spectators[i].type == "Streamer" && spectators[i].status == "Streaming") {
            cout << left << setw(8) << spectators[i].id 
                 << setw(12) << (spectators[i].firstName + " " + spectators[i].lastName)
                 << setw(10) << spectators[i].assignedSlot << endl;
        }
    }
}

void displaySeatedSpectators() {
    cout << "\n--- Seated Spectators (VIP/Influencer/General) ---\n";
    cout << left << setw(8) << "ID" << setw(12) << "Name" << setw(12) << "Type" << setw(8) << "Seat" << endl;
    for (int i = 0; i < spectatorCount; ++i) {
        if ((spectators[i].status == "Seated")) {
            cout << left << setw(8) << spectators[i].id 
                 << setw(12) << (spectators[i].firstName + " " + spectators[i].lastName)
                 << setw(12) << spectators[i].type << setw(8) << spectators[i].assignedSlot << endl;
        }
    }
}

// --- Slot Rotation ---
void rotateStreamingSlots() {
    int firstIdx = -1;
    int lastStreamingIdx = -1;
    int minSlot = 999, maxSlot = -1;
    // Find the streamer with the lowest slot (first in circular queue)
    for (int i = 0; i < spectatorCount; ++i) {
        if (spectators[i].type == "Streamer" && spectators[i].status == "Streaming" && !spectators[i].assignedSlot.empty()) {
            int slot = stoi(spectators[i].assignedSlot);
            if (slot < minSlot) { minSlot = slot; firstIdx = i; }
            if (slot > maxSlot) { maxSlot = slot; lastStreamingIdx = i; }
        }
    }
    // Find the first streamer waiting
    int waitingIdx = -1;
    for (int i = 0; i < spectatorCount; ++i) {
        if (spectators[i].type == "Streamer" && spectators[i].status == "Waiting") {
            waitingIdx = i;
            break;
        }
    }
    if (firstIdx == -1 || waitingIdx == -1) {
        cout << "Cannot rotate: Either no streaming or no waiting streamer.\n";
        return;
    }
    // Oldest streaming streamer moves to waiting
    spectators[firstIdx].status = "Waiting";
    spectators[firstIdx].assignedSlot = "";
    // Next waiting streamer takes the highest slot number
    spectators[waitingIdx].status = "Streaming";
    spectators[waitingIdx].assignedSlot = to_string(maxSlot);
    cout << spectators[firstIdx].firstName << " rotated out; " << spectators[waitingIdx].firstName << " now streaming in slot " << maxSlot << ".\n";
}

// --- Add/Remove/Update Spectators ---
void addSpectator() {
    if (spectatorCount >= MAX_SPECTATORS) {
        cout << "Database full. Cannot add more.\n";
        return;
    }
    Spectator s;
    cout << "Enter Spectator ID: "; cin >> s.id;
    cout << "First Name: "; cin >> s.firstName;
    cout << "Last Name: "; cin >> s.lastName;
    cout << "Type (VIP/Influencer/Streamer/General): "; cin >> s.type;
    cout << "Priority (1=VIP,2=Influencer,3=Streamer,4=General): "; cin >> s.priority;
    cout << "Reserved Slot (Yes/No): "; cin >> s.reservedSlot;
    s.status = "Waiting";
    s.assignedSlot = "";
    spectators[spectatorCount++] = s;
    cout << "Added spectator.\n";
}

void removeSpectator() {
    string id;
    cout << "Enter Spectator ID to remove: ";
    cin >> id;
    for (int i = 0; i < spectatorCount; ++i) {
        if (spectators[i].id == id) {
            for (int j = i; j < spectatorCount - 1; ++j) {
                spectators[j] = spectators[j + 1];
            }
            spectatorCount--;
            cout << "Spectator removed.\n";
            return;
        }
    }
    cout << "Not found.\n";
}

void updateSpectatorStatus() {
    string id;
    cout << "Enter Spectator ID to update: ";
    cin >> id;
    for (int i = 0; i < spectatorCount; ++i) {
        if (spectators[i].id == id) {
            cout << "Current status: " << spectators[i].status << ", slot: " << spectators[i].assignedSlot << endl;
            cout << "Enter new status: "; cin >> spectators[i].status;
            cout << "Enter new assigned slot (or blank): ";
            string slot;
            cin.ignore();
            getline(cin, slot);
            spectators[i].assignedSlot = slot;
            cout << "Updated.\n";
            return;
        }
    }
    cout << "Not found.\n";
}

// --- Main Task 3 Menu ---
void manageSpectators() {
    const string CSV_FILE = "Full_Spectator_Queue_completed.csv";
    loadSpectatorsFromCSV(CSV_FILE);

    int choice;
    do {
        cout << "\n=== Spectator & Streamer Queue Management ===\n";
        cout << "1. Display all spectators\n";
        cout << "2. Display streaming slots\n";
        cout << "3. Display seated spectators\n";
        cout << "4. Rotate streaming slots\n";
        cout << "5. Add new spectator\n";
        cout << "6. Remove a spectator\n";
        cout << "7. Update spectator status/slot\n";
        cout << "8. Save changes to CSV\n";
        cout << "0. Back to main menu\n";
        cout << "Choose option: ";
        cin >> choice;
        switch (choice) {
            case 1: displayAllSpectators(); break;
            case 2: displayStreamingSlots(); break;
            case 3: displaySeatedSpectators(); break;
            case 4: rotateStreamingSlots(); break;
            case 5: addSpectator(); break;
            case 6: removeSpectator(); break;
            case 7: updateSpectatorStatus(); break;
            case 8: saveSpectatorsToCSV(CSV_FILE); cout << "Saved.\n"; break;
            case 0: cout << "Returning to main menu.\n"; break;
            default: cout << "Invalid.\n";
        }
    } while (choice != 0);
}
