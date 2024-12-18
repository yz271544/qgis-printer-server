//
// Created by etl on 24-12-18.
//

#ifndef STARTERREGISTER_H
#define STARTERREGISTER_H

#include "starterregister.h"

#include <vector>
#include <unordered_map>
#include "starter.h"

class StarterRegister {
private:
    std::vector<Starter*> nonBlockingStarters;
    std::vector<Starter*> blockingStarters;
    std::unordered_map<std::string, Starter*> allStarters;
    static StarterRegister* instance;
    StarterRegister() {}

public:
    static StarterRegister* getInstance() {
        if (instance == nullptr) {
            instance = new StarterRegister();
        }
        return instance;
    }

    std::vector<Starter*> AllStarters() {
        std::vector<Starter*> starters;
        starters.reserve(nonBlockingStarters.size() + blockingStarters.size());
        starters.insert(starters.end(), nonBlockingStarters.begin(), nonBlockingStarters.end());
        starters.insert(starters.end(), blockingStarters.begin(), blockingStarters.end());
        return starters;
    }

    void Register(Starter* starter) {
        if (starter->StartBlocking()) {
            blockingStarters.push_back(starter);
        } else {
            nonBlockingStarters.push_back(starter);
        }
        allStarters[starter->GetName()] = starter;
    }

    Starter* get_starter(const std::string& name) {
        return allStarters[name];
    }
};

std::vector<Starter*> SortStarters();

#endif //STARTERREGISTER_H
