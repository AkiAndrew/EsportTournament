#ifndef QUEUE_H
#define QUEUE_H

#include "node.hpp"
#include <iostream>
using namespace std;

class Queue {
private:
    Node* front;    // Pointer to the front of the queue
    Node* rear;     // Pointer to the rear of the queue
    int size;       // Keep track of queue size

public:
    // Constructor
    Queue() {
        front = nullptr;
        rear = nullptr;
        size = 0;
    }

    // Destructor to prevent memory leaks
    ~Queue() {
        while (!isEmpty()) {
            dequeue();
        }
    }

    // Check if the queue is empty
    bool isEmpty() {
        return front == nullptr;
    }

    // Get queue size
    int getSize() {
        return size;
    }

    // Add a player to the rear of the queue
    void enqueue(Player p) {
        // Set default status if not set
        if (p.status == "") {
            p.status = "active";
        }
        
        Node* newNode = new Node(p);
        
        if (isEmpty()) {
            // If queue is empty, both front and rear point to new node
            front = rear = newNode;
        } else {
            // Add new node at rear and update rear pointer
            rear->next = newNode;
            rear = newNode;
        }
        size++;
        cout << "[Success] Player " << p.playerID << " (" << p.firstName << " " << p.lastName 
             << ") added to " << p.type << " queue!\n";
    }

    // Remove a player from the front of the queue
    void dequeue() {
        if (isEmpty()) {
            cout << "[Warning] Cannot dequeue from empty queue\n";
            return;
        }
        
        Node* temp = front;
        front = front->next;
        
        // If queue becomes empty, set rear to nullptr
        if (front == nullptr) {
            rear = nullptr;
        }
        
        delete temp;
        size--;
    }

    // Display all players in the queue
    void display() {
        if (isEmpty()) {
            cout << "Queue is empty\n";
            return;
        }
        
        Node* temp = front;
        int position = 1;
        cout << "Pos | Player ID | Name | Team | University | Status | Check-in\n";
        cout << "----|-----------|------|------|------------|--------|----------\n";
        
        while (temp != nullptr) {
            Player p = temp->data;
            cout << "[" << position << "] | " << p.playerID << " | " 
                 << p.firstName << " " << p.lastName << " | " 
                 << p.teamName << " | " << p.university << " | " 
                 << p.status << " | ";
            
            if (p.isCheckedIn) {
                cout << "✅ Checked In";
            } else {
                cout << "⏳ Not Checked In";
            }
            cout << endl;
            
            temp = temp->next;
            position++;
        }
    }

    // Check in a player using their ID
    bool checkInPlayer(string id) {
        Node* temp = front;
        while (temp != nullptr) {
            if (temp->data.playerID == id) {
                if (temp->data.status == "eliminated") {
                    cout << "[Failed] Cannot check-in eliminated player " << id << "\n";
                    return false;
                }
                temp->data.isCheckedIn = true;
                temp->data.status = "checked-in";
                cout << "[Success] Player " << id << " has checked in!\n";
                return true;
            }
            temp = temp->next;
        }
        return false;
    }

    // Withdraw player (change status to eliminated, don't delete)
    bool withdrawPlayer(string id) {
        Node* temp = front;
        while (temp != nullptr) {
            if (temp->data.playerID == id) {
                temp->data.status = "eliminated";
                temp->data.isCheckedIn = false;
                cout << "[Success] Player " << id << " has been withdrawn (status: eliminated)\n";
                return true;
            }
            temp = temp->next;
        }
        return false;
    }

    // Process next player in queue (find next active player)
    bool processNextPlayer() {
        if (isEmpty()) {
            cout << "[Info] Queue is empty, no player to process.\n";
            return false;
        }

        // Find next active/checked-in player
        Node* temp = front;
        while (temp != nullptr && temp->data.status == "eliminated") {
            temp = temp->next;
        }

        if (temp == nullptr) {
            cout << "[Info] No active players left in queue.\n";
            return false;
        }

        Player nextPlayer = temp->data;
        cout << "\n🎯 Processing Next Player:\n";
        cout << "Player ID: " << nextPlayer.playerID << "\n";
        cout << "Name: " << nextPlayer.firstName << " " << nextPlayer.lastName << "\n";
        cout << "Team: " << nextPlayer.teamName << "\n";
        cout << "Status: " << nextPlayer.status << "\n";
        cout << "Ready for match: " << (nextPlayer.isCheckedIn ? "Yes ✅" : "No ❌") << "\n";

        return true;
    }

    // Count players by status (for summary display)
    void showStatusSummary() {
        int active = 0, checkedIn = 0, eliminated = 0;
        Node* temp = front;
        
        while (temp != nullptr) {
            if (temp->data.status == "active") active++;
            else if (temp->data.status == "checked-in") checkedIn++;
            else if (temp->data.status == "eliminated") eliminated++;
            temp = temp->next;
        }
        
        cout << "📊 Queue Summary: Active=" << active 
             << ", Checked-in=" << checkedIn 
             << ", Eliminated=" << eliminated 
             << ", Total=" << size << "\n";
    }

    // Check if a team has enough players checked in (for match readiness)
    bool checkTeamReadiness(string teamID, int& totalMembers, int& checkedMembers) {
        totalMembers = 0;
        checkedMembers = 0;
        
        Node* temp = front;
        while (temp != nullptr) {
            if (temp->data.teamID == teamID && temp->data.status != "eliminated") {
                totalMembers++;
                if (temp->data.isCheckedIn) {
                    checkedMembers++;
                }
            }
            temp = temp->next;
        }
        
        return (totalMembers > 0 && checkedMembers >= totalMembers * 0.6);
    }

    // Get front player (peek without removing)
    Player getFrontPlayer() {
        if (!isEmpty()) {
            return front->data;
        }
        return Player(); // Return empty player if queue is empty
    }
};

#endif