#include <iostream>
#include "Solver.h"
#include "GSLVector.h"

struct Parameters {
    double a, b, c;
};

struct Equation {
    std::array<double, 2> operator()(GSLVector x_in, Parameters* p) {
        double x0 = x_in.get(0), x1 = x_in.get(1);
        std::array<double, 2> tmp{x0*x1 - p->a, x0*p->b  + x1*p->c};
        return tmp;
    }
};

int main(int argc, char const *argv[]) {
    Parameters p{-1.0, 2.0, 3.0};
    Solver<Equation, Parameters> s;
    auto ret = s.solve({1.5, 2.0}, p);
    
    for(auto e : ret) std::cout << e << ", ";
    std::cout << '\n'; 
}