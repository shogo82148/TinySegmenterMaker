# -*- coding: utf-8 -*-
# TinySegmenter 0.1 -- Super compact Japanese tokenizer in Javascript
# (c) 2008 Taku Kudo <taku@chasen.org>
# TinySegmenter is freely distributable under the terms of a new BSD licence.
# For details, see http://chasen.org/~taku/software/TinySegmenter/LICENCE.txt
#
# Ruby Version by Yuji Kaneda (llamerada at gmail dot com)
# Ruby Template for TinySegmenterMaker by Ichinose Shogo <shogo82148@gmail.com>

# UTF8 only
$KCODE = "u"

class TinySegmenter
  VERSION = 0.1

  module CharType
    Pattern = {
      "[一二三四五六七八九十百千万億兆]" => "M",
      "[一-龠々〆ヵヶ]" => "H",
      "[ぁ-ん]" => "I",
      "[ァ-ヴーｱ-ﾝﾞｰ]" => "K",
      "[a-zA-Za-zA-Z]" => "A",
      "[0-90-9]" => "N",
    }.map{|reg, type|
      [Regexp.compile(reg, Regexp::MULTILINE, "utf-8"), type]
    }
    OTHER_TYPE = "O"

    def self.detect(str)
      Pattern.each do |reg, type|
        if reg =~ str
          return type
        end
      end
      return OTHER_TYPE
    end
  end

  class Model
    __MODEL__

    def classify(p1, p2, p3,
                      w1, w2, w3, w4, w5, w6,
                      c1, c2, c3, c4, c5, c6)
      score = BIAS
      score += UP1[p1] || 0
      score += UP2[p2] || 0
      score += UP3[p3] || 0
      score += BP1[p1 + p2] || 0
      score += BP2[p2 + p3] || 0
      score += UW1[w1] || 0
      score += UW2[w2] || 0
      score += UW3[w3] || 0
      score += UW4[w4] || 0
      score += UW5[w5] || 0
      score += UW6[w6] || 0
      score += BW1[w2 + w3] || 0
      score += BW2[w3 + w4] || 0
      score += BW3[w4 + w5] || 0
      score += TW1[w1 + w2 + w3] || 0
      score += TW2[w2 + w3 + w4] || 0
      score += TW3[w3 + w4 + w5] || 0
      score += TW4[w4 + w5 + w6] || 0
      score += UC1[c1] || 0
      score += UC2[c2] || 0
      score += UC3[c3] || 0
      score += UC4[c4] || 0
      score += UC5[c5] || 0
      score += UC6[c6] || 0
      score += BC1[c2 + c3] || 0
      score += BC2[c3 + c4] || 0
      score += BC3[c4 + c5] || 0
      score += TC1[c1 + c2 + c3] || 0
      score += TC2[c2 + c3 + c4] || 0
      score += TC3[c3 + c4 + c5] || 0
      score += TC4[c4 + c5 + c6] || 0
      score += UQ1[p1 + c1] || 0
      score += UQ2[p2 + c2] || 0
      score += UQ1[p3 + c3] || 0
      score += BQ1[p2 + c2 + c3] || 0
      score += BQ2[p2 + c3 + c4] || 0
      score += BQ3[p3 + c2 + c3] || 0
      score += BQ4[p3 + c3 + c4] || 0
      score += TQ1[p2 + c1 + c2 + c3] || 0
      score += TQ2[p2 + c2 + c3 + c4] || 0
      score += TQ3[p3 + c1 + c2 + c3] || 0
      score += TQ4[p3 + c2 + c3 + c4] || 0
      return score
    end
  end

  def initialize
    @model = Model.new
  end

  def segment(input)
    input = input.to_s
    return [] if input.empty?
    result = [];
    seg = ["B3","B2","B1"];
    ctype = ["O","O","O"];
    input.split(//).each do |char|
      seg << char
      ctype << CharType.detect(char)
    end
    seg << "E1" << "E2" << "E3"
    ctype << "O" << "O" << "O"

    word = seg[3]
    ps  = ["U", "U", "U"]
    4.upto(seg.length - 4) do |i|
      score = @model.classify(ps[0], ps[1], ps[2],
                 seg[i-3], seg[i-2], seg[i-1], seg[i], seg[i+1], seg[i+2],
                 ctype[i-3], ctype[i-2], ctype[i-1], ctype[i], ctype[i+1], ctype[i+2])
      if score > 0
        # split
        result << word
        word = ""
        p = "B"
      else
        p = "O"
      end
      # update ps.
      ps.unshift(p)
      ps.pop
      word << seg[i]
    end
    result << word
    return result
  end
end # TinySegmenter
