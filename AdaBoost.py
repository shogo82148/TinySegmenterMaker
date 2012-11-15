# -*- coding:utf-8 -*-

import itertools
import math
import sys

class AdaBoost(object):
    def __init__(self):
        self.instances = []
        self.threshold = 0.01
        self.maximum_iter = 10000
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

        for i in xrange(self.maximum_iter):
            h_max = None
            e_max = 0.5
            for h in model.iterkeys():
                e = 0
                for d, (attributes, label) in itertools.izip(D, instances):
                    predict = +1 if h in attributes else -1
                    if label * predict < 0:
                        e += d
                if abs(0.5-e) > abs(0.5-e_max):
                    h_max = h
                    e_max = e
            print >> sys.stderr, i, abs(0.5-e_max)
            if abs(0.5-e_max) < 0.01:
                break
            if e_max < 1e-10:
                e_max = 1e-10
            if e_max > 1 - 1e-10:
                e_max = 1 - 1e-10
            a = 0.5 * math.log((1-e_max)/e_max)
            D_new = [d * math.exp(-a * label * (+1 if h_max in attributes else -1))
                     for d, (attributes, label) in itertools.izip(D, instances)]
            D_sum = sum(D_new)
            D = [d / D_sum for d in D_new]
            model[h_max] += a
        self.model = model

    def predict(self, attributes):
        score = 0
        for h, a in self.model.iteritems():
            score += a * (+1 if h in attributes else -1)
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
                self.model[None] = float(a)
            else:
                self.model[a[0]] = float(a)
