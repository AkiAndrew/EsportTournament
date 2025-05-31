#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>

using namespace std;

// === Helper Functions ===
bool containsIgnoreCase(const string& text, const string& keyword) {
    string lowerText = text, lowerKeyword = keyword;
    for (char& c : lowerText) c = tolower(c);
    for (char& c : lowerKeyword) c = tolower(c);
    return lowerText.find(lowerKeyword) != string::npos;
}

// === Core Display ===
void displayAllPlayerStats() {
    ifstream file("player_stats.csv");
    if (!file.is_open()) {
        cerr << "Error: Cannot open player_stats.csv\n";
        return;
    }

    string line;
    getline(file, line); // skip header
    cout << "\n--- Player Stats ---\n";
    cout << left << setw(8) << "Match" << setw(10) << "PlayerID" << setw(20) << "Name"
         << setw(10) << "TeamID" << setw(8) << "Kills" << setw(8) << "Deaths"
         << setw(8) << "Assists" << setw(6) << "MVP" << "\n";

    while (getline(file, line)) {
        stringstream ss(line);
        string matchId, playerId, playerName, teamId, kills, deaths, assists, mvp;
        getline(ss, matchId, ','); getline(ss, playerId, ','); getline(ss, playerName, ',');
        getline(ss, teamId, ','); getline(ss, kills, ','); getline(ss, deaths, ',');
        getline(ss, assists, ','); getline(ss, mvp, ',');

        cout << left << setw(8) << matchId << setw(10) << playerId << setw(20) << playerName
             << setw(10) << teamId << setw(8) << kills << setw(8) << deaths
             << setw(8) << assists << setw(6) << mvp << "\n";
    }
    file.close();
}

void displayAllMatchResults() {
    ifstream file("match_results.csv");
    if (!file.is_open()) {
        cerr << "Error: Cannot open match_results.csv\n";
        return;
    }

    string line;
    getline(file, line); // skip header
    cout << "\n--- Match Results ---\n";
    cout << left << setw(8) << "MatchID" << setw(10) << "TeamID" << setw(15) << "Match Type" << setw(6) << "Result" << "\n";

    while (getline(file, line)) {
        stringstream ss(line);
        string matchId, teamId, matchType, result;
        getline(ss, matchId, ','); getline(ss, teamId, ','); getline(ss, matchType, ','); getline(ss, result, ',');

        cout << left << setw(8) << matchId << setw(10) << teamId << setw(15) << matchType << setw(6) << result << "\n";
    }
    file.close();
}

// === Filters ===
void searchPlayerStats() {
    int choice;
    cout << "\n-- Filter Player Stats --\n";
    cout << "1. By Player ID\n2. By Team ID\n3. MVP Only\nEnter choice: ";
    cin >> choice;

    string keyword;
    ifstream file("player_stats.csv");
    if (!file.is_open()) {
        cerr << "Error opening file.\n";
        return;
    }
    string line;
    getline(file, line); // skip header
    cout << "\n--- Filtered Player Stats ---\n";
    cout << left << setw(8) << "Match" << setw(10) << "PlayerID" << setw(20) << "Name"
         << setw(10) << "TeamID" << setw(8) << "Kills" << setw(8) << "Deaths"
         << setw(8) << "Assists" << setw(6) << "MVP" << "\n";

    switch (choice) {
        case 1:
            cout << "Enter Player ID: "; cin >> keyword;
            break;
        case 2:
            cout << "Enter Team ID: "; cin >> keyword;
            break;
        case 3:
            keyword = "true";
            break;
        default:
            cout << "Invalid filter.\n"; return;
    }

    while (getline(file, line)) {
        stringstream ss(line);
        string matchId, playerId, playerName, teamId, kills, deaths, assists, mvp;
        getline(ss, matchId, ','); getline(ss, playerId, ','); getline(ss, playerName, ',');
        getline(ss, teamId, ','); getline(ss, kills, ','); getline(ss, deaths, ',');
        getline(ss, assists, ','); getline(ss, mvp, ',');

        if ((choice == 1 && containsIgnoreCase(playerId, keyword)) ||
            (choice == 2 && containsIgnoreCase(teamId, keyword)) ||
            (choice == 3 && mvp == "true")) {
            cout << left << setw(8) << matchId << setw(10) << playerId << setw(20) << playerName
                 << setw(10) << teamId << setw(8) << kills << setw(8) << deaths
                 << setw(8) << assists << setw(6) << mvp << "\n";
        }
        if (choice == 3) {
    for (char& c : keyword) c = tolower(c);
}

    }
    file.close();
}

void searchMatchResults() {
    int choice;
    cout << "\n-- Filter Match Results --\n";
    cout << "1. By Match ID\n2. By Team ID\n3. By Match Type (Quarterfinal, Semifinal, Final)\nEnter choice: ";
    cin >> choice;

    string keyword;
    cout << "Enter keyword: ";
    cin >> keyword;

    // Convert keyword to lowercase for matching
    for (char& c : keyword) c = tolower(c);

    // Validate match type keyword if choice is 3
    if (choice == 3) {
        if (keyword != "quarterfinal" && keyword != "semifinal" && keyword != "final") {
            cout << "Invalid match type. Please enter 'Quarterfinal', 'Semifinal', or 'Final'.\n";
            return;
        }
    }

    ifstream file("match_results.csv");
    if (!file.is_open()) {
        cerr << "Error opening file.\n";
        return;
    }

    string line;
    getline(file, line); // skip header
    cout << "\n--- Filtered Match Results ---\n";
    cout << left << setw(8) << "MatchID" << setw(10) << "TeamID" << setw(15) << "Match Type" << setw(6) << "Result" << "\n";

    while (getline(file, line)) {
        stringstream ss(line);
        string matchId, teamId, matchType, result;
        getline(ss, matchId, ','); getline(ss, teamId, ','); getline(ss, matchType, ','); getline(ss, result, ',');

        if (choice == 1 && containsIgnoreCase(matchId, keyword)) {
            cout << left << setw(8) << matchId << setw(10) << teamId << setw(15) << matchType << setw(6) << result << "\n";
        } else if (choice == 2 && containsIgnoreCase(teamId, keyword)) {
            cout << left << setw(8) << matchId << setw(10) << teamId << setw(15) << matchType << setw(6) << result << "\n";
        } else if (choice == 3) {
            // Convert matchType to lowercase for comparison
            for (char& c : matchType) c = tolower(c);
            if (matchType == keyword) {
                cout << left << setw(8) << matchId << setw(10) << teamId << setw(15) << matchType << setw(6) << result << "\n";
            }
        }
    }
    file.close();
}


// === Main Database Menu ===
void databaseMenu() {
    int choice;
    do {
        cout << "\n== Database Viewer ==\n";
        cout << "1. Display all player stats\n";
        cout << "2. Display all match results\n";
        cout << "3. Search player stats\n";
        cout << "4. Search match results\n";
        cout << "0. Back to main menu\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                displayAllPlayerStats();
                break;
            case 2:
                displayAllMatchResults();
                break;
            case 3:
                searchPlayerStats();
                break;
            case 4:
                searchMatchResults();
                break;
            case 0:
                cout << "Returning to main menu.\n";
                break;
            default:
                cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 0);
}

