" vi:set ts=8 sts=2 sw=2 tw=0:
scriptencoding utf-8

" autoload/tinysegmenter.vim
"
" Maintainer: KIHARA Hideto <deton@m1.interq.or.jp>
" Last Change: 2013-03-10
"
" TinySegmenter 0.1 -- Super compact Japanese tokenizer in Javascript
" (c) 2008 Taku Kudo <taku@chasen.org>
" TinySegmenter is freely distributable under the terms of a new BSD licence.
" For details, see http://chasen.org/~taku/software/TinySegmenter/LICENCE.txt

let s:save_cpo = &cpo
set cpo&vim

let s:model = __MODEL__

if !exists('g:tinysegmenter#model')
  let g:tinysegmenter#model = s:model
endif

let s:patterns = {
  \ '[一二三四五六七八九十百千万億兆]':'M',
  \ '[々〆ヵヶ]':'H',
  \ '[ぁ-ん]':'I',
  \ '[ァ-ヴーｱ-ﾝﾞｰ]':'K',
  \ '[a-zA-Zａ-ｚＡ-Ｚ]':'A',
  \ '[0-9０-９]':'N'
\ }
" \ '[一-龠々〆ヵヶ]':'H',

" for &encoding == 'utf-8'
let s:kanjimin = char2nr('一')
let s:kanjimax = char2nr('龠')
if &encoding == 'euc-jp' || &encoding == 'cp932'
  " TODO: support JIS X 0213
  let s:kanjimin = char2nr('亜')
  let s:kanjimax = char2nr('熙')
endif

function s:getctype(str)
  for [pat, value] in items(s:patterns)
    if match(a:str, pat) >= 0
      return value
    endif
  endfor
  let nr = char2nr(a:str)
  if nr >= s:kanjimin && nr <= s:kanjimax
    return 'H'
  endif
  return 'O'
endfunction

function tinysegmenter#segment(input)
  if a:input == ''
    return []
  endif
  let m = g:tinysegmenter#model
  let result = []
  let seg = ['B3','B2','B1']
  let ctype = ['O','O','O']
  for c in split(a:input, '\zs')
    call add(seg, c)
    call add(ctype, s:getctype(c))
  endfor
  call extend(seg, ['E1','E2','E3'])
  call extend(ctype, ['O','O','O'])
  let word = seg[3]
  let p1 = 'U'
  let p2 = 'U'
  let p3 = 'U'
  let i = 4
  while i < len(seg) - 3
    let score = m.BIAS
    let w1 = seg[i-3]
    let w2 = seg[i-2]
    let w3 = seg[i-1]
    let w4 = seg[i]
    let w5 = seg[i+1]
    let w6 = seg[i+2]
    let c1 = ctype[i-3]
    let c2 = ctype[i-2]
    let c3 = ctype[i-1]
    let c4 = ctype[i]
    let c5 = ctype[i+1]
    let c6 = ctype[i+2]
    let score += get(m.UP1, p1)
    let score += get(m.UP2, p2)
    let score += get(m.UP3, p3)
    let score += get(m.BP1, p1 . p2)
    let score += get(m.BP2, p2 . p3)
    let score += get(m.UW1, w1)
    let score += get(m.UW2, w2)
    let score += get(m.UW3, w3)
    let score += get(m.UW4, w4)
    let score += get(m.UW5, w5)
    let score += get(m.UW6, w6)
    let score += get(m.BW1, w2 . w3)
    let score += get(m.BW2, w3 . w4)
    let score += get(m.BW3, w4 . w5)
    let score += get(m.TW1, w1 . w2 . w3)
    let score += get(m.TW2, w2 . w3 . w4)
    let score += get(m.TW3, w3 . w4 . w5)
    let score += get(m.TW4, w4 . w5 . w6)
    let score += get(m.UC1, c1)
    let score += get(m.UC2, c2)
    let score += get(m.UC3, c3)
    let score += get(m.UC4, c4)
    let score += get(m.UC5, c5)
    let score += get(m.UC6, c6)
    let score += get(m.BC1, c2 . c3)
    let score += get(m.BC2, c3 . c4)
    let score += get(m.BC3, c4 . c5)
    let score += get(m.TC1, c1 . c2 . c3)
    let score += get(m.TC2, c2 . c3 . c4)
    let score += get(m.TC3, c3 . c4 . c5)
    let score += get(m.TC4, c4 . c5 . c6)
    let score += get(m.UQ1, p1 . c1)
    let score += get(m.UQ2, p2 . c2)
    let score += get(m.UQ3, p3 . c3)
    let score += get(m.BQ1, p2 . c2 . c3)
    let score += get(m.BQ2, p2 . c3 . c4)
    let score += get(m.BQ3, p3 . c2 . c3)
    let score += get(m.BQ4, p3 . c3 . c4)
    let score += get(m.TQ1, p2 . c1 . c2 . c3)
    let score += get(m.TQ2, p2 . c2 . c3 . c4)
    let score += get(m.TQ3, p3 . c1 . c2 . c3)
    let score += get(m.TQ4, p3 . c2 . c3 . c4)
    let p = 'O'
    if score > 0
      call add(result, word)
      let word = ''
      let p = 'B'
    endif
    let p1 = p2
    let p2 = p3
    let p3 = p
    let word .= seg[i]
    let i += 1
  endwhile
  call add(result, word)
  return result
endfunction

let &cpo = s:save_cpo
