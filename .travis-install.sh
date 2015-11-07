#!/bin/sh

set -ex

sudo add-apt-repository -y ppa:staticfloat/juliareleases
sudo add-apt-repository -y ppa:staticfloat/julia-deps
sudo apt-get update -y
sudo apt-get install -y julia
