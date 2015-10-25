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

    auto start = time(NULL);
    for (int i = 0; i < 100; i++) {
        segmenter.segment(str);
    }
    auto end = time(NULL);
    cout << (end - start);
}
