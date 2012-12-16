#!/usr/bin/env python
# -*- coding:utf-8 -*-

import sys
import codecs

sys.stdin  = codecs.getreader('utf-8')(sys.stdin)
sys.stdout = codecs.getwriter('utf-8')(sys.stdout)

kanji_numbers = u"一二三四五六七八九十百千万億兆"

hiragana = u"あいうえおかきくけこがぎぐげごさしすせそざじずぜぞたちつてとだぢづでどなにぬねのはひふへほばびぶべぼぱぴぷぺぽまみむめもやゐゆゑよらりるれろわをんぁぃぅぇぉゎっ"
katakana = u"アイウエオカキクケコガギグゲゴサシスセソザジズゼゾタチツテトダヂヅデドナニヌネノハヒフヘホバビブベボパピプペポマミムメモヤヰユヱヨラリルレロワヲンァィゥェォヮヴーッ" \
    u"ｱｲｳｴｵｶｷｸｹｺｻｼｽｾｿﾀﾁﾂﾃﾄﾅﾆﾇﾈﾉﾊﾋﾌﾍｵﾏﾐﾑﾒﾓﾔﾕﾖﾗﾘﾙﾚﾛﾜｵﾝﾞﾟｰｯ"

alphabets = u"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZａｂｃｄｅｆｇｈｉｊｋｌｍｎｏｐｑｒｓｔｕｖｗｘｙｚＡＢＣＤＥＦＧＨＩＪＫＬＭＮＯＰＱＲＳＴＵＶＷＸＹＺ"

numbers = u"０１２３４５６７８９"

ctype = r"\def\tseg@ctype@%s{%s}"

for ch in kanji_numbers:
    print ctype % (ch, 'M')

for ch in hiragana:
    print ctype % (ch, 'I')

for ch in katakana:
    print ctype % (ch, 'K')

for ch in alphabets:
    print ctype % (ch, 'A')

for ch in numbers:
    print ctype % (ch, 'N')
