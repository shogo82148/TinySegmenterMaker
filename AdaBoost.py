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

    def predict(self, attributes):
        model = self.model
        score = model[None]
        for h in attributes:
            score += model.get(h, 0)
        return +1 if score >= 0 else -1

    def load(self, f):
        self.model = {}
        for line in f:
            a = line.strip().split('\t')
            if len(a) == 1:
                self.model[None] = float(a[0])
            else:
                self.model[a[0]] = float(a[1])
