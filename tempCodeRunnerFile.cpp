#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <cmath>
#include <cstring>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <algorithm>


using namespace std;

const int MAX_PLAYERS = 500;
const int MAX_TEAMS = 100;
const int MAX_QUALIFIED = 8;

struct Player {
    string id;
    string firstName;
    string lastName;
    string teamId;
    string teamName;
    string university;
    int points;
    bool isBackup = false;
};

struct Team {
    string name;
    string id;
    int totalPoints;
};

struct MatchResult {
    string matchId;
    string teamId;
    string matchType;
    string result; // "Win" or "Lose"
};

struct PlayerStats {
    string matchId;
    string playerId;
    string playerName;
    string teamId;
    int kills;
    int deaths;
    int assists;
    bool isMVP;
};

// Globals
Player players[MAX_PLAYERS];
int playerCount = 0;

Team teams[MAX_TEAMS];
int teamCount = 0;

map<string, string> teamIds;   // teamName -> teamId
map<string, int> teamPoints;   // teamName -> totalPoints

int matchCounter = 1;

// Utility to format match ID
string getMatchId() {
    stringstream ss;
    ss << "M" << setfill('0') << setw(3) << matchCounter++;
    return ss.str();
}

void readCSV(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open file " << filename << endl;
        return;
    }

    string line;
    getline(file, line); // Skip header

    while (getline(file, line)) {
        stringstream ss(line);
        string fields[9];
        for (int i = 0; i < 9 && getline(ss, fields[i], ','); i++);

        if (fields[0].empty() || fields[7].empty()) {
            cerr << "Skipping row due to insufficient data.\n";
            continue;
        }
        Player p;
        p.id = fields[0];
        p.firstName = fields[1];
        p.lastName = fields[2];
        p.teamId = fields[3];
        p.teamName = fields[4];
        p.university = fields[5];
        // Backup is column 6
        string backupStr = fields[6];
        backupStr.erase(remove_if(backupStr.begin(), backupStr.end(), ::isspace), backupStr.end());
        p.isBackup = (backupStr == "true" || backupStr == "1" || backupStr == "True");

        // Points is column 7
        try {
            p.points = stoi(fields[7]);
        } catch (...) {
            cerr << "Skipping row due to invalid points format.\n";
            continue;
        }


        players[playerCount++] = p;
        teamIds[p.teamName] = p.teamId;
        teamPoints[p.teamName] += p.points;
    }

    // Fill teams array
    map<string, bool> added;
    for (int i = 0; i < playerCount; i++) {
        string name = players[i].teamName;
        if (!added[name]) {
            teams[teamCount].name = name;
            teams[teamCount].id = teamIds[name];
            teams[teamCount].totalPoints = teamPoints[name];
            teamCount++;
            added[name] = true;
        }
    }
}

void displayAllTeams() {
    cout << "\n--- All Teams Loaded ---\n";
    for (int i = 0; i < teamCount; i++) {
        cout << teams[i].name << " [" << teams[i].id << "] - " << teams[i].totalPoints << " pts\n";
    }
}

void getQualifiedTeams(Team qualified[MAX_QUALIFIED]) {
    cout << "\nEnter 8 qualified team IDs (one per line):\n";
    string enteredIds[MAX_QUALIFIED];
    int count = 0;

    while (count < MAX_QUALIFIED) {
        cout << "Team ID " << (count + 1) << ": ";
        string id;
        cin >> id;

        bool duplicate = false;
        for (int j = 0; j < count; j++) {
            if (enteredIds[j] == id) {
                cout << "Duplicate ID. Try again.\n";
                duplicate = true;
                break;
            }
        }
        if (duplicate) continue;

        bool found = false;
        for (int i = 0; i < teamCount; i++) {
            if (teams[i].id == id) {
                qualified[count] = teams[i];
                enteredIds[count] = id;
                count++;
                found = true;
                break;
            }
        }
        if (!found) {
            cout << "Invalid ID. Try again.\n";
        }
    }
}

void sortTeamsByPoints(Team teams[], int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (teams[i].totalPoints > teams[j].totalPoints) {
                Team temp = teams[i];
                teams[i] = teams[j];
                teams[j] = temp;
            }
        }
    }
}

void displayMatches(Team roundTeams[], int count) {
    cout << "\n--- Round Matches (closest in points) ---\n";
    for (int i = 0; i + 1 < count; i += 2) {
        cout << "Match: " << roundTeams[i].name << " [" << roundTeams[i].id << "] vs "
             << roundTeams[i + 1].name << " [" << roundTeams[i + 1].id << "]\n";
    }
    if (count % 2 != 0) {
        cout << "Team: " << roundTeams[count - 1].name << " [" << roundTeams[count - 1].id << "] gets a bye.\n";
    }
}

int getTeamIndexById(Team roundTeams[], int count, const string& id) {
    for (int i = 0; i < count; i++) {
        if (roundTeams[i].id == id) return i;
    }
    return -1;
}

void logMatchAndStats(string matchId, string matchType, Team team1, Team team2, string winnerId) {
    ofstream matchFile("match_results.csv", ios::app);
    ofstream statsFile("player_stats.csv", ios::app);

    for (int i = 0; i < 2; i++) {
        Team team = (i == 0) ? team1 : team2;
        string result = (team.id == winnerId) ? "Win" : "Lose";

        matchFile << matchId << "," << team.id << "," << matchType << "," << result << "\n";

        // === MVP logic ===
        int nonBackupIndices[50]; // temporary array to store indices of non-backup players
        int count = 0;

        for (int j = 0; j < playerCount; j++) {
            if (players[j].teamId == team.id && !players[j].isBackup) {
                nonBackupIndices[count++] = j;
            }
        }

        int mvpIndex = -1;
        if (team.id == winnerId && count > 0) {
            mvpIndex = nonBackupIndices[rand() % count]; // pick random non-backup player
        }

        for (int i = 0; i < count; i++) {
            int j = nonBackupIndices[i];
            bool isMVP = (j == mvpIndex);
            int kills = rand() % 11;
            int deaths = rand() % 6;
            int assists = rand() % 8;

            statsFile << matchId << "," << players[j].id << "," << players[j].firstName + " " + players[j].lastName
                      << "," << team.id << "," << kills << "," << deaths << "," << assists << ","
                      << (isMVP ? "true" : "false") << "\n";
        }
    }
}


void runTournament(Team qualified[MAX_QUALIFIED]) {
    int roundCount = MAX_QUALIFIED;
    Team currentRound[MAX_QUALIFIED];
    for (int i = 0; i < MAX_QUALIFIED; i++) {
        currentRound[i] = qualified[i];
    }

    int round = 1;

    while (roundCount > 1) {
        cout << "\n========== ROUND " << round << " ==========\n";

        string matchType = (round == 1) ? "Quarterfinal" : (round == 2) ? "Semifinal" : "Final";

        sortTeamsByPoints(currentRound, roundCount);
        displayMatches(currentRound, roundCount);

        Team nextRound[MAX_QUALIFIED];
        int nextCount = 0;

        for (int i = 0; i + 1 < roundCount; i += 2) {
            string winnerId;
            bool valid = false;
            string matchId = getMatchId();

            while (!valid) {
                cout << "Enter the winning team ID for match (" << currentRound[i].id << " vs " << currentRound[i + 1].id << "): ";
                cin >> winnerId;
                if (winnerId == currentRound[i].id || winnerId == currentRound[i + 1].id) {
                    int index = getTeamIndexById(currentRound, roundCount, winnerId);
                    nextRound[nextCount++] = currentRound[index];
                    logMatchAndStats(matchId, matchType, currentRound[i], currentRound[i + 1], winnerId);
                    valid = true;
                } else {
                    cout << "Invalid input. Try again.\n";
                }
            }
        }

        if (roundCount % 2 != 0) {
            nextRound[nextCount++] = currentRound[roundCount - 1];
            cout << "Team " << currentRound[roundCount - 1].name << " gets a bye to next round.\n";
        }

        for (int i = 0; i < nextCount; i++) {
            currentRound[i] = nextRound[i];
        }
        roundCount = nextCount;
        round++;
    }

    if (roundCount == 1) {
        cout << "\n Winner of the Tournament: " << currentRound[0].name << " [" << currentRound[0].id << "]\n";
    }
}

// Entry point
void manageMatches() {
    srand(time(0));
    readCSV("Groups_dataset.csv");
    displayAllTeams();

    Team qualified[MAX_QUALIFIED];
    getQualifiedTeams(qualified);

    runTournament(qualified);
}
