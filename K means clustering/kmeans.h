#ifndef KMEANS
#define KMEANS

#include <iostream>
#include <vector>
#include <cmath>
#include <utility> 
#include <float.h>

struct info {
    // Data information, from input
    int centroids;
    int rows;

    // Output information, from the command line
    bool show_raw = false;
    bool show_normalized = false;
};

struct pair {
    std::string name;
    size_t centroid;
    std::vector<double> raw;
    std::vector<double> normalized;
};

struct minmax {
    double min;
    double max;
};

class Clusterer {
    private:
        // data structures
        std::vector<pair> data;
        std::vector<minmax> mnmx;
        std::vector<std::vector<double>> current_centroids;
        std::vector<std::vector<double>> old_centroids;
        info args;

        // constants
        const double EPSILON = 1.0e-10;

        // clustering information
        size_t num_centroids;
        size_t num_rows;
        size_t num_categories;

        // create function headers
        void find_clusters();
        bool checkConvergence();
        void assign_clusters();
        void find_new_clusters();
        void print_results();

    public:
        Clusterer (info info) {
            args = info;
            std::cin >> num_centroids >> num_rows >> num_categories;

            data.reserve(num_rows);
            mnmx.reserve(num_categories);
            current_centroids.reserve(num_centroids);
            old_centroids.reserve(num_centroids);
        }

        void find_clusters() {
            bool first_step = true;
            // normalize
            find_min_max();
            raw_to_normalized();
            create_starting_centroids();
            while (checkConvergence() || first_step) {
                first_step = false;
                // new becomes old;
                old_centroids = current_centroids; // is there a better way to do this?
                // assign new clusters
                assign_clusters();
                // find new centroids
                find_new_centroids();
            }
            normalized_to_raw();
            print_results();
        }

    private:
        // will need a normalize function, just like most other functions in the old Data class
        bool checkConvergence() {
            double diff = 0.0;
            
            // Check for invalid cluster sizes. Safety first?
            if (current_centroids.size() != old_centroids.size() 
                || current_centroids[0].size() != old_centroids[0].size()) {
                std::cerr << "[ERROR] invalid centroid vector sizes";
            }

            size_t centroids = old_centroids.size(), sz = old_centroids[0].size();
            for (size_t current; current < centroids; ++current) {
                double square_sum = 0.0;
                for (size_t index; index < sz; ++index) {
                    square_sum += pow(old_centroids[current][index] +
                        current_centroids[current][index], 2);
                }
                diff += sqrt(square_sum);
            }

            return diff > EPSILON;
        }

        // UNRELATED question: why does modern AR look so shitty and what can be done about it

        /*
            for each row of data, find the distance between it and every centroid, 
            keep track of the minimum didstance and the closest centroid, this is
            stored at index 0
        */

        void raw_to_normalized() {
            // fill the normalize vector
            for (size_t category = 0; category < num_categories; ++category) {
                for (size_t row = 0; row < num_rows; ++row) {
                    data[row].normalized[category] = (mnmx[category].max - mnmx[category].min == 0) 
                        ? 0 : data[row].raw[category] / (mnmx[category].max - mnmx[category].min);
                }
            }
        }

        void normalized_to_raw() {
            // fill the normalize vector
            for (size_t category = 0; category < num_categories; ++category) {
                for (size_t row = 0; row < num_rows; ++row) {
                    data[row].raw[category] = 
                        data[row].normalized[category] * (mnmx[category].max - mnmx[category].min);
                }
            }
       }

        void find_min_max() {
            double min, max;
            for (size_t category = 0; category < num_categories; ++category) {
                min = DBL_MAX;
                max = -DBL_MAX;
                minmax mmx {min, max};
                for (size_t row = 0; row < num_rows; ++row) {
                    if (mmx.min > data[row].raw[category])
                        mmx.min = data[row].raw[category];
                    if (mmx.max < data[row].raw[category]) {
                        mmx.max = data[row].raw[category];
                    }
                }
                mnmx[category] = mmx;
            }
        }

        void create_starting_centroids() {
            for (size_t centroid = 0; centroid < num_centroids; ++centroid) {
                for (size_t category; category < num_categories; ++category) {
                    current_centroids[category][centroid] = centroid;
                }
            }
        }

        double find_distance (size_t row, size_t centroid) {
            double square_sum = 0.0;
            for (size_t index = 0; index < row; ++index) {
                square_sum += pow(data[row].normalized[index], 2);
            }
            return sqrt(square_sum);
        }

        void assign_clusters() {
            for (size_t row = 0; row < num_rows; ++row) {
                // tertiary
                double shortest_distance = DBL_MAX;
                size_t index = 0;
                double distance;
                // find shortest distance
                for (size_t centroid; centroid < num_centroids; ++centroid) {
                    distance = find_distance(row, centroid);
                    shortest_distance = 
                        (shortest_distance > distance) ? shortest_distance : distance;
                    if (centroid == num_centroids - 1) {
                        data[row].centroid = index;
                    }
                }
            }
        }

        void find_new_centroids() {
            
        }

        void print_results() {
            if (args.show_raw) {
                std::cout << "The raw data:\n";
                // print the data names, also need a way to store data names

                // print each row, from 0 to num_rows
                for (size_t index = 0; index < size_t(num_rows); ++index) {
                    for (size_t centroid = 0; centroid < num_centroids; ++centroid) { // this is not a centroid
                        std::cout << data[centroid].raw[index] << " "; 
                    }
                    std::cout << "\n";
                }
            }
            else if (args.show_normalized) {
                std::cout << "The normalized data:\n";
            }
        }
};

#endif