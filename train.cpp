#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <cmath>
#include <algorithm>
#include <set>
#include <cstdlib>
#include <unistd.h>

class AdaBoost {
private:
    std::vector<double> D;
    std::vector<double> model;
    std::vector<std::string> features;
    std::vector<std::vector<unsigned int> > instances;
    std::vector<signed char> labels;
    unsigned int num_instances;

public:
    double threshold;
    unsigned int numIteration;

    AdaBoost() {
        threshold = 0.01;
        numIteration = 100;
    }

    void initializeFeatures(const char* instances_file) {
        std::set<std::string> S;
        std::ifstream ifinstances(instances_file);
        std::string line;
        num_instances = 0;
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
        model.reserve(S.size());
        for(std::set<std::string>::iterator it = S.begin(); it != S.end(); ++it) {
            features.push_back(*it);
            model.push_back(0.0);
        }
    }

    void initializeInstances(const char* instances_file) {
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

    void train() {
        const unsigned int num_features = features.size();
        std::vector<double> errors(num_features);
        unsigned int h_best = 0;
        double e_best = 0.5;
        double a = 0;
        double a_exp = 1;

        for(int t = 0; t < numIteration; ++t) {
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
            if(std::abs(0.5-e_best)<threshold) break;
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
    }

    void saveModel(const char* model_file) const {
        const unsigned int num_features = features.size();
        std::ofstream ofmodel(model_file);
        double bias = -model[0];
        for(unsigned int h = 1; h < num_features; ++h) {
            const double a = model[h];
            if(a == 0.0) continue;
            ofmodel << features[h] << "\t" << (a*2) << std::endl;
            bias -= a;
        }
        ofmodel << bias << std::endl;
    }

    double getBias() const {
        double bias = 0.0;
        const unsigned int num_features = features.size();
        for(unsigned int h = 0; h < num_features; ++h) {
            bias -= model[h];
        }
        return bias;
    }

    void showResult() const {
        const double bias = getBias();
        const unsigned int num_features = features.size();
        std::cerr << "Result:" << std::endl;
        unsigned int pp = 0, pn = 0, np = 0, nn = 0;
        for(unsigned int i = 0; i < num_instances; ++i) {
            const int label = labels[i];
            const std::vector<unsigned int> &hs = instances[i];
            double score = bias;
            for(unsigned int j = 0; j < hs.size(); ++j) {
                score += model[hs[j]];
            }
            if(score >= 0) {
                if(label > 0) {
                    ++pp;
                } else {
                    ++pn;
                }
            } else {
                if(label > 0) {
                    ++np;
                } else {
                    ++nn;
                }
            }
        }
        std::cerr << "Accuracy: " << static_cast<double>(pp+nn)/num_instances*100 << "% (" << (pp+nn) << "/" << num_instances << ")" << std::endl;
        std::cerr << "Precision: " << static_cast<double>(pp)/(pp+pn)*100 << "% (" << pp << "/" << (pp+pn) << ")" << std::endl;
        std::cerr << "Recall: " << static_cast<double>(pp)/(pp+np)*100 << "% (" << pp << "/" << (pp+np) << ")" << std::endl;
        std::cerr << "System/Answer p/p p/n n/p n/n: " << pp << " " << pn << " " << np << " " << nn << std::endl;
    }
};

int main(int argc, char** argv) {
    // Parse arg
    int c;
    AdaBoost t;
    while((c=getopt(argc, argv, "t:n:"))!=-1) {
        switch (c) {
        case 't':
            t.threshold = std::atof(optarg);
            break;
        case 'n':
            t.numIteration = std::atoi(optarg);
            break;
        }
    }

    if(optind+1>=argc) {
        std::cerr << "Usage: " << argv[0] << " [-t threshold] [-n number-of-iteration] instances_file model_file" << std::endl;
        return -1;
    }

    const char* instances_file = argv[optind];
    const char* model_file = argv[optind+1];

    t.initializeFeatures(instances_file);
    t.initializeInstances(instances_file);
    t.train();
    t.saveModel(model_file);
    t.showResult();
}
