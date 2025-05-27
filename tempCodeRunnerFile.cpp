#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>

using namespace std;

struct Player {
    string id;
    string firstName;
    string lastName;
    string teamId;
    string teamName;
    string university;
    int points;
};

struct Team {
    string name;
    string id;
    int totalPoints;
};

bool comparePoints(const Player &a, const Player &b) {
    return a.points > b.points;
}

bool compareTeamPoints(const Team &a, const Team &b) {
    return a.totalPoints > b.totalPoints;
}

void readCSVToArray(const string &filename, vector<vector<string>> &dataRows) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open file " << filename << endl;
        return;
    }

    string line, word;
    getline(file, line); // Skip header

    while (getline(file, line)) {
        stringstream ss(line);
        vector<string> row;
        while (getline(ss, word, ',')) {
            row.push_back(word);
        }
        dataRows.push_back(row);
    }
}

void loadPlayers(const vector<vector<string>> &dataRows, map<string, vector<Player>> &groupedPlayers, map<string, string> &teamIds) {
    for (const auto &row : dataRows) {
        if (row.size() < 8) {
            cerr << "Skipping row due to insufficient data.\n";
            continue;
        }
        try {
            Player p{row[0], row[1], row[2], row[3], row[4], row[5], stoi(row[7])};
            groupedPlayers[p.teamName].push_back(p);
            teamIds[p.teamName] = p.teamId;
        } catch (...) {
            cerr << "Skipping row due to invalid points format.\n";
        }
    }

    // Prepare and sort teams by total points
    vector<Team> teams;
    for (const auto &group : groupedPlayers) {
        int totalPoints = 0;
        for (const auto &player : group.second) {
            totalPoints += player.points;
        }
        teams.push_back({group.first, teamIds[group.first], totalPoints});
    }
    sort(teams.begin(), teams.end(), compareTeamPoints);

    // Display sorted groups
    cout << "\n--- Grouped Players by Total Points ---\n";
    int groupId = 1;
    for (const auto &team : teams) {
        cout << "\nGroup " << groupId++ << " (" << team.name << " [" << team.id <<"]):" << endl;
        int totalPoints = 0;
        const auto &players = groupedPlayers[team.name];
        for (const auto &player : players) {
            cout << player.firstName << " " << player.lastName << " (" << player.points << " pts)" << endl;
            totalPoints += player.points;
        }
        cout << "Total Points for Group: " << totalPoints << "\n";
    }
}

void displayMatches(const vector<Team> &teams) {
    cout << "\n--- Round Matches ---\n";
    for (size_t i = 0; i + 1 < teams.size(); i += 2) {
        cout << "Match: " << teams[i].name << " [" << teams[i].id << "] (" << teams[i].totalPoints << " pts) vs "
             << teams[i + 1].name << " [" << teams[i + 1].id << "] (" << teams[i + 1].totalPoints << " pts)\n";
    }
    if (teams.size() % 2 != 0) {
        cout << "Team: " << teams.back().name << " [" << teams.back().id << "] (" << teams.back().totalPoints << " pts) has no opponent and gets a bye.\n";
    }
}

vector<Team> getWinnersFromInput(const vector<Team> &currentRound) {
    vector<Team> nextRound;
    for (size_t i = 0; i + 1 < currentRound.size(); i += 2) {
        string winnerId;
        bool validInput = false;
        while (!validInput) {
            cout << "Enter the winning team ID for match (" << currentRound[i].id << " vs " << currentRound[i + 1].id << "): ";
            cin >> winnerId;

            if (winnerId == currentRound[i].id) {
                nextRound.push_back(currentRound[i]);
                validInput = true;
            } else if (winnerId == currentRound[i + 1].id) {
                nextRound.push_back(currentRound[i + 1]);
                validInput = true;
            } else {
                cout << "Invalid input. Please enter a valid team ID from the match.\n";
            }
        }
    }
    if (currentRound.size() % 2 != 0) {
        cout << "\nTeam " << currentRound.back().name << " [" << currentRound.back().id << "] gets a bye to next round.\n";
        nextRound.push_back(currentRound.back());
    }
    return nextRound;
}

void runTournament(vector<Team> teams) {
    int round = 1;
    while (teams.size() > 1) {
        cout << "\n========== ROUND " << round << " ==========" << endl;
        displayMatches(teams);
        teams = getWinnersFromInput(teams);
        round++;
    }
    if (!teams.empty()) {
        cout << "\nWinner of the Tournament: " << teams[0].name << " [" << teams[0].id << "]\n";
    }
}

void scheduleTeamMatches(const map<string, vector<Player>> &groupedPlayers, const map<string, string> &teamIds) {
    vector<Team> teams;

    // Calculate total points per team
    for (const auto &group : groupedPlayers) {
        int totalPoints = 0;
        for (const auto &player : group.second) {
            totalPoints += player.points;
        }
        teams.push_back({group.first, teamIds.at(group.first), totalPoints});
    }

    // Sort teams by total points ascending to match closest together
    sort(teams.begin(), teams.end(), compareTeamPoints);

    cout << "\n--- Scheduled Team Matches ---\n";
    for (size_t i = 0; i + 1 < teams.size(); i += 2) {
        cout << teams[i].name << " [" << teams[i].id << "] (" << teams[i].totalPoints << " pts) vs "
             << teams[i + 1].name << " [" << teams[i + 1].id << "] (" << teams[i + 1].totalPoints << " pts)" << endl;
    }

    if (teams.size() % 2 != 0) {
        cout << "\nTeam: " << teams.back().name << " [" << teams.back().id << "] has no opponent and gets a bye." << endl;
    }

    runTournament(teams);
}

int main() {
    map<string, vector<Player>> groupedPlayers;
    map<string, string> teamIds;
    vector<vector<string>> dataRows;

    readCSVToArray("Groups_dataset.csv", dataRows);
    loadPlayers(dataRows, groupedPlayers, teamIds);
    scheduleTeamMatches(groupedPlayers, teamIds);

    return 0;
}
