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
    StarterRegister() = default;

public:
    static StarterRegister* getInstance() {
        if (instance == nullptr) {
            instance = new StarterRegister();
            instance->nonBlockingStarters = std::vector<Starter*>();
            instance->blockingStarters = std::vector<Starter*>();
            instance->allStarters = std::unordered_map<std::string, Starter*>();
        }
        return instance;
    }

    std::vector<Starter*> AllStarters();

    void Register(Starter* starter);

    Starter* get_starter(const std::string& name);
};

std::vector<Starter*> SortStarters();

#endif //STARTERREGISTER_H
