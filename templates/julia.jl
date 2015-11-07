__precompile__()
module TinySegmenter

export tokenize

typealias US UTF8String

macro t_str(s)
  tuple(s...)
end

# Use out of range of Unicode code point. See also: https://en.wikipedia.org/wiki/Code_point
const B1 = Char(0x110001)
const B2 = Char(0x110002)
const B3 = Char(0x110003)
const E1 = Char(0x110004)
const E2 = Char(0x110005)

__MODEL__

const CHARDICT = Dict{Char, UInt8}()
for (chars,cat) in (
    ("一二三四五六七八九十百千万億兆",'M'),
    ("々〆ヵヶ", 'H'),
    ('ぁ':'ん','I'),
    ('ァ':'ヴ','K'),
    ("ーｰ\uff9e",'K'),
    ('ｱ':'ﾝ','K'),
    (['a':'z';'A':'Z';'ａ':'ｚ';'Ａ':'Ｚ'],'A'),
    (['0':'9';'０':'９'],'N')
  )
  for c in chars
      CHARDICT[c] = cat
  end
end
const Achar = UInt8('A')
const Ichar = UInt8('I')
const Hchar = UInt8('H')
const Ochar = UInt8('O')
const Uchar = UInt8('U')
const Bchar = UInt8('B')
function ctype(c::Char)
  return get(CHARDICT, c, '一' <= c <= '龠' ? Hchar : Ochar)
end

"""
    tokenize(text::AbstractString)

Given a `text` string, `tokenize` attempts to segment it into a list
of words, and in particular tries to segment Japanese text
into words ("tokens" or "segments"), using the TinySegmenter algorithm.
It returns an array of substrings consisting of the guessed tokens in
`text`, in the order that they appear.
"""
function tokenize{T<:AbstractString}(text::T)
  result = SubString{T}[]
  isempty(text) && return result

  wordstart = start(text)
  pos = wordstart

  p1 = Uchar
  p2 = Uchar
  p3 = Uchar
  w1, w2, w3 = B3, B2, B1
  c1, c2, c3 = Ochar, Ochar, Ochar
  w4 = text[pos]
  c4 = ctype(w4)

  pos1 = nextind(text, pos) # pos + 1 character
  pos2 = nextind(text, pos1) # pos + 2 characters
  if pos == endof(text)
    w5, w6 = E1, E2
    c5, c6 = Ochar, Ochar
  else
    w5 = text[pos1]
    c5 = ctype(w5)
    if pos1 == endof(text)
      w6 = E1
      c6 = Ochar
    else
      w6 = text[pos2]
      c6 = ctype(w6)
    end
  end

  while pos < endof(text)
    score = BIAS
    w1,w2,w3,w4,w5 = w2,w3,w4,w5,w6
    c1,c2,c3,c4,c5 = c2,c3,c4,c5,c6
    pos3 = nextind(text, pos2) # pos + 3
    if pos3 <= endof(text)
      w6 = text[pos3]
      c6 = ctype(w6)
    elseif pos2 == endof(text)
      w6 = E1
      c6 = Ochar
    else # pos1 == endof(text)
      w6 = E2
      c6 = Ochar
    end

    if p1 == Ochar; score += -214; end # score += get(UP1, p1, 0)
    if p2 == Bchar; score += 69; elseif p2 == Ochar; score += 935; end # score += get(UP2, p2, 0)
    if p3 == Bchar; score += 189; end # score += get(UP3, p3, 0)
    score += get(BP1, (p1, p2), 0)
    score += get(BP2, (p2, p3), 0)
    score += get(UW1, w1, 0)
    score += get(UW2, w2, 0)
    score += get(UW3, w3, 0)
    score += get(UW4, w4, 0)
    score += get(UW5, w5, 0)
    score += get(UW6, w6, 0)
    score += get(BW1, (w2, w3), 0)
    score += get(BW2, (w3, w4), 0)
    score += get(BW3, (w4, w5), 0)
    score += get(TW1, (w1, w2, w3), 0)
    score += get(TW2, (w2, w3, w4), 0)
    score += get(TW3, (w3, w4, w5), 0)
    score += get(TW4, (w4, w5, w6), 0)
    score += get(UC1, c1, 0)
    score += get(UC2, c2, 0)
    if c3 == Achar; score += -1370; elseif c3 == Ichar; score += 2311; end # score += get(UC3, c3, 0)
    score += get(UC4, c4, 0)
    score += get(UC5, c5, 0)
    score += get(UC6, c6, 0)
    score += get(BC1, (c2, c3), 0)
    score += get(BC2, (c3, c4), 0)
    score += get(BC3, (c4, c5), 0)
    score += get(TC1, (c1, c2, c3), 0)
    score += get(TC2, (c2, c3, c4), 0)
    score += get(TC3, (c3, c4, c5), 0)
    score += get(TC4, (c4, c5, c6), 0)
    score += get(UQ1, (p1, c1), 0)
    score += get(UQ2, (p2, c2), 0)
    score += get(UQ3, (p3, c3), 0)
    score += get(BQ1, (p2, c2, c3), 0)
    score += get(BQ2, (p2, c3, c4), 0)
    score += get(BQ3, (p3, c2, c3), 0)
    score += get(BQ4, (p3, c3, c4), 0)
    score += get(TQ1, (p2, c1, c2, c3), 0)
    score += get(TQ2, (p2, c2, c3, c4), 0)
    score += get(TQ3, (p3, c1, c2, c3), 0)
    score += get(TQ4, (p3, c2, c3, c4), 0)

    p = Ochar
    if score > 0
      push!(result, SubString(text, wordstart, pos))
      wordstart = pos1
      p = Bchar
    end

    p1 = p2
    p2 = p3
    p3 = p
    pos = pos1
    pos1 = pos2
    pos2 = pos3
  end

  push!(result, SubString(text, wordstart, pos))
  return result
end

end # module
