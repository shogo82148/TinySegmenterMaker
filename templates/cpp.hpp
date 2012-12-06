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

    static const char* ctype_(const std::string& ch) {
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

    std::vector<std::string> result, ctype, seg;
    std::string word;
    std::string tmp;

public:
    std::vector<std::string> segment(const std::string& s) {
        if(s.empty()) return std::vector<std::string>();
        result.resize(0);
        ctype.resize(0);
        seg.resize(0);

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

        word = seg[3];
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
            tmp = p1; tmp += p2; score += BP1(tmp);
            tmp = p2; tmp += p3; score += BP2(tmp);
            score += UW1(w1);
            score += UW2(w2);
            score += UW3(w3);
            score += UW4(w4);
            score += UW5(w5);
            score += UW6(w6);
            tmp = w1; tmp += w2; tmp += w3; score += TW1(tmp);
            tmp = w2; tmp += w3; score += BW1(tmp);
            tmp += w4; score += TW2(tmp);
            tmp = w3; tmp += w4; score += BW2(tmp);
            tmp += w5; score += TW3(tmp);
            tmp = w4; tmp += w5; score += BW3(tmp);
            tmp += w6; score += TW4(tmp);
            score += UC1(c1);
            score += UC2(c2);
            score += UC3(c3);
            score += UC4(c4);
            score += UC5(c5);
            score += UC6(c6);
            tmp = c1; tmp += c2; tmp += c3; score += TC1(tmp);
            tmp = c2; tmp += c3; score += BC1(tmp);
            tmp += c4; score += TC2(tmp);
            tmp = c3; tmp += c4; score += BC2(tmp);
            tmp += c5; score += TC3(tmp);
            tmp = c4; tmp += c5; score += BC3(tmp);
            tmp += c6; score += TC4(tmp);
            tmp = p1; tmp += c1; score += UQ1(tmp);
            tmp = p2; tmp += c2; score += UQ2(tmp);
            tmp += c3; score += BQ1(tmp);
            tmp += c4; score += TQ2(tmp);
            tmp = p3; tmp += c3; score += UQ3(tmp);
            tmp += c4; score += BQ4(tmp);
            tmp = p2; tmp += c3; tmp += c4; score += BQ2(tmp);
            tmp = p3; tmp += c2; tmp += c3; score += BQ3(tmp);
            tmp += c4; score += TQ4(tmp);
            tmp = p2; tmp += c1; tmp += c2; tmp += c3; score += TQ1(tmp);
            tmp = p3; tmp += c1; tmp += c2; tmp += c3; score += TQ3(tmp);
            std::string p = "O";
            if(score > 0) {
                result.push_back(word);
                word.resize(0);
                p = "B";
            }
            p1 = p2;
            p2 = p3;
            p3 = p;
            word += seg[i];
        }
        result.push_back(word);
        return result;
    }
};

#endif
