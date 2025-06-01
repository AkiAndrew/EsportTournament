#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "player.hpp"
#include "node.hpp"
#include "queue.hpp"

using namespace std;

// Match structure (no STL containers)
struct Match {
    string matchID;
    string stage;
    string teamA;
    string teamB;
    string time;
    string winner;
    
    Match() {
        matchID = "";
        stage = "";
        teamA = "";
        teamB = "";
        time = "";
        winner = "";
    }
};

// Simple array-based container for matches (no STL)
class MatchContainer {
private:
    Match* matches;
    int capacity;
    int count;

public:
    MatchContainer() {
        capacity = 100; // Initial capacity
        matches = new Match[capacity];
        count = 0;
    }
    
    ~MatchContainer() {
        delete[] matches;
    }
    
    void addMatch(Match m) {
        if (count < capacity) {
            matches[count] = m;
            count++;
        }
    }
    
    int getCount() {
        return count;
    }
    
    Match getMatch(int index) {
        if (index >= 0 && index < count) {
            return matches[index];
        }
        return Match();
    }
};

// Global queues
Queue earlyBirdQueue, wildcardQueue, regularQueue;
MatchContainer matchSchedule;

// Determine player type based on ID (no STL)
string determineType(string playerID) {
    if (playerID.length() < 2) return "Regular";
    
    // Extract number from playerID (assuming format like P001, P025, etc.)
    string numberPart = playerID.substr(1); // Remove 'P'
    int idNum = 0;
    
    // Convert string to int manually (no stoi)
    for (int i = 0; i < numberPart.length(); i++) {
        if (numberPart[i] >= '0' && numberPart[i] <= '9') {
            idNum = idNum * 10 + (numberPart[i] - '0');
        }
    }
    
    if (idNum <= 20) return "EarlyBird";
    else if (idNum <= 50) return "Wildcard";
    else return "Regular";
}

// Load players from CSV file (no STL)
void loadPlayers(string filename) {
    ifstream file(filename.c_str());
    if (!file.is_open()) {
        cout << "Warning: Unable to open file " << filename << endl;
        return;
    }

    string line;
    getline(file, line); // Skip header
    int count = 0;

    while (getline(file, line)) {
        // Parse CSV line manually (no stringstream for full STL-free)
        Player p;
        string field = "";
        int fieldIndex = 0;
        
        for (int i = 0; i <= line.length(); i++) {
            if (i == line.length() || line[i] == ',') {
                // Process completed field
                switch (fieldIndex) {
                    case 0: p.playerID = field; break;
                    case 1: p.firstName = field; break;
                    case 2: p.lastName = field; break;
                    case 3: p.teamID = field; break;
                    case 4: p.teamName = field; break;
                    case 5: p.university = field; break;
                }
                field = "";
                fieldIndex++;
            } else {
                field += line[i];
            }
        }

        p.type = determineType(p.playerID);
        p.isCheckedIn = false;
        p.status = "active";

        // Add to appropriate queue
        if (p.type == "EarlyBird") earlyBirdQueue.enqueue(p);
        else if (p.type == "Wildcard") wildcardQueue.enqueue(p);
        else regularQueue.enqueue(p);
        
        count++;
    }

    file.close();
    cout << "Successfully loaded " << count << " players\n";
}

// Load match schedule from CSV (no STL)
void loadMatchSchedule(string filename) {
    ifstream file(filename.c_str());
    if (!file.is_open()) {
        cout << "Warning: Unable to open file " << filename << endl;
        return;
    }

    string line;
    getline(file, line); // skip header

    while (getline(file, line)) {
        Match m;
        string field = "";
        int fieldIndex = 0;
        
        for (int i = 0; i <= line.length(); i++) {
            if (i == line.length() || line[i] == ',') {
                // Process completed field
                switch (fieldIndex) {
                    case 0: m.matchID = field; break;
                    case 1: m.stage = field; break;
                    case 2: m.teamA = field; break;
                    case 3: m.teamB = field; break;
                    case 4: m.time = field; break;
                    case 5: m.winner = field; break;
                }
                field = "";
                fieldIndex++;
            } else {
                field += line[i];
            }
        }
        matchSchedule.addMatch(m);
    }
    file.close();
}

// Display all queues with enhanced formatting
void showAllQueues() {
    cout << "\n========== TOURNAMENT QUEUES ==========\n";
    
    cout << "\n🥇 Early Bird Queue (" << earlyBirdQueue.getSize() << " players):\n";
    earlyBirdQueue.display();
    earlyBirdQueue.showStatusSummary();
    
    cout << "\n🎲 Wildcard Queue (" << wildcardQueue.getSize() << " players):\n";
    wildcardQueue.display();
    wildcardQueue.showStatusSummary();
    
    cout << "\n👥 Regular Queue (" << regularQueue.getSize() << " players):\n";
    regularQueue.display();
    regularQueue.showStatusSummary();
    
    cout << "\n=======================================\n";
}

// Check if team is ready (60% of members checked in) - no STL
bool isTeamReady(string teamID) {
    int totalA = 0, checkedA = 0;
    int totalB = 0, checkedB = 0;
    int totalC = 0, checkedC = 0;
    
    // Check early bird queue
    earlyBirdQueue.checkTeamReadiness(teamID, totalA, checkedA);
    // Check wildcard queue
    wildcardQueue.checkTeamReadiness(teamID, totalB, checkedB);
    // Check regular queue
    regularQueue.checkTeamReadiness(teamID, totalC, checkedC);
    
    int total = totalA + totalB + totalC;
    int checked = checkedA + checkedB + checkedC;
    
    return (total > 0 && checked >= total * 0.6);
}

// Show ready matches
void showReadyMatches() {
    cout << "\n🏆 READY MATCHES (Both teams checked in ≥60%):\n";
    cout << "Match ID | Stage | Team A vs Team B | Time\n";
    cout << "---------|-------|------------------|-----\n";
    
    bool hasReadyMatches = false;
    
    for (int i = 0; i < matchSchedule.getCount(); i++) {
        Match m = matchSchedule.getMatch(i);
        if (isTeamReady(m.teamA) && isTeamReady(m.teamB)) {
            cout << m.matchID << " | " << m.stage << " | "
                 << m.teamA << " vs " << m.teamB << " | " << m.time << " ✅\n";
            hasReadyMatches = true;
        }
    }
    
    if (!hasReadyMatches) {
        cout << "No matches are ready yet. ⏳\n";
    }
}

// Register new player manually
void registerNewPlayer() {
    Player newP;
    cout << "\n=== REGISTER NEW PLAYER ===\n";
    cout << "Enter player ID (e.g., P075): "; 
    cin >> newP.playerID;
    cout << "First name: "; 
    cin >> newP.firstName;
    cout << "Last name: "; 
    cin >> newP.lastName;
    cout << "Team ID: "; 
    cin >> newP.teamID;
    cout << "Team name: "; 
    cin >> newP.teamName;
    cout << "University: "; 
    cin.ignore(); 
    getline(cin, newP.university);

    newP.type = determineType(newP.playerID);
    newP.isCheckedIn = false;
    newP.status = "active";

    if (newP.type == "EarlyBird") earlyBirdQueue.enqueue(newP);
    else if (newP.type == "Wildcard") wildcardQueue.enqueue(newP);
    else regularQueue.enqueue(newP);
    
    cout << "[Success] Player registered as " << newP.type << "!\n";
}

// Process next player in all queues
void processNextInAllQueues() {
    cout << "\n🔄 PROCESSING NEXT PLAYERS IN ALL QUEUES:\n";
    cout << "\n--- Early Bird Queue ---\n";
    earlyBirdQueue.processNextPlayer();
    
    cout << "\n--- Wildcard Queue ---\n";
    wildcardQueue.processNextPlayer();
    
    cout << "\n--- Regular Queue ---\n";
    regularQueue.processNextPlayer();
}

// Show menu
void showMenu() {
    cout << "\n========== TOURNAMENT REGISTRATION MANAGEMENT ==========\n";
    cout << "1. Display all queues\n";
    cout << "2. Player check-in\n";
    cout << "3. Register new player (manual input)\n";
    cout << "4. Withdraw player from tournament\n";
    cout << "5. Display ready matches\n";
    cout << "6. Process next player in queue\n";
    cout << "0. Exit program\n";
    cout << "=========================================================\n";
    cout << "Enter your choice: ";
}

// Main program
int main() {
    cout << "🏆 Welcome to Tournament Registration Management System\n";
    cout << "📋 System initializing...\n";
    
    // Load initial data
    cout << "\n📂 Loading player data...\n";
    loadPlayers("Updated_Player_Data.csv");
    
    cout << "\n📅 Loading match schedule...\n";
    loadMatchSchedule("Realistic_Match_Schedule.csv");
    
    // Show initial state
    showAllQueues();
    showReadyMatches();

    // Main menu loop
    int choice;
    do {
        showMenu();
        cin >> choice;
        cin.ignore(); // Clear input buffer

        switch (choice) {
            case 1:
                showAllQueues();
                break;
                
            case 2: {
                string id;
                cout << "Enter the player ID to check-in (e.g., P005): ";
                cin >> id;
                
                bool found = earlyBirdQueue.checkInPlayer(id) ||
                           wildcardQueue.checkInPlayer(id) ||
                           regularQueue.checkInPlayer(id);
                           
                if (!found) {
                    cout << "[Failed] Player " << id << " not found.\n";
                }
                break;
            }
            
            case 3:
                registerNewPlayer();
                break;
            
            case 4: {
                string id;
                cout << "Enter the player ID to withdraw (e.g., P007): ";
                cin >> id;
                
                if (earlyBirdQueue.withdrawPlayer(id) ||
                    wildcardQueue.withdrawPlayer(id) ||
                    regularQueue.withdrawPlayer(id)) {
                    cout << "[Success] Player " << id << " status changed to eliminated.\n";
                } else {
                    cout << "[Failed] Player " << id << " not found.\n";
                }
                break;
            }
            
            case 5:
                showReadyMatches();
                break;
                
            case 6:
                processNextInAllQueues();
                break;
                
            case 7:
                cout << "🎯 Program ended. Thank you for using Tournament Management System!\n";
                break;
                
            default:
                cout << "❌ Invalid choice, please try again.\n";
                break;
        }
        
    } while (choice != 0);

    return 0;
}