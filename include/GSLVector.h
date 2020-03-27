#pragma once
#include <gsl/gsl_vector.h>
#include <initializer_list>
#include <iostream>

class GSLVector {
    gsl_vector* data;

public:
    template<typename ArrayType>
    GSLVector(const ArrayType& array) : data(gsl_vector_alloc(array.size())) {
        size_t i = 0;
        for(double e : array) gsl_vector_set(data, i++, e);
    }

    GSLVector(gsl_vector* vec) : data(vec) {
        vec = nullptr;
    }

    GSLVector(const gsl_vector* vec) : data(gsl_vector_alloc(vec->size)) {
        for(size_t i = 0; i < vec->size; ++i) set(i, gsl_vector_get(vec, i));
    }

    GSLVector(const GSLVector& other) {
        GSLVector(other.data);
    }

    ~GSLVector() {
        if(data) gsl_vector_free(data);
    }

    void set(size_t i, double v) {
        gsl_vector_set(data, i, v);
    }

    double get(size_t i) const {
        return gsl_vector_get(data, i);
    }

    gsl_vector* getPointer() const {
        return data;
    }

    gsl_vector* moveToPointer() {
        auto tmp = data;
        data = nullptr;
        return tmp;
    }

    void print() const {
        for(size_t i = 0; i < data->size; ++i) std::cout << get(i) << ", ";
        std::cout << '\n';
    }
};