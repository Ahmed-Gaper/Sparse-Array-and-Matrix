//each row is represented by a SparseArray , The SparseMatrix "has a" collection of SparseArray objects.
#ifndef SPARSE_MATRIX_H
#define SPARSE_MATRIX_H

#include "SparseArray.h"
#include <stdexcept>
#include <iostream>

template <typename T>
class SparseMatrix {
private:
    int rows_;
    int cols_;
    SparseArray<T>** data;

public:
   
    SparseMatrix(int rows, int cols) : rows_(rows), cols_(cols) {
        if (rows <= 0 || cols <= 0)
            throw std::invalid_argument("Matrix dimensions must be positive.");
        data = new SparseArray<T>*[rows];
        for (int i = 0; i < rows; i++) {
            data[i] = new SparseArray<T>(cols);
        }
    }

    ~SparseMatrix() {
        for (int i = 0; i < rows_; i++) {
            delete data[i];
        }
        delete[] data;
    }

    SparseMatrix(const SparseMatrix& other)
        : rows_(other.rows_), cols_(other.cols_) {
        data = new SparseArray<T>*[rows_];
        for (int i = 0; i < rows_; i++) {
            data[i] = new SparseArray<T>(*(other.data[i]));
        }
    }

    SparseMatrix& operator=(const SparseMatrix& other) {
        if (this != &other) {
            if (rows_ != other.rows_ || cols_ != other.cols_)
                throw std::invalid_argument("Matrix dimension mismatch in assignment");
            for (int i = 0; i < rows_; i++) {
                *(data[i]) = *(other.data[i]);
            }
        }
        return *this;
    }

    SparseMatrix(SparseMatrix&& other) noexcept
        : rows_(other.rows_), cols_(other.cols_), data(other.data) {
        other.data = nullptr;
        other.rows_ = 0;
        other.cols_ = 0;
    }

    SparseMatrix& operator=(SparseMatrix&& other) noexcept {
        if (this != &other) {
            for (int i = 0; i < rows_; i++) {
                delete data[i];
            }
            delete[] data;

            rows_ = other.rows_;
            cols_ = other.cols_;
            data = other.data;

            other.data = nullptr;
            other.rows_ = 0;
            other.cols_ = 0;
        }
        return *this;
    }

    class RowProxy {
    private:
        SparseArray<T>* rowData;
        int cols_;
    public:
        RowProxy(SparseArray<T>* rowData, int cols)
            : rowData(rowData), cols_(cols) {}

        typename SparseArray<T>::Proxy operator[](int col) {
            if (col < 0 || col >= cols_)
                throw std::out_of_range("Column index out of bounds");
            return (*rowData)[col];
        }

        T operator[](int col) const {
            if (col < 0 || col >= cols_)
                throw std::out_of_range("Column index out of bounds");
            return rowData->get(col);
        }
    };

    RowProxy operator[](int row) {
        if (row < 0 || row >= rows_)
            throw std::out_of_range("Row index out of bounds");
        return RowProxy(data[row], cols_);
    }

    const RowProxy operator[](int row) const {
        if (row < 0 || row >= rows_)
            throw std::out_of_range("Row index out of bounds");
        return RowProxy(data[row], cols_);
    }

    void print(std::ostream& os = std::cout) const {
        for (int i = 0; i < rows_; i++) {
            data[i]->print(os);
        }
    }
};

#endif
