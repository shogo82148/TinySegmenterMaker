package tinysegmenter

import (
	"unicode"
	"unicode/utf8"
)

const (
	B1 rune = 0x110001
	B2 rune = 0x110002
	B3 rune = 0x110003
	E1 rune = 0x110004
	E2 rune = 0x110005
	E3 rune = 0x110006
)

var (
	kannumTable = []rune{
		'一', '二', '三', '四', '五', '六', '七', '八', '九', '十',
		'百', '千', '万', '億', '兆',
	}
	kanjiTable = &unicode.RangeTable{ //[一-龠々〆ヵヶ]
		R16: []unicode.Range16{
			{0x3005, 0x3006, 1},
			{0x30F5, 0x30F6, 1},
			{0x4E00, 0xff9E, 1},
		},
	}
	hiraganaTable = &unicode.RangeTable{ //[ぁ-ん]
		R16: []unicode.Range16{
			{0x3041, 0x3093, 1},
		},
	}
	katakanaTable = &unicode.RangeTable{ //[ァ-ヴーｱ-ﾝﾞｰ]
		R16: []unicode.Range16{
			{0x30A1, 0x30F4, 1},
			{0x30FC, 0x30FC, 1},
			{0xFF70, 0xFF9E, 1},
		},
	}
	alphabetTable = &unicode.RangeTable{ //[a-zA-Zａ-ｚＡ-Ｚ]
		R16: []unicode.Range16{
			{0x41, 0x5A, 1},
			{0x61, 0x7A, 1},
			{0xFF21, 0xFF3A, 1},
			{0xFF41, 0xFF5A, 1},
		},
	}
	numberTable = &unicode.RangeTable{ //[0-9０-９]
		R16: []unicode.Range16{
			{0x30, 0x39, 1},
			{0xFF10, 0xFF19, 1},
		},
	}
)

type Segmenter struct {
	Bias int
	UP1  map[rune]int
	UP2  map[rune]int
	UP3  map[rune]int
	UC1  map[rune]int
	UC2  map[rune]int
	UC3  map[rune]int
	UC4  map[rune]int
	UC5  map[rune]int
	UC6  map[rune]int
	BP1  map[[2]rune]int
	BP2  map[[2]rune]int
	BC1  map[[2]rune]int
	BC2  map[[2]rune]int
	BC3  map[[2]rune]int
	UQ1  map[[2]rune]int
	UQ2  map[[2]rune]int
	UQ3  map[[2]rune]int
	TC1  map[[3]rune]int
	TC2  map[[3]rune]int
	TC3  map[[3]rune]int
	TC4  map[[3]rune]int
	BQ1  map[[3]rune]int
	BQ2  map[[3]rune]int
	BQ3  map[[3]rune]int
	BQ4  map[[3]rune]int
	TQ1  map[[4]rune]int
	TQ2  map[[4]rune]int
	TQ3  map[[4]rune]int
	TQ4  map[[4]rune]int
	UW1  map[rune]int
	UW2  map[rune]int
	UW3  map[rune]int
	UW4  map[rune]int
	UW5  map[rune]int
	UW6  map[rune]int
	BW1  map[[2]rune]int
	BW2  map[[2]rune]int
	BW3  map[[2]rune]int
	TW1  map[[3]rune]int
	TW2  map[[3]rune]int
	TW3  map[[3]rune]int
	TW4  map[[3]rune]int
}

func NewSegmenter() *Segmenter {
	s := &Segmenter{__MODEL__}
	return s
}

func gettype(c rune) rune {
	for _, x := range kannumTable {
		if x == c {
			return 'M'
		}
	}
	switch {
	case unicode.In(c, kanjiTable):
		return 'H'
	case unicode.In(c, hiraganaTable):
		return 'I'
	case unicode.In(c, katakanaTable):
		return 'K'
	case unicode.In(c, alphabetTable):
		return 'A'
	case unicode.In(c, numberTable):
		return 'N'
	}
	return 'O'
}

func (s *Segmenter) Segment(input string) []string {
	ret := make([]string, 0, len(input))
	if input == "" {
		return ret
	}

	wordstart := 0
	pos := wordstart

	p1, w1, c1 := 'U', B3, 'O'
	p2, w2, c2 := 'U', B2, 'O'
	p3, w3, c3 := 'U', B1, 'O'

	w5, c5 := E1, 'O'
	w6, c6 := E2, 'O'

	var pos1, pos2, pos3, size int
	w4, pos1 := utf8.DecodeRuneInString(input[pos:])
	c4 := gettype(w4)
	if pos1 < len(input) {
		w5, size = utf8.DecodeRuneInString(input[pos1:])
		c5 = gettype(w5)
		pos2 = pos1 + size
		if pos2 < len(input) {
			w6, size = utf8.DecodeRuneInString(input[pos2:])
			c6 = gettype(w6)
			pos3 = pos2 + size
		} else {
			w6, c6 = E1, 'O'
		}
	}

	for pos < len(input) {
		w1, w2, w3, w4, w5 = w2, w3, w4, w5, w6
		c1, c2, c3, c4, c5 = c2, c3, c4, c5, c6

		if pos1 == len(input) {
			w6, c6 = E2, 'O'
		} else if pos2 == len(input) {
			w6, c6 = E1, 'O'
		} else {
			pos3 = pos2 + size
			w6, size = utf8.DecodeRuneInString(input[pos3:])
			c6 = gettype(w6)
		}

		score := s.Bias
		score += s.UP1[p1]
		score += s.UP2[p2]
		score += s.UP2[p2]
		score += s.BP1[[2]rune{p1, p2}]
		score += s.BP2[[2]rune{p2, p3}]
		score += s.UW1[w1]
		score += s.UW2[w2]
		score += s.UW3[w3]
		score += s.UW4[w4]
		score += s.UW5[w5]
		score += s.UW6[w6]
		score += s.BW1[[2]rune{w2, w3}]
		score += s.BW2[[2]rune{w3, w4}]
		score += s.BW3[[2]rune{w4, w5}]
		score += s.TW1[[3]rune{w1, w2, w3}]
		score += s.TW2[[3]rune{w2, w3, w4}]
		score += s.TW3[[3]rune{w3, w4, w5}]
		score += s.TW4[[3]rune{w4, w5, w6}]
		score += s.UC1[c1]
		score += s.UC2[c2]
		score += s.UC3[c3]
		score += s.UC4[c4]
		score += s.UC5[c5]
		score += s.UC6[c6]
		score += s.BC1[[2]rune{c2, c3}]
		score += s.BC2[[2]rune{c3, c4}]
		score += s.BC3[[2]rune{c4, c5}]
		score += s.TC1[[3]rune{c1, c2, c3}]
		score += s.TC2[[3]rune{c2, c3, c4}]
		score += s.TC3[[3]rune{c3, c4, c5}]
		score += s.TC4[[3]rune{c4, c5, c6}]
		score += s.UQ1[[2]rune{p1, c1}]
		score += s.UQ2[[2]rune{p2, c2}]
		score += s.UQ3[[2]rune{p3, c3}]
		score += s.BQ1[[3]rune{p2, c2, c3}]
		score += s.BQ2[[3]rune{p2, c3, c4}]
		score += s.BQ3[[3]rune{p3, c2, c3}]
		score += s.BQ4[[3]rune{p3, c3, c4}]
		score += s.TQ1[[4]rune{p2, c1, c2, c3}]
		score += s.TQ2[[4]rune{p2, c2, c3, c4}]
		score += s.TQ3[[4]rune{p3, c1, c2, c3}]
		score += s.TQ4[[4]rune{p3, c2, c3, c4}]

		p := 'O'
		if score > 0 {
			ret = append(ret, input[wordstart:pos1])
			wordstart = pos1
			p = 'B'
		}
		p1, p2, p3 = p2, p3, p
		pos, pos1, pos2 = pos1, pos2, pos3

	}
	if wordstart != len(input) {
		ret = append(ret, input[wordstart:])
	}

	return ret
}
