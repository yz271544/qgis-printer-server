//
// Created by etl on 24-12-18.
//

#include "starterregister.h"


#include <vector>
#include <algorithm>
#include "starter.h"

StarterRegister* StarterRegister::instance = nullptr;

// 比较函数，用于排序Starters
bool cmp(Starter* a, Starter* b) {
    // 先判断是否有StartBlocking为true的情况，将其往后放
    if (a->StartBlocking() &&!b->StartBlocking()) {
        return false;
    }
    if (!a->StartBlocking() && b->StartBlocking()) {
        return true;
    }
    // 如果都为StartBlocking为true或者都为false，再按照之前的优先级规则比较
    if (a->PriorityGroup() > b->PriorityGroup()) {
        return true;
    }
    if (a->PriorityGroup() == b->PriorityGroup() && a->Priority() > b->Priority()) {
        return true;
    }
    return false;
}

// 对Starters进行排序
std::vector<Starter*> SortStarters() {
    std::vector<Starter*> starters = StarterRegister::getInstance()->AllStarters();
    std::sort(starters.begin(), starters.end(), cmp);
    return starters;
}

std::vector<Starter*> StarterRegister::AllStarters() {
    std::vector<Starter*> starters;
    starters.reserve(nonBlockingStarters.size() + blockingStarters.size());
    starters.insert(starters.end(), nonBlockingStarters.begin(), nonBlockingStarters.end());
    starters.insert(starters.end(), blockingStarters.begin(), blockingStarters.end());
    return starters;
}

void StarterRegister::Register(Starter* starter) {
    if (starter->StartBlocking()) {
        blockingStarters.push_back(starter);
    } else {
        nonBlockingStarters.push_back(starter);
    }
    allStarters[starter->GetName()] = starter;
}

Starter* StarterRegister::get_starter(const std::string& name) {
    return allStarters[name];
}
