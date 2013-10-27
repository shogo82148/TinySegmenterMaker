// JSX version of TinySegmenter. -- Super compact Japanese tokenizer in Javascript
// Ported by Yoshiki Shibukawa under MIT license
//
// Original Code Copyright
// TinySegmenter 0.2 -- Super compact Japanese tokenizer in Javascript
// (c) 2008 Taku Kudo <taku@chasen.org>
// TinySegmenter is freely distributable under the terms of a new BSD licence.
// For details, see http://chasen.org/~taku/software/TinySegmenter/LICENCE.txt

class CharType
{
    var re : RegExp;
    var type : string;
    function constructor (pattern : RegExp, type : string)
    {
        this.re = pattern;
        this.type = type;
    }
}

class TinySegmenter
{
    static const _chartype = [
        new CharType(/[一二三四五六七八九十百千万億兆]/ ,"M"),
        new CharType(/[一-龠々〆ヵヶ]/, "H"),
        new CharType(/[ぁ-ん]/, "I"),
        new CharType(/[ァ-ヴーｱ-ﾝﾞｰ]/, "K"),
        new CharType(/[a-zA-Zａ-ｚＡ-Ｚ]/, "A"),
        new CharType(/[0-9０-９]/, "N")
    ];

    static const _BIAS = __BIAS__;
    static const _model : Map.<Map.<number>> = __MODEL__;

    static function _ctype (str : string) : string
    {
        for (var i = 0; i < TinySegmenter._chartype.length; i++)
        {
            if (str.match(TinySegmenter._chartype[i].re))
            {
                return TinySegmenter._chartype[i].type;
            }
        }
        return "O";
    }

    static function segment (input : string) : string []
    {
        function get (map : Map.<number>, key : string) : number
        {
            if (map.hasOwnProperty(key))
            {
                return map[key];
            }
            return 0;
        }
        if (input == "")
        {
            return [] : string[];
        }
        var m = TinySegmenter._model;
        var result = [] : string[];
        var seg = ["B3","B2","B1"];
        var ctype = ["O","O","O"];
        var o = input.split("");
        for (var i = 0; i < o.length; ++i)
        {
            seg.push(o[i]);
            ctype.push(TinySegmenter._ctype(o[i]));
        }
        seg.push("E1");
        seg.push("E2");
        seg.push("E3");
        ctype.push("O");
        ctype.push("O");
        ctype.push("O");
        var word = seg[3];
        var p1 = "U";
        var p2 = "U";
        var p3 = "U";
        for (var i = 4; i < seg.length - 3; ++i)
        {
            var score = TinySegmenter._BIAS;
            var w1 = seg[i-3];
            var w2 = seg[i-2];
            var w3 = seg[i-1];
            var w4 = seg[i];
            var w5 = seg[i+1];
            var w6 = seg[i+2];
            var c1 = ctype[i-3];
            var c2 = ctype[i-2];
            var c3 = ctype[i-1];
            var c4 = ctype[i];
            var c5 = ctype[i+1];
            var c6 = ctype[i+2];

            score += get(m['UP1'], p1);
            score += get(m['UP2'], p2);
            score += get(m['UP3'], p3);
            score += get(m['BP1'], p1 + p2);
            score += get(m['BP2'], p2 + p3);
            score += get(m['UW1'], w1);
            score += get(m['UW2'], w2);
            score += get(m['UW3'], w3);
            score += get(m['UW4'], w4);
            score += get(m['UW5'], w5);
            score += get(m['UW6'], w6);
            score += get(m['BW1'], w2 + w3);
            score += get(m['BW2'], w3 + w4);
            score += get(m['BW3'], w4 + w5);
            score += get(m['TW1'], w1 + w2 + w3);
            score += get(m['TW2'], w2 + w3 + w4);
            score += get(m['TW3'], w3 + w4 + w5);
            score += get(m['TW4'], w4 + w5 + w6);
            score += get(m['UC1'], c1);
            score += get(m['UC2'], c2);
            score += get(m['UC3'], c3);
            score += get(m['UC4'], c4);
            score += get(m['UC5'], c5);
            score += get(m['UC6'], c6);
            score += get(m['BC1'], c2 + c3);
            score += get(m['BC2'], c3 + c4);
            score += get(m['BC3'], c4 + c5);
            score += get(m['TC1'], c1 + c2 + c3);
            score += get(m['TC2'], c2 + c3 + c4);
            score += get(m['TC3'], c3 + c4 + c5);
            score += get(m['TC4'], c4 + c5 + c6);
            score += get(m['UQ1'], p1 + c1);
            score += get(m['UQ2'], p2 + c2);
            score += get(m['UQ3'], p3 + c3);
            score += get(m['BQ1'], p2 + c2 + c3);
            score += get(m['BQ2'], p2 + c3 + c4);
            score += get(m['BQ3'], p3 + c2 + c3);
            score += get(m['BQ4'], p3 + c3 + c4);
            score += get(m['TQ1'], p2 + c1 + c2 + c3);
            score += get(m['TQ2'], p2 + c2 + c3 + c4);
            score += get(m['TQ3'], p3 + c1 + c2 + c3);
            score += get(m['TQ4'], p3 + c2 + c3 + c4);

            var p = "O";
            if (score > 0)
            {
                result.push(word);
                word = "";
                p = "B";
            }
            p1 = p2;
            p2 = p3;
            p3 = p;
            word += seg[i];
        }
        result.push(word);
        return result;
    }
}
