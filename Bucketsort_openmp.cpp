#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <omp.h>
#include <ctime>
#include <cstdlib>

using namespace std;

// Bucket sort function prototype
void bucketSort(unsigned int* array, int SIZE, int num_threads);

int main(int argc, char* argv[]) {
    int SIZE;
    int num_threads = 4; //  Number of threads to use

    // Reading size and elements from file
    ifstream fd("testing_ingo.txt");
    fd >> SIZE;
    unsigned int *array = new unsigned int[SIZE];
    for (int i = 0; i < SIZE; i++) {
        fd >> array[i];
    }
    fd.close();


    double starttime = omp_get_wtime();
    bucketSort(array, SIZE, num_threads);
    double endtime = omp_get_wtime();
    ofstream fr("results.txt");
    for (int i = 0; i < SIZE; i++) {
        fr << array[i] << endl;
    }
    fr.close();


    cout << "It took " << (endtime - starttime) << " seconds." << endl;

    delete[] array;

    return 0;
}

void bucketSort(unsigned int* array, int SIZE, int num_threads) {
    unsigned int maxVal = *max_element(array, array + SIZE);
    unsigned int minVal = *min_element(array, array + SIZE);
    unsigned int range = maxVal - minVal + 1;
    vector<vector<unsigned int>> buckets(num_threads);

    // Determine bucket range
    unsigned int bucketRange = range / num_threads + 1;

    #pragma omp parallel num_threads(num_threads)
    {
        int thread_id = omp_get_thread_num();
        // Each thread works on its bucket
        #pragma omp for
        for (int i = 0; i < SIZE; i++) {
            int bucketIndex = (array[i] - minVal) / bucketRange;
            #pragma omp critical
            buckets[bucketIndex].push_back(array[i]);
        }
    }

    // Sort individual buckets and concatenate them
    int index = 0;
    for (int i = 0; i < num_threads; i++) {
        sort(buckets[i].begin(), buckets[i].end());
        for (unsigned int j = 0; j < buckets[i].size(); j++) {
            array[index++] = buckets[i][j];
        }
    }
}
