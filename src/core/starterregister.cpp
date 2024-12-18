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
