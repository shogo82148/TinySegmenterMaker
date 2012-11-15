# -*- coding:utf-8 -*-

from AdaBoost import AdaBoost
import re

patterns = [
    [re.compile(u'[一ニ三四五六七八九十百千万億兆]'), 'M'],
    [re.compile(u'[一-龠々〆ヵヶ]'), 'H'],
    [re.compile(u'[ぁ-ん]'), 'I'],
    [re.compile(u'[ァ-ヴーｱ-ﾝﾞｰ]'), 'K'],
    [re.compile(u'[a-zA-Zａ-ｚＡ-Ｚ]'), 'A'],
    [re.compile(u'[0-9０-９]'), 'N'],
]

def getType(ch):
    for pattern, sType in patterns:
        if pattern.match(ch):
            return sType
    return 'O'

def addSentence(learner, sentence):
    tags = ['U', 'U', 'U']
    chars = ['B3', 'B2', 'B1']
    types = ['O', 'O', 'O']

    for word in sentence.split(' '):
        tags.extend(['O'] * (len(word)-1) + ['B'])
        for ch in word:
            chars.append(ch.encode('utf-8'))
            types.append(getType(ch))

    chars.extend(['E1', 'E2', 'E3'])
    types.extend(['O', 'O', 'O'])

    for i in xrange(4, len(chars) - 3):
        label = +1 if tags[i] == 'B' else -1
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
                'UP1:' + p1,
                'UP2:' + p2,
                'UP3:' + p3,
                'BP1:' + p1 + p2,
                'BP2:' + p2 + p3,
                'UW1:' + w1,
                'UW2:' + w2,
                'UW3:' + w3,
                'UW4:' + w4,
                'UW5:' + w5,
                'UW6:' + w6,
                'BW1:' + w2 + w3,
                'BW2:' + w3 + w4,
                'BW3:' + w4 + w5,
                'TW1:' + w1 + w2 + w3,
                'TW2:' + w2 + w3 + w4,
                'TW3:' + w3 + w4 + w5,
                'TW4:' + w4 + w5 + w6,
                'UC1:' + c1,
                'UC2:' + c2,
                'UC3:' + c3,
                'UC4:' + c4,
                'UC5:' + c5,
                'UC6:' + c6,
                'BC1:' + c2 + c3,
                'BC2:' + c3 + c4,
                'BC3:' + c4 + c5,
                'TC1:' + c1 + c2 + c3,
                'TC2:' + c2 + c3 + c4,
                'TC3:' + c3 + c4 + c5,
                'TC4:' + c4 + c5 + c6,
                'UQ1:' + p1 + c1,
                'UQ2:' + p2 + c2,
                'UQ3:' + p3 + c3,
                ])
        learner.add_instance(attributes, label)
