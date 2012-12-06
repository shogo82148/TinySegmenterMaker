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
#include <boost/thread.hpp>
#include <boost/bind.hpp>

static volatile sig_atomic_t eflag = 0;

static void handler(int signum) {
    if(eflag) {
        eflag = 0;
    } else {
        exit(0);
    }
}

class AdaBoost {
private:
    std::vector<double> D;
    std::vector<double> model;
    std::vector<std::string> features;
    std::vector<std::vector<unsigned int> > instances;
    std::vector<signed char> labels;
    unsigned int num_instances;

    class Task {
    private:
        const std::vector<std::vector<unsigned int> > & instances_;
        const std::vector<signed char> & labels_;
        std::vector<double> & D_;
        unsigned int numThreads;
        unsigned int no;
        boost::thread thread;
        double a_exp;
        unsigned int h_best;

        void run() {
            const std::vector<std::vector<unsigned int> > & instances = instances_;
            const std::vector<signed char> & labels = labels_;
            std::vector<double> & D = D_;
            const unsigned int num_instances = instances.size();

            D_sum = 0.0;
            D_sum_plus = 0.0;
            std::fill(errors.begin(), errors.end(), 0.0);
            for(unsigned int i = no; i < num_instances; i+=numThreads) {
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
        }

    public:
        std::vector<double> errors;
        double D_sum;
        double D_sum_plus;

        Task(const std::vector<std::vector<unsigned int> > & instances,
             const std::vector<signed char> & labels,
             std::vector<double> D,
             unsigned int num_instances,
             unsigned int no,
             unsigned int numThreads):
            instances_(instances), labels_(labels), D_(D), errors(num_instances) {

            this->numThreads = numThreads;
            this->no = no;
        }

        void start(unsigned int h_best, double a_exp) {
            this->h_best = h_best;
            this->a_exp = a_exp;
            thread = boost::thread(boost::bind(&Task::run, this));
        }

        void join() {
            thread.join();
        }
    };

public:
    double threshold;
    unsigned int numIteration;
    unsigned int numThreads;

    AdaBoost() {
        threshold = 0.01;
        numIteration = 100;
        numThreads = 1;
    }

    void initializeFeatures(const char* instances_file) {
        std::map<std::string, double> m;
        std::ifstream ifinstances(instances_file);
        std::string line;
        num_instances = 0;
        while(ifinstances && getline(ifinstances, line)) {
            std::istringstream is(line);
            std::string h;
            int label;
            is >> label;
            while(is >> h) {
                m[h] = 0.0;
            }
            ++num_instances;
            if(num_instances % 1000 == 0) {
                std::cerr << "finding instances...: " << num_instances << " instances found\r";
            }
        }
        std::cerr << "finding instances...: " << num_instances << " instances found\n";
        m[""] = 0.0;

        for(unsigned int h = 0; h < features.size(); ++h) {
            m[features[h]] = model[h];
        }

        D.resize(0);
        D.reserve(num_instances);
        labels.resize(0);
        labels.reserve(num_instances);
        instances.resize(0);
        instances.reserve(num_instances);
        features.resize(0);
        features.reserve(m.size());
        model.resize(0);
        model.reserve(m.size());
        for(std::map<std::string, double>::iterator it = m.begin(); it != m.end(); ++it) {
            features.push_back(it->first);
            model.push_back(it->second);
        }
    }

    void initializeInstances(const char* instances_file) {
        std::ifstream ifinstances(instances_file);
        std::string line;
        const double bias = getBias();
        while(ifinstances && getline(ifinstances, line)) {
            std::istringstream is(line);
            std::string h;
            std::vector<unsigned int> v;
            int label;
            double score = bias;
            is >> label;
            labels.push_back(label);
            while(is >> h) {
                std::vector<std::string>::iterator it = std::lower_bound(features.begin(), features.end(), h);
                const unsigned int index = it - features.begin();
                v.push_back(index);
                score += model[index];
            }
            std::sort(v.begin(), v.end());
            instances.push_back(v);
            D.push_back(std::exp(-label*score*2));
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
        std::vector<Task*> tasks;

        for(int i = 0; i < numThreads; ++i) {
            tasks.push_back(new Task(instances, labels, D, num_features, i, numThreads));
        }

        for(int t = 0; eflag && t < numIteration; ++t) {
            // update & calculate errors
            double D_sum = 0.0;
            double D_sum_plus = 0.0;
            std::fill(errors.begin(), errors.end(), 0.0);
            for(int i = 0; i < numThreads; ++i) {
                tasks[i]->start(h_best, a_exp);
            }
            for(int i = 0; i < numThreads; ++i) {
                tasks[i]->join();
                D_sum += tasks[i]->D_sum;
                D_sum_plus += tasks[i]->D_sum_plus;
                for(unsigned int h = 1; h < num_features; ++h) {
                    errors[h] += tasks[i]->errors[h];
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

        for(int i = 0; i < numThreads; ++i) {
            delete tasks[i];
        }

        std::cerr << std::endl;
    }

    void saveModel(const char* model_file) const {
        const unsigned int num_features = features.size();
        std::ofstream ofmodel(model_file);
        double bias = -model[0];
        ofmodel.precision(20);
        for(unsigned int h = 1; h < num_features; ++h) {
            const double a = model[h];
            if(a == 0.0) continue;
            ofmodel << features[h] << "\t" << a << std::endl;
            bias -= a;
        }
        ofmodel << bias/2 << std::endl;
    }

    void loadModel(const char* model_file) {
        std::ifstream ifmodel(model_file);
        std::string line;
        std::map<std::string, double> m;
        double bias = 0.0;
        m[""] = 0.0;
        while(ifmodel && getline(ifmodel, line)) {
            std::istringstream is(line);
            std::string h;
            double value;
            is >> h;
            if(is >> value) {
                m[h] = value;
                bias += value;
            } else {
                m[""] = std::atof(h.c_str());
            }
        }
        m[""] = -m[""]*2 - bias;

        features.resize(0);
        features.reserve(m.size());
        model.resize(0);
        model.reserve(m.size());
        for(std::map<std::string, double>::iterator it = m.begin(); it != m.end(); ++it) {
            features.push_back(it->first);
            model.push_back(it->second);
        }
    }

    double getBias() const {
        double bias = 0.0;
        const unsigned int num_features = features.size();
        for(unsigned int h = 0; h < num_features; ++h) {
            bias -= model[h];
        }
        return bias / 2;
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
    if (signal(SIGINT, handler) == SIG_ERR) {
        std::cerr << "signal error" << std::endl;
        return -1;
    }

    // Parse arg
    int c;
    AdaBoost t;
    while((c=getopt(argc, argv, "t:n:M:m:"))!=-1) {
        switch (c) {
        case 't':
            t.threshold = std::atof(optarg);
            break;
        case 'n':
            t.numIteration = std::atoi(optarg);
            break;
        case 'M':
            t.loadModel(optarg);
            break;
        case 'm':
            t.numThreads = std::atoi(optarg);
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
    eflag = 1;
    t.train();
    t.saveModel(model_file);
    t.showResult();
    return 0;
}
