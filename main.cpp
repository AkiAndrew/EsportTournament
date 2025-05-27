#include <iostream>

using namespace std;

// Forward declaration
void manageMatches();

int main() {
    int choice;

    while (true) {
        cout << "\n===== MAIN MENU =====\n";
        cout << "1. Manage Matches\n";
        cout << "2. Option 2 (Coming Soon)\n";
        cout << "3. Option 3 (Coming Soon)\n";
        cout << "4. Exit\n";
        cout << "Choose an option (1-4): ";
        cin >> choice;

        switch (choice) {
            case 1:
                manageMatches();  // Defined in match_manager.cpp
                break;
            case 2:
                cout << "Option 2 not implemented yet.\n";
                break;
            case 3:
                cout << "Option 3 not implemented yet.\n";
                break;
            case 4:
                cout << "Exiting...\n";
                return 0;
            default:
                cout << "Invalid choice. Try again.\n";
        }
    }

    return 0;
}
