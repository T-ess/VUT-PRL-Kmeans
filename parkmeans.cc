#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <mpi.h>

using std::cout;
using std::endl;

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    uint8_t *global = NULL;
    float *means = NULL;
    uint8_t currentNum = 0;
    int rank, size, number[4], count[4], best;
    bool change = true;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // read numbers from file
    if (rank == 0) {
        uint8_t num;
        std::ifstream fileIn ("numbers", std::ios::binary);
        fileIn.seekg(0, std::ios::end);
        size_t length = fileIn.tellg();
        if (length < size) {
            fprintf(stderr, "Error: less values than processors.");
            return 1;
        }
        if (length > size) length = size;
        fileIn.seekg(0, std::ios::beg);
        global = (uint8_t*)malloc(length * sizeof(uint8_t));
        fileIn.read(reinterpret_cast<char*>(global), length);
        for (int i = 0; i < 4; i++) means[i] = (float)global[i];
    }

    MPI_Scatter(global, 1, MPI_UNSIGNED_CHAR, 
                &currentNum, 1, MPI_UNSIGNED_CHAR, 
                0, MPI_COMM_WORLD);

    // while (change == true) {
    //     // broadcast new values of means
    //     MPI_Bcast(means, 4, MPI_FLOAT, 0, MPI_COMM_WORLD);
    //     memset(number, 0, 4*sizeof(int));
    //     memset(count, 0, 4*sizeof(int));
    //     best = 0;
    //     float minDistance = 1000.0;
    //     // calculate the nearest cluster
    //     for (int i = 0; i < 4; i++) {
    //         if (abs(currentNum - means[i]) < minDistance) {
    //             minDistance = abs(currentNum - means[i]);
    //             best = i;
    //         }
    //     }
    //     // set the corresponding values in arrays (index = cluster)
    //     number[best] = currentNum;
    //     count[best] = 1;

    //     // get the division to clusters
    //     int *numberTotal = NULL;
    //     int *countTotal = NULL;
    //     if (rank == 0) {
    //         numberTotal = (int*)malloc(4*sizeof(int));
    //         countTotal = (int*)malloc(4*sizeof(int));
    //     }
    //     MPI_Reduce(&number, &numberTotal, 4, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    //     MPI_Reduce(&count, &countTotal, 4, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    //     // calculate new means
    //     if (rank == 0) {
    //         float newMean = 0.0;
    //         change = false;
    //         for (int i = 0; i < 4; i++) {
    //             newMean = numberTotal[i] / countTotal[i];
    //             if (means[i] != newMean) {
    //                 change = true;
    //                 means[i] = newMean;
    //             }
    //         }
    //     }
    // }

    // cout << currentNum << best << endl;
    MPI_Finalize();
    return 0;
}