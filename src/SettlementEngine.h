#ifndef SETTLEMENT_ENGINE_H
#define SETTLEMENT_ENGINE_H

#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <bitset>
#include <cmath>
#include <algorithm>
#include <functional> // For std::hash

using namespace std;

// TYPES
typedef bitset<16> ModeSet;

struct Bank {
    string name;
    long balance;
    ModeSet optimizedModes;
    int shardId;
};

// LOGIC

// 1. Sharding Logic
inline int getBankShard(const string& name) {
    size_t hashVal = hash<string>{}(name);
    return hashVal % 4;
}

// 2. Optimized Solver
inline int optimizedSolver(int numBanks, vector<Bank> banks) {
    int txCount = 0;
    priority_queue<pair<long, int>> creditors;
    priority_queue<pair<long, int>, vector<pair<long, int>>, greater<pair<long, int>>> debtors;

    for(int i=0; i<numBanks; ++i) {
        if(banks[i].balance > 0) creditors.push({banks[i].balance, i});
        else if(banks[i].balance < 0) debtors.push({banks[i].balance, i});
    }

    while(!creditors.empty() && !debtors.empty()) {
        auto cred = creditors.top(); creditors.pop();
        auto debt = debtors.top(); debtors.pop();

        long amount = min(cred.first, abs(debt.first));
        
        // Check if RBI (Index 0) is involved OR if they share a mode
        bool isRBI = (cred.second == 0 || debt.second == 0);
        bool sharedMode = (banks[cred.second].optimizedModes & banks[debt.second].optimizedModes).any();
        
        if(!isRBI && !sharedMode) {
            txCount++; // Penalty op for routing via RBI
        }
        txCount++; // The actual settlement

        cred.first -= amount;
        debt.first += amount;

        if(cred.first > 0) creditors.push(cred);
        if(debt.first < 0) debtors.push(debt);
    }
    return txCount;
}

#endif