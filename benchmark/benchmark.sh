#!/bin/sh

set -xe

# cpp
cp ../tinysegmenter.hpp ./tinysegmenter.hpp
g++ -o benchmark benchmark.cpp
./benchmark

# perl
cp ../tinysegmenter.pm ./tinysegmenter.pm
perl -I. benchmark.pl

# node
cp ../tinysegmenter.js ./tinysegmenter.js
node --version
node benchmark.js

# go
mkdir -p tinysegmenter
cp ../tinysegmenter.go tinysegmenter
go version
go run benchmark.go

# python
cp ../tinysegmenter.py ./tinysegmenter.py
python --version
python benchmark.py
