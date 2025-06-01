#ifndef NODE_H
#define NODE_H

#include "player.hpp"

struct Node {
    Player data;    // Player information stored in this node
    Node* next;     // Pointer to the next node in the linked list
    
    // Default constructor
    Node() {
        next = nullptr;
    }
    
    // Constructor with player data
    Node(Player playerData) {
        data = playerData;
        next = nullptr;
    }
    
    // Constructor with player data and next pointer
    Node(Player playerData, Node* nextNode) {
        data = playerData;
        next = nextNode;
    }
};

#endif