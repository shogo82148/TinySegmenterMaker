package tinysegmenter

import (
	"regexp"
	"strings"
)

type CharType struct {
	name string
	re   *regexp.Regexp
}

type Segmenter struct {
	chartypes []CharType
	model     map[string]int
}

func NewSegmenter() *Segmenter {
	s := new(Segmenter)
	regs := [][]string{
		{"[一二三四五六七八九十百千万億兆]", "M"},
		{"[一-龠々〆ヵヶ]", "H"},
		{"[ぁ-ん]", "I"},
		{"[ァ-ヴーｱ-ﾝﾞｰ]", "K"},
		{"[a-zA-Zａ-ｚＡ-Ｚ]", "A"},
		{"[0-9０-９]", "N"},
	}

	for _, v := range regs {
		re, _ := regexp.Compile(v[0])
		s.chartypes = append(s.chartypes, CharType{v[1], re})
	}

	s.model = map[string]int{__MODEL__}

	return s
}

func (s *Segmenter) gettype(str string) string {
	for _, v := range s.chartypes {
		if v.re.MatchString(str) {
			return v.name
		}
	}
	return "O"
}

func (s *Segmenter) Segment(str string) []string {
	o := strings.Split(str, "")
	m := s.model

	ctype := []string{"O", "O", "O"}
	for _, v := range o {
		ctype = append(ctype, s.gettype(v))
	}
	ctype = append(ctype, "O", "O", "O")

	seg := []string{"B3", "B2", "B1"}
	seg = append(seg, o...)
	seg = append(seg, "E1", "E2", "E3")

	var result []string
	word := []string{seg[3]}
	p1, p2, p3 := "U", "U", "U"

	for i := 4; i < len(seg)-3; i++ {
		score := m["BIAS"]
		w1 := seg[i-3]
		w2 := seg[i-2]
		w3 := seg[i-1]
		w4 := seg[i]
		w5 := seg[i+1]
		w6 := seg[i+2]
		c1 := ctype[i-3]
		c2 := ctype[i-2]
		c3 := ctype[i-1]
		c4 := ctype[i]
		c5 := ctype[i+1]
		c6 := ctype[i+2]
		score += m["UP1:"+p1]
		score += m["UP2:"+p2]
		score += m["UP3:"+p3]
		score += m["BP1:"+p1+p2]
		score += m["BP2:"+p2+p3]
		score += m["UW1:"+w1]
		score += m["UW2:"+w2]
		score += m["UW3:"+w3]
		score += m["UW4:"+w4]
		score += m["UW5:"+w5]
		score += m["UW6:"+w6]
		score += m["BW1:"+w2+w3]
		score += m["BW2:"+w3+w4]
		score += m["BW3:"+w4+w5]
		score += m["TW1:"+w1+w2+w3]
		score += m["TW2:"+w2+w3+w4]
		score += m["TW3:"+w3+w4+w5]
		score += m["TW4:"+w4+w5+w6]
		score += m["UC1:"+c1]
		score += m["UC2:"+c2]
		score += m["UC3:"+c3]
		score += m["UC4:"+c4]
		score += m["UC5:"+c5]
		score += m["UC6:"+c6]
		score += m["BC1:"+c2+c3]
		score += m["BC2:"+c3+c4]
		score += m["BC3:"+c4+c5]
		score += m["TC1:"+c1+c2+c3]
		score += m["TC2:"+c2+c3+c4]
		score += m["TC3:"+c3+c4+c5]
		score += m["TC4:"+c4+c5+c6]
		score += m["UQ1:"+p1+c1]
		score += m["UQ2:"+p2+c2]
		score += m["UQ3:"+p3+c3]
		score += m["BQ1:"+p2+c2+c3]
		score += m["BQ2:"+p2+c3+c4]
		score += m["BQ3:"+p3+c2+c3]
		score += m["BQ4:"+p3+c3+c4]
		score += m["TQ1:"+p2+c1+c2+c3]
		score += m["TQ2:"+p2+c2+c3+c4]
		score += m["TQ3:"+p3+c1+c2+c3]
		score += m["TQ4:"+p3+c2+c3+c4]
		p := "O"
		if score > 0 {
			result = append(result, strings.Join(word, ""))
			word = word[0:0]
			p = "S"
		}
		p1, p2, p3 = p2, p3, p
		word = append(word, seg[i])
	}
	result = append(result, strings.Join(word, ""))

	return result
}
