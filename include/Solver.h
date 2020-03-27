#pragma once
#include <iostream>
#include "GSLVector.h"
#include <gsl/gsl_multiroots.h>
#include <memory>

//TODO make CRTP to reach F directly
class SimpleMethod {
protected:
    using solverType = gsl_multiroot_fsolver;
    static void deleteSolver(solverType* ptr) { gsl_multiroot_fsolver_free(ptr); }
    solverType* allocateSolver(unsigned N) { return gsl_multiroot_fsolver_alloc(gsl_multiroot_fsolver_hybrids, N); }
    void setSolver(solverType* method, gsl_multiroot_function* F, gsl_vector* x_init) {
        gsl_multiroot_fsolver_set(method, F, x_init);
    }
    inline int iterate(solverType* method) { return gsl_multiroot_fsolver_iterate(method); }
    inline int test(solverType* method, double precision) { return gsl_multiroot_test_residual(method->f, precision); }
};

template<typename Equation, typename Parameters, typename Method = SimpleMethod>
class Solver : public Method {
    struct methodDeleter { void operator()(typename Method::solverType* ptr) { Method::deleteSolver(ptr); } };
    using arrayType = typename std::result_of<Equation(const gsl_vector*, Parameters*)>::type;

    static constexpr size_t N = arrayType().size();
    std::unique_ptr<typename Method::solverType, methodDeleter> method{Method::allocateSolver(N), methodDeleter{}};
    gsl_multiroot_function F;

    static int system(const gsl_vector* x, void* params, gsl_vector* f) {
        Parameters* p = reinterpret_cast<Parameters*>(params);
        std::array<double, N> result = Equation()(x, p);
        for(size_t i = 0; i < N; ++i) gsl_vector_set(f, i, result[i]);
        return GSL_SUCCESS;
    }

public:
    Solver() : Method() {
        F.f = system;
        F.n = N;
    }

    std::array<double, N> solve(std::array<double, N> initValues, Parameters& params, double precision = 1e-7) {
        F.params = &params;
        Method::setSolver(method.get(), &F, GSLVector{initValues}.getPointer());
        int status = 1;
        do {
            status = Method::iterate(method.get());
            if(status != GSL_SUCCESS) break;
            status = Method::test(method.get(), precision);
        } while(status == GSL_CONTINUE);
        std::array<double, N> ret;
        for(size_t i = 0; i < N; ++i) ret[i] = gsl_vector_get(method->x, i);
        return ret;
    }

};
