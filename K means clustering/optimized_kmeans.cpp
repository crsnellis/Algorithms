// this should be a header file that lets the user create k clusters 
// from a data set with n quantifiable categories

/*
    file format: csv
    Name D1 D2 D3 D3 D4 D5 DN
    ...
    ...
    ...
*/
// Project Identifier: 9504853406CBAC39EE89AA3AD238AA12CA198043
#include <iostream>
#include <stack>
#include <string>
#include <climits>
#include <queue>
#include <stdio.h>
#include "getopt.h"
#include "kmeans.h"

info getInfo(int argc, char* argv[]) {
    info r;
    int choice, option_index;
    const option long_opts[] = {
        {"normalize", no_argument, nullptr, 'n'},
        {"raw", no_argument, nullptr, 'r'},
    };

    while ((choice = getopt_long(argc, argv, "nr", long_opts,
        &option_index)) != -1) {
        switch (choice) {
            case 'n':
                r.show_normalized = true;
            case 'r':
                r.show_raw = true;
            default:
                std::cerr << "[ERROR] unknown command line input\n";
        }
    }
    return r;
}

int  main(int argc, char* argv[]) {
    std::ios_base::sync_with_stdio(false);
    info i = getInfo(argc, argv);
    Clusterer cluster = Clusterer(i);
    cluster.find_clusters();
}



