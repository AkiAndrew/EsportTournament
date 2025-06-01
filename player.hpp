#ifndef PLAYER_H
#define PLAYER_H

#include <string>
using namespace std;

struct Player {
    string playerID;
    string firstName;
    string lastName;
    string teamID;
    string teamName;
    string university;
    string type;         // EarlyBird, Wildcard, Regular
    bool isCheckedIn;    // Check-in status
    string status;       // active, checked-in, eliminated
    
    // Default constructor to initialize values
    Player() {
        playerID = "";
        firstName = "";
        lastName = "";
        teamID = "";
        teamName = "";
        university = "";
        type = "Regular";
        isCheckedIn = false;
        status = "active";
    }
    
    // Constructor with parameters
    Player(string id, string fName, string lName, string tID, string tName, string uni) {
        playerID = id;
        firstName = fName;
        lastName = lName;
        teamID = tID;
        teamName = tName;
        university = uni;
        isCheckedIn = false;
        status = "active";
        type = "Regular"; // Default, will be determined later
    }
};

#endif