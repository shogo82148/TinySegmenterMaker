#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
TinySegmenter 0.1 -- Super compact Japanese tokenizer in Javascript
(c) 2008 Taku Kudo <taku@chasen.org>
TinySegmenter is freely distributable under the terms of a new BSD licence.
For details, see http://chasen.org/~taku/software/TinySegmenter/LICENCE.txt
"""

import re

class TinySegmenter(object):
    default_model = __MODEL__
    patterns = {
        u"[一二三四五六七八九十百千万億兆]":u"M",
        u"[一-龠々〆ヵヶ]":u"H",
        u"[ぁ-ん]":u"I",
        u"[ァ-ヴーｱ-ﾝﾞｰ]":u"K",
        u"[a-zA-Zａ-ｚＡ-Ｚ]":u"A",
        u"[0-9０-９]":u"N"
    };

    def __init__(self, model = None):
        self.model = model or self.default_model;

        chartype = [];
        for pattern, label in self.patterns.iteritems():
            chartype.append([re.compile(pattern), label])
        self.chartype = chartype

    def getctype(self, str):
        for pattern, label in self.chartype:
            if pattern.match(str):
                return label
        return u"O"

    def segment(self, input):
        if not input:
            return []

        m = self.model
        result = []
        seg = [u"B3",u"B2",u"B1"]
        ctype = [u"O",u"O",u"O"]
        for ch in input:
            seg.append(ch)
            ctype.append(self.getctype(ch))
        seg.append(u"E1")
        seg.append(u"E2")
        seg.append(u"E3")
        ctype.append(u"O")
        ctype.append(u"O")
        ctype.append(u"O")
        word = seg[3]
        p1 = u"U"
        p2 = u"U"
        p3 = u"U"

        for i in xrange(4, len(seg)-3):
            score = m["BIAS"]
            w1 = seg[i-3]
            w2 = seg[i-2]
            w3 = seg[i-1]
            w4 = seg[i]
            w5 = seg[i+1]
            w6 = seg[i+2]
            c1 = ctype[i-3]
            c2 = ctype[i-2]
            c3 = ctype[i-1]
            c4 = ctype[i]
            c5 = ctype[i+1]
            c6 = ctype[i+2]
            score += m[u'UP1'].get(p1,0)
            score += m[u'UP2'].get(p2,0)
            score += m[u'UP3'].get(p3,0)
            score += m[u'BP1'].get(p1 + p2,0)
            score += m[u'BP2'].get(p2 + p3,0)
            score += m[u'UW1'].get(w1,0)
            score += m[u'UW2'].get(w2,0)
            score += m[u'UW3'].get(w3,0)
            score += m[u'UW4'].get(w4,0)
            score += m[u'UW5'].get(w5,0)
            score += m[u'UW6'].get(w6,0)
            score += m[u'BW1'].get(w2 + w3,0)
            score += m[u'BW2'].get(w3 + w4,0)
            score += m[u'BW3'].get(w4 + w5,0)
            score += m[u'TW1'].get(w1 + w2 + w3,0)
            score += m[u'TW2'].get(w2 + w3 + w4,0)
            score += m[u'TW3'].get(w3 + w4 + w5,0)
            score += m[u'TW4'].get(w4 + w5 + w6,0)
            score += m[u'UC1'].get(c1,0)
            score += m[u'UC2'].get(c2,0)
            score += m[u'UC3'].get(c3,0)
            score += m[u'UC4'].get(c4,0)
            score += m[u'UC5'].get(c5,0)
            score += m[u'UC6'].get(c6,0)
            score += m[u'BC1'].get(c2 + c3,0)
            score += m[u'BC2'].get(c3 + c4,0)
            score += m[u'BC3'].get(c4 + c5,0)
            score += m[u'TC1'].get(c1 + c2 + c3,0)
            score += m[u'TC2'].get(c2 + c3 + c4,0)
            score += m[u'TC3'].get(c3 + c4 + c5,0)
            score += m[u'TC4'].get(c4 + c5 + c6,0)
            score += m[u'UQ1'].get(p1 + c1,0)
            score += m[u'UQ2'].get(p2 + c2,0)
            score += m[u'UQ3'].get(p3 + c3,0)
            score += m[u'BQ1'].get(p2 + c2 + c3,0)
            score += m[u'BQ2'].get(p2 + c3 + c4,0)
            score += m[u'BQ3'].get(p3 + c2 + c3,0)
            score += m[u'BQ4'].get(p3 + c3 + c4,0)
            score += m[u'TQ1'].get(p2 + c1 + c2 + c3,0)
            score += m[u'TQ2'].get(p2 + c2 + c3 + c4,0)
            score += m[u'TQ3'].get(p3 + c1 + c2 + c3,0)
            score += m[u'TQ4'].get(p3 + c2 + c3 + c4,0)
            p = u"O"
            if score > 0:
                result.append(word)
                word = u""
                p = u"B"
            p1, p2, p3 = p2, p3, p
            word += seg[i]
        result.append(word)
        return result
