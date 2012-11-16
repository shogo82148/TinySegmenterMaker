# -*- coding:utf-8 -*-

import itertools
import math
import sys

class AdaBoost(object):
    def __init__(self):
        self.instances = []
        self.threshold = 0.01
        self.maximum_iter = 100
        self.model = None

    def add_instance(self, attributes, label):
        self.instances.append((frozenset(attributes), label))

    def train(self):
        instances = self.instances
        m = len(instances)
        D = [1.0/m] * m
        model = {}
        for attributes, label in instances:
            for attribute in attributes:
                model[attribute] = 0
        model[None] = 0

        for t in xrange(self.maximum_iter):
            D_sum_plus = sum(D[i] for i in xrange(m) if instances[i][1] > 0)
            errors = {}
            for h in model.iterkeys():
                errors[h] = D_sum_plus
            for d, (attributes, label) in itertools.izip(D, instances):
                d *= label
                for h in attributes:
                    errors[h] -= d
            h_max = None
            e_max = D_sum_plus
            for h in model.iterkeys():
                e = errors[h]
                if abs(0.5-e) > abs(0.5-e_max):
                    e_max = e
                    h_max = h
            print >> sys.stderr, t, abs(0.5-e_max)
            if abs(0.5-e_max) < 0.01:
                break
            if e_max < 1e-10:
                e_max = 1e-10
            if e_max > 1 - 1e-10:
                e_max = 1 - 1e-10
            a = 0.5 * math.log((1-e_max)/e_max)
            a_exp = math.exp(a)
            D_new = [d * a_exp if label * (+1 if h_max in attributes else -1) < 0 else d / a_exp
                     for d, (attributes, label) in itertools.izip(D, instances)]
            D_sum = sum(D_new)
            D = [d / D_sum for d in D_new]
            model[h_max] += a

        bias = sum(model.itervalues())
        for h in model.iterkeys():
            model[h] *= 2
        model[None] = -bias-model[None]
        self.model = model

    def predict(self, attributes):
        model = self.model
        score = model[None]
        for h in attributes:
            score += model.get(h, 0)
        return +1 if score >= 0 else -1

    def save(self, f):
        for h, a in self.model.iteritems():
            if a == 0:
                continue
            if h == None:
                f.write(str(a) + '\n')
            else:
                f.write(str(h) + '\t' + str(a) + '\n')

    def load(self, f):
        self.model = {}
        for line in f:
            a = line.strip().split('\t')
            if len(a) == 1:
                self.model[None] = float(a[0])
            else:
                self.model[a[0]] = float(a[1])
