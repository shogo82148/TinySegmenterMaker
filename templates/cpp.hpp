#ifndef TINYSEGMENTER_HPP__
#define TINYSEGMENTER_HPP__

#include <string>
#include <vector>

class TinySegmeter {
protected:
    __MODEL__
    static unsigned int lengthOfCharactor(int ch) {
        if((ch & 0x80) == 0) {
            return 1;
        } else if((ch & 0xE0) == 0xC0) {
            return 2;
        } else if((ch & 0xF0) == 0xE0) {
            return 3;
        } else if((ch && 0xF8) == 0xF0) {
            return 4;
        }
        return 0;
    }

    static std::string ctype_(const std::string& ch) {
        // [一二三四五六七八九十百千万億兆]
        if(ch.size() == 3 && (
               ch[0] == '\xe4' && (
                   ch[1] == '\xb8' && (ch[2] == '\x80' || ch[2] == '\x83' || ch[2] == '\x87' || ch[2] == '\x89') ||
                   ch[1] == '\xb9' && ch[2] == '\x9d' ||
                   ch[1] == '\xba' && ch[2] == '\x94') ||
               ch[0] == '\xe5' && (
                   ch[1] == '\x84' && ch[2] == '\x84' ||
                   ch[1] == '\x85' && (ch[2] == '\x86' || ch[2] == '\xab' || ch[2] == '\xad') ||
                   ch[1] == '\x8d' && (ch[2] == '\x81' || ch[2] == '\x83') ||
                   ch[1] == '\x9b' && ch[2] == '\x9b') ||
               ch[0] == '\xe7' && ch[1] == '\x99' && ch[2] == '\xbe')) return "M";

        // [一-龠々〆ヵヶ]
        if("\xe4\xb8\x80" <= ch && ch <= "\xe9\xbe\xa0" ||
               ch.size() == 3 && ch[0] == '\xe3' && (
                       ch[1] == '\x80' && (ch[2] == '\x85' || ch[2] == '\x86') ||
                       ch[1] == '\x83' && (ch[2] == '\xb5' || ch[2] == '\xb6'))) return "H";

        // [ぁ-ん]
        if("\xe3\x81\x81" <= ch && ch <= "\xe3\x82\x93") return "I";

        //[ァ-ヴーｱ-ﾝﾞｰ]
        if("\xe3\x82\xa1" <= ch && ch <= "\xe3\x83\xb4" || ch == "\xe3\x83\xbc" ||
           "\xef\xbd\xb1" <= ch && ch <= "\xef\xbe\x9d" || ch == "\xef\xbe\x9e" || ch == "\xef\xbd\xb0") return "K";

        //[a-zA-Zａ-ｚＡ-Ｚ]
        if("a" <= ch && ch <= "z" ||
           "A" <= ch && ch <= "Z" ||
           "\xef\xbd\x81" <= ch && ch <= "\xef\xbd\x9a" ||
           "\xef\xbc\xa1" <= ch && ch <= "\xef\xbc\xba") return "A";

        //[0-9０-９]
        if("0" <= ch && ch <= "9" ||
           "\xef\xbc\x90" <= ch && ch <= "\xef\xbc\x99") return "N";

        return "O";
    }
public:
    std::vector<std::string> segment(const std::string& s) const {
        if(s.empty()) return std::vector<std::string>();

        std::vector<std::string> result, ctype, seg;
        seg.push_back("B3"); ctype.push_back("O");
        seg.push_back("B2"); ctype.push_back("O");
        seg.push_back("B1"); ctype.push_back("O");
        for(unsigned int i = 0; i < s.size();) {
            unsigned int len = lengthOfCharactor(s[i]);
            if(len==0) throw "Invalid Charactor";
            std::string ch = s.substr(i, len);
            seg.push_back(ch);
            ctype.push_back(ctype_(ch));
            i += len;
        }
        seg.push_back("E1"); ctype.push_back("O");
        seg.push_back("E2"); ctype.push_back("O");
        seg.push_back("E3"); ctype.push_back("O");

        std::string word = seg[3];
        std::string p1 = "U", p2 = "U", p3 = "U";

        for(unsigned int i = 4; i<seg.size()-3; ++i) {
            int score = BIAS();
            const std::string& w1 = seg[i-3];
            const std::string& w2 = seg[i-2];
            const std::string& w3 = seg[i-1];
            const std::string& w4 = seg[i];
            const std::string& w5 = seg[i+1];
            const std::string& w6 = seg[i+2];
            const std::string& c1 = ctype[i-3];
            const std::string& c2 = ctype[i-2];
            const std::string& c3 = ctype[i-1];
            const std::string& c4 = ctype[i];
            const std::string& c5 = ctype[i+1];
            const std::string& c6 = ctype[i+2];
            score += UP1(p1);
            score += UP2(p2);
            score += UP3(p3);
            score += BP1(p1 + p2);
            score += BP2(p2 + p3);
            score += UW1(w1);
            score += UW2(w2);
            score += UW3(w3);
            score += UW4(w4);
            score += UW5(w5);
            score += UW6(w6);
            score += BW1(w2 + w3);
            score += BW2(w3 + w4);
            score += BW3(w4 + w5);
            score += TW1(w1 + w2 + w3);
            score += TW2(w2 + w3 + w4);
            score += TW3(w3 + w4 + w5);
            score += TW4(w4 + w5 + w6);
            score += UC1(c1);
            score += UC2(c2);
            score += UC3(c3);
            score += UC4(c4);
            score += UC5(c5);
            score += UC6(c6);
            score += BC1(c2 + c3);
            score += BC2(c3 + c4);
            score += BC3(c4 + c5);
            score += TC1(c1 + c2 + c3);
            score += TC2(c2 + c3 + c4);
            score += TC3(c3 + c4 + c5);
            score += TC4(c4 + c5 + c6);
            score += UQ1(p1 + c1);
            score += UQ2(p2 + c2);
            score += UQ3(p3 + c3);
            score += BQ1(p2 + c2 + c3);
            score += BQ2(p2 + c3 + c4);
            score += BQ3(p3 + c2 + c3);
            score += BQ4(p3 + c3 + c4);
            score += TQ1(p2 + c1 + c2 + c3);
            score += TQ2(p2 + c2 + c3 + c4);
            score += TQ3(p3 + c1 + c2 + c3);
            score += TQ4(p3 + c2 + c3 + c4);
            std::string p = "O";
            if(score > 0) {
                result.push_back(word);
                word.resize(0);
                p = "B";
            }
            p1 = p2;
            p2 = p3;
            p3 = p;
            word = word + seg[i];
        }
        result.push_back(word);
        return result;
    }
};

#endif
