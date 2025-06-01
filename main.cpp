#include <iostream>
#include "spectator_manager.h"
using namespace std;

// Declare external functions
void manageMatches();      // From match_manager.cpp
void databaseMenu();       // From database_viewer.cpp

int main() {
    int choice;

    while (true) {
        cout << "\n===== MAIN MENU =====\n";
        cout << "1. Manage Matches\n";
        cout << "2. Tournament Registration\n";
        cout << "3. Manage Spectators/Live Stream\n";
        cout << "4. View Game Data\n";
        cout << "5. Exit\n";
        cout << "Choose an option (1-5): ";
        cin >> choice;

        switch (choice) {
            case 1:
                manageMatches();
                break;
            case 2:
                cout << "Option 2 not implemented yet.\n";
                break;
            case 3:
                manageSpectators();
                break;
            case 4:
                databaseMenu(); // Calls function from database_viewer.cpp
                break;
            case 5:
                cout << "Exiting the program.\n";
                return 0;
            default:
                cout << "Invalid choice. Try again.\n";
        }
    }

    return 0;
}
