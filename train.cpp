#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <cmath>
#include <algorithm>
#include <set>

int main(int argc, char** argv) {
    const char* instances_file = argv[1];
    const char* model_file = argv[2];

    //initialize
    std::vector<double> D;
    std::vector<std::string> features;
    std::vector<std::vector<unsigned int> > instances;
    std::vector<signed char> labels;
    unsigned int num_instances = 0;
    {
        std::set<std::string> S;
        std::ifstream ifinstances(instances_file);
        std::string line;
        while(ifinstances && getline(ifinstances, line)) {
            std::istringstream is(line);
            std::string h;
            int label;
            is >> label;
            while(is >> h) {
                S.insert(h);
            }
            ++num_instances;
            if(num_instances % 1000 == 0) {
                std::cerr << "finding instances...: " << num_instances << " instances found\r";
            }
        }
        std::cerr << "finding instances...: " << num_instances << " instances found\n";
        S.insert("");

        D.reserve(num_instances);
        labels.reserve(num_instances);
        instances.reserve(num_instances);
        features.reserve(S.size());
        for(std::set<std::string>::iterator it = S.begin(); it != S.end(); ++it) {
            features.push_back(*it);
        }
    }
    {
        std::ifstream ifinstances(instances_file);
        std::string line;
        while(ifinstances && getline(ifinstances, line)) {
            std::istringstream is(line);
            std::string h;
            std::vector<unsigned int> v;
            int label;
            is >> label;
            labels.push_back(label);
            while(is >> h) {
                std::vector<std::string>::iterator it = std::lower_bound(features.begin(), features.end(), h);
                v.push_back(it - features.begin());
            }
            std::sort(v.begin(), v.end());
            instances.push_back(v);
            D.push_back(1.0);
            if(D.size() % 1000 == 0)
                std::cerr << "loading instances...: " << D.size() << "/" << num_instances << " instances loaded\r";
        }
        std::cerr << std::endl;
    }
    const unsigned int num_features = features.size();
    std::vector<double> model(num_features), errors(num_features);
    unsigned int h_best = 0;
    double e_best = 0.5;
    double a = 0;
    double a_exp = 1;

    for(int t = 0; t < 100000; ++t) {
        // update & calculate errors
        double D_sum = 0.0;
        double D_sum_plus = 0.0;
        std::fill(errors.begin(), errors.end(), 0.0);
        for(unsigned int i = 0; i < num_instances; ++i) {
            const int label = labels[i];
            const std::vector<unsigned int> &hs = instances[i];
            const std::vector<unsigned int>::const_iterator it = std::lower_bound(hs.begin(), hs.end(), h_best);
            const int prediction = (it==hs.end() || *it != h_best) ? -1 : +1;
            if(label * prediction < 0) {
                D[i] *= a_exp;
            } else {
                D[i] /= a_exp;
            }
            D_sum += D[i];
            if(label > 0) D_sum_plus += D[i];
            const double d = D[i] * label;
            for(unsigned int j = 0; j < hs.size(); ++j) {
                errors[hs[j]] -= d;
            }
            i++;
        }

        // select best classifier
        e_best = D_sum_plus / D_sum;
        h_best = 0;
        for(unsigned int h = 1; h < num_features; ++h) {
            const double e = (errors[h] + D_sum_plus) / D_sum;
            if(std::abs(0.5-e) > std::abs(0.5-e_best)) {
                h_best = h;
                e_best = e;
            }
        }
        std::cerr << t << "\t" << std::abs(0.5-e_best) << "\r";
        if(std::abs(0.5-e_best)<0.0001) break;
        if(e_best<1e-10) e_best = 1e-10;
        if(e_best>1-1e-10) e_best = 1-1e-10;

        // update model
        a = 0.5 * std::log((1-e_best)/e_best);
        a_exp = std::exp(a);
        model[h_best] += a;

        // normalize
        for(unsigned int i = 0; i < num_instances; ++i) {
            D[i] /= D_sum;
        }
    }
    std::cerr << std::endl;

    //ouput
    std::ofstream ofmodel(model_file);
    double bias = -model[0] * 2;
    for(unsigned int h = 1; h < num_features; ++h) {
        const double a = model[h];
        if(a == 0.0) continue;
        ofmodel << features[h] << "\t" << (a*2) << std::endl;
        bias -= a;
    }
    ofmodel << bias << std::endl;
}
