# -*- coding:utf-8 -*-

from AdaBoost import AdaBoost
import re

class Segmenter(object):
    patterns = [
        [re.compile(u'[一二三四五六七八九十百千万億兆]'), 'M'],
        [re.compile(u'[一-龠々〆ヵヶ]'), 'H'],
        [re.compile(u'[ぁ-ん]'), 'I'],
        [re.compile(u'[ァ-ヴーｱ-ﾝﾞｰ]'), 'K'],
        [re.compile(u'[a-zA-Zａ-ｚＡ-Ｚ]'), 'A'],
        [re.compile(u'[0-9０-９]'), 'N'],
        ]

    def __init__(self, learner = None):
        self.learner = learner or AdaBoost()

    def getType(self, ch):
        for pattern, sType in self.patterns:
            if pattern.match(ch):
                return sType
        return u'O'

    def addSentence(self, sentence):
        if not sentence:
            return
        learner = self.learner
        tags = [u'U', u'U', u'U']
        chars = [u'B3', u'B2', u'B1']
        types = [u'O', u'O', u'O']

        for word in sentence.split(u' '):
            tags.extend([u'B'] + [u'O'] * (len(word)-1))
            for ch in word:
                chars.append(ch)
                types.append(self.getType(ch))
        if len(tags) < 4:
            return
        tags[3] = u'U'

        chars.extend([u'E1', u'E2', u'E3'])
        types.extend([u'O', u'O', u'O'])

        for i in range(4, len(chars) - 3):
            label = +1 if tags[i] == u'B' else -1
            learner.add_instance(self.getAttributes(i, tags, chars, types), label)

    def parse(self, sentence):
        if not sentence:
            return []
        learner = self.learner
        tags = [u'U', u'U', u'U', u'U']
        chars = [u'B3', u'B2', u'B1']
        types = [u'O', u'O', u'O']

        for ch in sentence:
            chars.append(ch)
            types.append(self.getType(ch))
        chars.extend([u'E1', u'E2', u'E3'])
        types.extend([u'O', u'O', u'O'])

        result = []
        word = chars[3]
        for i in range(4, len(chars) - 3):
            label = learner.predict(self.getAttributes(i, tags, chars, types))
            if label >= 0:
                result.append(word)
                word = u''
                tags.append(u'B')
            else:
                tags.append(u'O')
            word += chars[i]
        result.append(word)
        return result

    def getAttributes(self, i, tags, chars, types):
        w1 = chars[i-3]
        w2 = chars[i-2]
        w3 = chars[i-1]
        w4 = chars[i]
        w5 = chars[i+1]
        w6 = chars[i+2]
        c1 = types[i-3]
        c2 = types[i-2]
        c3 = types[i-1]
        c4 = types[i]
        c5 = types[i+1]
        c6 = types[i+2]
        p1 = tags[i-3]
        p2 = tags[i-2]
        p3 = tags[i-1]
        attributes = set([
                u'UP1:' + p1,
                u'UP2:' + p2,
                u'UP3:' + p3,
                u'BP1:' + p1 + p2,
                u'BP2:' + p2 + p3,
                u'UW1:' + w1,
                u'UW2:' + w2,
                u'UW3:' + w3,
                u'UW4:' + w4,
                u'UW5:' + w5,
                u'UW6:' + w6,
                u'BW1:' + w2 + w3,
                u'BW2:' + w3 + w4,
                u'BW3:' + w4 + w5,
                u'TW1:' + w1 + w2 + w3,
                u'TW2:' + w2 + w3 + w4,
                u'TW3:' + w3 + w4 + w5,
                u'TW4:' + w4 + w5 + w6,
                u'UC1:' + c1,
                u'UC2:' + c2,
                u'UC3:' + c3,
                u'UC4:' + c4,
                u'UC5:' + c5,
                u'UC6:' + c6,
                u'BC1:' + c2 + c3,
                u'BC2:' + c3 + c4,
                u'BC3:' + c4 + c5,
                u'TC1:' + c1 + c2 + c3,
                u'TC2:' + c2 + c3 + c4,
                u'TC3:' + c3 + c4 + c5,
                u'TC4:' + c4 + c5 + c6,
                u'UQ1:' + p1 + c1,
                u'UQ2:' + p2 + c2,
                u'UQ3:' + p3 + c3,
                u'BQ1:' + p2 + c2 + c3,
                u'BQ2:' + p2 + c3 + c4,
                u'BQ3:' + p3 + c2 + c3,
                u'BQ4:' + p3 + c3 + c4,
                u'TQ1:' + p2 + c1 + c2 + c3,
                u'TQ2:' + p2 + c2 + c3 + c4,
                u'TQ3:' + p3 + c1 + c2 + c3,
                u'TQ4:' + p3 + c2 + c3 + c4,
                ])
        return attributes
