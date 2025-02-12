#include <iostream>
#include <cstdlib>
#include "SparseArray.h"
#include "SparseMatrix.h"

using namespace std;

// Utility function for a normal dense 2D array
size_t calculateMemoryUsageNormalArray(int rows, int cols) {
    return static_cast<size_t>(rows) * cols * sizeof(int);
}

// For the SparseMatrix, we approximate memory usage as follows:
// For each nonzero element, a node is assumed to hold:
// - an int for value,
// - an int for index,
// - plus two pointers (next and previous) as in SparseArray.
size_t calculateMemoryUsageSparseMatrix(const SparseMatrix<int>& matrix, int rows, int cols) {
    size_t nonZeroCount = 0;
    // Count nonzero elements by iterating through each cell.
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (matrix[i][j] != 0)
                nonZeroCount++;
        }
    }
    size_t nodeSize = sizeof(int) + sizeof(int) + 2 * sizeof(void*);
    return nonZeroCount * nodeSize;
}

int main() {
    //==============================
    // Test 1: SparseArray vs. Dense 1D Array Memory Usage
    //==============================
    cout << "======== SparseArray vs Dense 1D Array Memory Usage Comparison ========\n";

    const int capacity = 1000000;        // Total elements in the 1D arrays
    const int nonzero_frequency = 10000; // Nonzero inserted every 10,000 indices

    // Create a SparseArray.
    SparseArray<int> sparseArr(capacity);
    // Create a normal dynamic array (dense 1D array), zero-initialized.
    int* denseArr = new int[capacity]();
    
    // Populate both arrays with random nonzero values at defined intervals.
    for (int i = 0; i < capacity; i += nonzero_frequency) {
        int value = rand() % 100 + 1;
        sparseArr.set(i, value);
        denseArr[i] = value;
    }

    // Estimate memory usage for the dense array.
    size_t denseMemory = capacity * sizeof(int);
    // For the SparseArray, each nonzero node is assumed to use:
    // sizeof(int) for value + sizeof(int) for index + 2 * sizeof(void*) for pointers.
    size_t nodeSize = sizeof(int) + sizeof(int) + 2 * sizeof(void*);
    size_t sparseMemory = sparseArr.nonZeroCount() * nodeSize;

    cout << "Dense 1D array memory usage: " << denseMemory << " bytes\n";
    cout << "SparseArray memory usage: " << sparseMemory << " bytes\n";
    cout << "Memory saved: " << (denseMemory - sparseMemory) << " bytes\n\n";


    //==============================
    // Test 2: SparseMatrix vs. Normal 2D Array Memory Usage
    //==============================
    cout << "======== SparseMatrix vs Normal 2D Array Memory Usage Comparison ========\n";

    const int ROWS = 1000;
    const int COLS = 1000;
    const int SPARSITY = 100; // Only 1 in 100 elements is nonzero

    // Initialize a SparseMatrix.
    SparseMatrix<int> sparseMat(ROWS, COLS);

    // Populate the SparseMatrix with sparse data.
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if ((i * COLS + j) % SPARSITY == 0) {
                sparseMat[i][j] = i * COLS + j;
            }
        }
    }

    // Create a normal 2D array (dense matrix) and fill it with zeros.
    int** normalArray = new int*[ROWS];
    for (int i = 0; i < ROWS; i++) {
        normalArray[i] = new int[COLS](); // Zero-initialized
    }
    // Populate the normal 2D array with the same sparse data.
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if ((i * COLS + j) % SPARSITY == 0) {
                normalArray[i][j] = i * COLS + j;
            }
        }
    }

    // Calculate the memory usage for the normal 2D array and the SparseMatrix.
    size_t normalMemoryUsage = calculateMemoryUsageNormalArray(ROWS, COLS);
    size_t sparseMemoryUsage = calculateMemoryUsageSparseMatrix(sparseMat, ROWS, COLS);

    cout << "Normal 2D array memory usage: " << normalMemoryUsage << " bytes\n";
    cout << "SparseMatrix memory usage: " << sparseMemoryUsage << " bytes\n";
    cout << "Memory saved: " << (normalMemoryUsage - sparseMemoryUsage) << " bytes\n";

    // Clean up the normal 1D array.
    delete[] denseArr;

    // Clean up the normal 2D array.
    for (int i = 0; i < ROWS; i++) {
        delete[] normalArray[i];
    }
    delete[] normalArray;

    return 0;
}
