#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <cmath>

typedef std::map<std::string, double> MODEL;

int main(int argc, char** argv) {
    const char* instances_file = argv[1];
    const char* model_file = argv[2];

    //initialize
    std::vector<double> D;
    std::vector<signed char> labels;
    MODEL model, errors;
    {
        std::ifstream instances(instances_file);
        std::string line;
        while(instances && getline(instances, line)) {
            std::istringstream is(line);
            std::string h;
            int label;
            is >> label;
            D.push_back(1.0);
            labels.push_back(label);
            while(is >> h) {
                model[h] = 0.0;
            }
        }
    }
    model[""] = 0.0;
    errors[""] = 0.0;
    const int num_instances = labels.size();

    for(int t = 0; t < 100; ++t) {
        // calculate errors
        double D_sum = 0;
        double D_sum_plus = 0;
        for(int i = 0; i < num_instances; ++i) {
            D_sum += D[i];
            if(labels[i] > 0) {
                D_sum_plus += D[i];
            }
        }
        D_sum_plus /= D_sum;
        for(MODEL::iterator it = model.begin(); it!=model.end(); ++it) {
            errors[it->first] = D_sum_plus;
        }
        {
            int i = 0;
            std::ifstream instances(instances_file);
            std::string line;
            while(instances && getline(instances, line)) {
                std::istringstream is(line);
                std::string h;
                int label;
                is >> label;
                D[i] /= D_sum;
                const double d = D[i] * label;
                while(is >> h) {
                    errors[h] -= d;
                }
                i++;
            }
        }

        // select best classifier
        std::string h_best = "";
        double e_best = D_sum_plus;
        for(MODEL::iterator it = errors.begin(); it!=errors.end(); ++it) {
            const std::string &h = it->first;
            const double e = it->second;
            if(std::abs(0.5-e) > std::abs(0.5-e_best)) {
                h_best = h;
                e_best = e;
            }
        }
        std::cerr << t << "\t" << std::abs(0.5-e_best) << std::endl;
        if(std::abs(0.5-e_best)<0.01) break;
        if(e_best<1e-10) e_best = 1e-10;
        if(e_best>1-1e-10) e_best = 1-1e-10;

        // update model
        {
            const double a = 0.5 * std::log((1-e_best)/e_best);
            const double a_exp = std::exp(a);
            int i = 0;
            std::ifstream instances(instances_file);
            std::string line;
            while(instances && getline(instances, line)) {
                std::istringstream is(line);
                std::string h;
                int label;
                int prediction = -1;
                is >> label;
                while(is >> h) {
                    if(h == h_best) {
                        prediction = +1;
                        break;
                    }
                }
                if(label * prediction < 0) {
                    D[i] *= a_exp;
                } else {
                    D[i] /= a_exp;
                }
                i++;
            }
            model[h_best] += a;
        }
    }

    //ouput
    std::ofstream ofmodel(model_file);
    double bias = -model[""];
    for(MODEL::iterator it = model.begin(); it!=model.end(); ++it) {
        const std::string &h = it->first;
        const double a = it->second;
        if(h == "") continue;
        if(a == 0.0) continue;
        ofmodel << h << "\t" << (a*2) << std::endl;
    }
    ofmodel << bias << std::endl;
}
