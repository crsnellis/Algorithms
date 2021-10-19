#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <time.h>

// create a .h file with templated versions of every sorting algorithm below
// include advanced and elementary sorts, be able to sort using the comparator
// included in the creation of it
/*
    quick_sort(data.begin(), data.end(), sz);
*/
class Sorting {
private:
    std::vector<int> data;
    int sz;
public:
    Sorting(int size) {
        sz = size;
        data.resize(sz);
        for (int i = 0; i < sz; ++i) {
            srand(i);
            data[i] = rand() % 100;
        }
    }

    void quicksort() {
        std::sort(data.begin(), data.end());
    }

    void introsort() {

    }

    void mergesort () {

    }

    void insertionsort() {

    }

    void shuffle() {
        int k, j;
        for (int i = 0; i < sz * 10; ++i) {
            srand(i);
            int k = rand() % sz;
            srand((i+1)^2);
            int j = rand() % sz;
            swap(k, j);
        }
    }

    void swap(int i, int j) {
        int temp = data[i];
        data[i] = data[j];
        data[j] = temp;
    }

    void sort() {
        std::cout << "quick sort--------------------------------------------\n";
        shuffle();
        quicksort();
        print_data();
        std::cout << "intro sort--------------------------------------------\n";
        shuffle();
        introsort();
        print_data();
        std::cout << "merge sort--------------------------------------------\n";
        shuffle();
        mergesort();
        print_data();
        std::cout << "insertion sort--------------------------------------------\n";
        shuffle();
        insertionsort();
        print_data();
    }

    bool check() {
        int previous = -1;
        for (int i : data) {
            if (i < previous) {
                return false;
            }
            else {
                previous = i;
            }
        }
        return true;
    }

    void print_data() {
        if (!check()) {
            std::cout << "**failed**\n";
        }
        for(int i : data) {
            std::cout << i << " ";
        }
        std::cout << std::endl;
    }
};

// g++ -std=c++11 sorting -o main.cpp <# elements to sort>
int main(int argc, char* argv[]) {
    std::cout << argv[1] << std::endl;
    Sorting s(std::atoi(argv[1]));
    s.sort();
    return 1;
}