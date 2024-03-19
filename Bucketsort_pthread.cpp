#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <limits>
#include <vector>

using namespace std;

struct listNode {
    float val;
    listNode *next;
};

int bucketSize;
vector<listNode*> buckets;
float maxVal = -numeric_limits<float>::infinity();
float minVal = numeric_limits<float>::infinity();
int threadCount = 2;

void* bucketSort(void *arg);
listNode* insertionSort(listNode *list);

int pthread_sort(int numOfElements, float *data) {
    // Finding max and min element
    for(int i = 0; i < numOfElements; i++) {
        if(maxVal < data[i]) maxVal = data[i];
        if(minVal > data[i]) minVal = data[i];
    }
    
    if (minVal == maxVal) return 0; // All elements are the same.

    bucketSize = ceil(float(numOfElements) / threadCount);
    buckets.resize(threadCount);

    // Initializing buckets
    for(int i = 0; i < threadCount; i++) {
        buckets[i] = NULL;
    }

    // Distributing elements into buckets
    float range = maxVal - minVal;
    for(int i = 0; i < numOfElements; i++) {
        int bucketIndex = int((data[i] - minVal) / range * (threadCount - 1));
        listNode* newNode = new listNode {data[i], buckets[bucketIndex]};
        buckets[bucketIndex] = newNode;
    }

    // Creating and joining threads
    pthread_t *threads = new pthread_t[threadCount];
    int *threadIDs = new int[threadCount];
    for (int i = 0; i < threadCount; i++) {
        threadIDs[i] = i;
        pthread_create(&threads[i], NULL, bucketSort, (void*)&threadIDs[i]);
    }

    for (int i = 0; i < threadCount; i++) {
        pthread_join(threads[i], NULL);
    }

    // Merging sorted buckets
    int index = 0;
    for (int i = 0; i < threadCount; i++) {
        listNode* node = buckets[i];
        while (node != NULL) {
            data[index++] = node->val;
            listNode* prev = node;
            node = node->next;
            delete prev;
        }
    }

    delete[] threads;
    delete[] threadIDs;
    
    return 0;
}

void* bucketSort(void *arg) {
    int threadIndex = *(int*)arg;
    int start = threadIndex * bucketSize;
    int end = (threadIndex + 1) * bucketSize;

    for (int i = start; i < end && i < buckets.size(); i++) {
        if (buckets[i] != NULL) {
            buckets[i] = insertionSort(buckets[i]);
        }
    }

    return NULL;
}

listNode* insertionSort(listNode *head) {
    if (!head) return NULL;

    listNode *sorted = NULL;

    listNode *current = head;
    while (current) {
        listNode *next = current->next;

        if (!sorted || sorted->val >= current->val) {
            current->next = sorted;
            sorted = current;
        } else {
            listNode *temp = sorted;
            while (temp) {
                if (!temp->next || temp->next->val >= current->val) {
                    current->next = temp->next;
                    temp->next = current;
                    break;
                }
                temp = temp->next;
            }
        }
        current = next;
    }
    
    return sorted;
}

// Use this main function for testing the pthread_sort function.
int main() {
    int numOfElements = 100; // For example
    float* data = new float[numOfElements];

    // Fill the data array with random numbers for testing
    for (int i = 0; i < numOfElements; ++i) {
        data[i] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/1000.0));
    }

    // Call the pthread_sort function
    pthread_sort(numOfElements, data);

    // Output the sorted data
    for (int i = 0; i < numOfElements; ++i) {
        printf("%.2f ", data[i]);
    }
    printf("\n");

    delete[] data;

    return 0;
}
