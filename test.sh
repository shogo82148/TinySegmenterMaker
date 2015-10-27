#!/bin/sh

set -xe

./extract < test/corpus.txt > features.txt
g++ -O3 -o train train.cpp
./train -t 0.001 -n 10000 features.txt model
./segment model < test/input.txt | tee output.txt
diff output.txt test/corpus.txt

./maker all < RWCP.model
