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
    int *numberTotal = NULL;
    int *countTotal = NULL;
    double means[4];
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
        if (length < 4 || length > 32) {
            fprintf(stderr, "Error: the length of file must be between 4 and 32.");
            return 1;
        }
        if (length < size) {
            fprintf(stderr, "Error: less values than processors.");
            return 1;
        }
        if (length > size) length = size;
        fileIn.seekg(0, std::ios::beg);
        global = (uint8_t*)malloc(length * sizeof(uint8_t));
        fileIn.read(reinterpret_cast<char*>(global), length);
        for (int i = 0; i < 4; i++) means[i] = (double)global[i];
    }

    MPI_Scatter(global, 1, MPI_UNSIGNED_CHAR, 
                &currentNum, 1, MPI_UNSIGNED_CHAR, 
                0, MPI_COMM_WORLD);

    while (change == true) {
        // broadcast new values of means
        MPI_Bcast(means, 4, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        memset(number, 0, 4*sizeof(int));
        memset(count, 0, 4*sizeof(int));
        best = 0;
        double minDistance = 1000.0;
        // calculate the nearest cluster
        for (int i = 0; i < 4; i++) {
            if (abs((double)currentNum - means[i]) <= minDistance) {
                minDistance = abs((double)currentNum - means[i]);
                best = i;
            }
        }
        // set the corresponding values in arrays (index = cluster)
        number[best] = currentNum;
        count[best] = 1;
        // get the division to clusters
        if (rank == 0) {
            numberTotal = (int*)malloc(4*sizeof(int));
            countTotal = (int*)malloc(4*sizeof(int));
        }
        MPI_Reduce(&number, numberTotal, 4, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        MPI_Reduce(&count, countTotal, 4, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        // calculate new means
        if (rank == 0) {
            double newMean = 0.0;
            change = false;
            for (int i = 0; i < 4; i++) {
                if (countTotal[i] != 0) {
                    newMean = (double)numberTotal[i] / countTotal[i];
                    if (means[i] != newMean) {
                        change = true;
                        means[i] = newMean;
                    }
                }
            }
        }
        MPI_Bcast(&change, 1, MPI_CXX_BOOL, 0, MPI_COMM_WORLD);
    }

    if (rank == 0) {
        std::vector<uint8_t> Afinal, Bfinal, Cfinal, Dfinal;
        for (int numIndex = 0; numIndex < size; numIndex++) {
            best = 0;
            double minDistance = 1000.0;
            for (int i = 0; i < 4; i++) {
                if (abs((double)global[numIndex] - means[i]) <= minDistance) {
                    minDistance = abs((double)global[numIndex] - means[i]);
                    best = i;
                }
            }
            switch (best) {
            case 0:
                Afinal.push_back(global[numIndex]);
                break;
            case 1:
                Bfinal.push_back(global[numIndex]);
                break;
            case 2:
                Cfinal.push_back(global[numIndex]);
                break;
            case 3:
                Dfinal.push_back(global[numIndex]);
                break;
            default:
                break;
            }
        }
        free(global);
        printf("[%.3f] ", means[0]);
        for (auto &num : Afinal)
        cout << (int)num << " ";
        cout << endl;
        printf("[%.3f] ", means[1]);
        for (auto &num : Bfinal)
        cout << (int)num << " ";
        cout << endl;
        printf("[%.3f] ", means[2]);
        for (auto &num : Cfinal)
        cout << (int)num << " ";
        cout << endl;
        printf("[%.3f] ", means[3]);
        for (auto &num : Dfinal)
        cout << (int)num << " ";
        cout << endl;
    }
    MPI_Finalize();
    return 0;
}