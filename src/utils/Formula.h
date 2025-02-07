//
// Created by Lyndon on 2025/1/26.
//

#ifndef FORMULA_H
#define FORMULA_H

#include <cmath>

#define DOUBLECOMPARENEAR(a, b) std::fabs((a) - (b)) <= std::numeric_limits<double>::epsilon() * std::max(std::fabs((a)), std::fabs((b)))

#define POINTXYCOMPARENEAR(a, b) (DOUBLECOMPARENEAR((a[0]), (b[0])) && DOUBLECOMPARENEAR((a[1]), (b[1])))

#define POINTCOMPARENEAR(a, b) DOUBLECOMPARENEAR((a[0]), (b[0])) && DOUBLECOMPARENEAR((a[1]), (b[1])) && DOUBLECOMPARENEAR((a[2]), (b[2]))



class Formula {
public:
    static int bla(int arg1);
};



#endif //FORMULA_H
