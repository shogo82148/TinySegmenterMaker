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
            while(is >> h) {
                model[h] = 0.0;
            }
        }
    }
    model[""] = 0.0;
    errors[""] = 0.0;
    const int num_instances = D.size();
    std::string h_best = "";
    double e_best = 0.5;
    double a = 0;
    double a_exp = 1;

    for(int t = 0; t < 100; ++t) {
        // update & calculate errors
        double D_sum = 0;
        double D_sum_plus = 0;

        for(MODEL::iterator it = model.begin(); it!=model.end(); ++it) {
            errors[it->first] = 0;
        }
        {
            int i = 0;
            std::ifstream instances(instances_file);
            std::string line;
            while(instances && getline(instances, line)) {
                std::istringstream is(line);
                std::string h;
                std::vector<std::string> hs;
                int label;
                int prediction = -1;
                is >> label;
                while(is >> h) {
                    hs.push_back(h);
                    if(h == h_best) prediction = +1;
                }
                if(label * prediction < 0) {
                    D[i] *= a_exp;
                } else {
                    D[i] /= a_exp;
                }
                D_sum += D[i];
                if(label > 0) D_sum_plus += D[i];
                const double d = D[i] * label;
                for(int j = 0; j < hs.size(); ++j) {
                    errors[hs[j]] -= d;
                }
                i++;
            }
        }

        // select best classifier
        e_best = D_sum_plus / D_sum;
        h_best = "";
        for(MODEL::iterator it = errors.begin(); it!=errors.end(); ++it) {
            const std::string &h = it->first;
            const double e = (it->second + D_sum_plus) / D_sum;
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
        a = 0.5 * std::log((1-e_best)/e_best);
        a_exp = std::exp(a);
        model[h_best] += a;

        // normalize
        for(int i = 0; i < D.size(); ++i) {
            D[i] /= D_sum;
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
