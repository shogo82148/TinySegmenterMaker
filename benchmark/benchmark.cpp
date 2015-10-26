#include "tinysegmenter.hpp"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "tinysegmenter.hpp"

using namespace std;

int main() {
    ifstream ifs("timemachineu8j.txt");
    string str((istreambuf_iterator<char>(ifs)), istreambuf_iterator<char>());
    TinySegmenter segmenter;

    time_t start = time(NULL);
    for (int i = 0; i < 100; i++) {
        segmenter.segment(str);
    }
    time_t end = time(NULL);
    cout << (end - start);
}
