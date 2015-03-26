// C# version of TinySegmenter. -- Super compact Japanese tokenizer in Javascript
// Ported by Yoshiki Shibukawa under MIT license
//
// Original Code Copyright
// TinySegmenter 0.2 -- Super compact Japanese tokenizer in Javascript
// (c) 2008 Taku Kudo <taku@chasen.org>
// TinySegmenter is freely distributable under the terms of a new BSD licence.
// For details, see http://chasen.org/~taku/software/TinySegmenter/LICENCE.txt

using System;
using System.Collections.Generic;
using System.Text.RegularExpressions;

class tinysegmenter
{
    private class _Model
    {
        public const int Bias = __BIAS__;

__MODEL__
    }

    private static readonly Regex _CTypeRegexM = new Regex("[一二三四五六七八九十百千万億兆]", RegexOptions.Compiled);
    private static readonly Regex _CTypeRegexH = new Regex("[一-龠々〆ヵヶ]", RegexOptions.Compiled);
    private static readonly Regex _CTypeRegexI = new Regex("[ぁ-ん]", RegexOptions.Compiled);
    private static readonly Regex _CTypeRegexK = new Regex("[ァ-ヴーｱ-ﾝﾞｰ]", RegexOptions.Compiled);
    private static readonly Regex _CTypeRegexA = new Regex("[a-zA-Zａ-ｚＡ-Ｚ]", RegexOptions.Compiled);
    private static readonly Regex _CTypeRegexN = new Regex("[0-9０-９]", RegexOptions.Compiled);

    private string _CType(string str)
    {
        if (_CTypeRegexM.IsMatch(str)) return "M";
        else if (_CTypeRegexH.IsMatch(str)) return "H";
        else if (_CTypeRegexI.IsMatch(str)) return "I";
        else if (_CTypeRegexK.IsMatch(str)) return "K";
        else if (_CTypeRegexA.IsMatch(str)) return "A";
        else if (_CTypeRegexN.IsMatch(str)) return "N";

        return "O";
    }

    public List<string> Segment(string input)
    {
        List<string> result = new List<string>();

        if (string.IsNullOrEmpty(input))
            return result;

        List<string> seg = new List<string>();
        seg.Add("B3");
        seg.Add("B2");
        seg.Add("B1");
        List<string> ctype = new List<string>();
        ctype.Add("O");
        ctype.Add("O");
        ctype.Add("O");
        for (int i = 0; i < input.Length; i++)
        {
            string c = input[i].ToString();
            seg.Add(c);
            ctype.Add(_CType(c));
        }
        seg.Add("E1");
        seg.Add("E2");
        seg.Add("E3");
        ctype.Add("O");
        ctype.Add("O");
        ctype.Add("O");
        string word = seg[3];
        string p1 = "U";
        string p2 = "U";
        string p3 = "U";
        for (int i = 4; i < seg.Count - 3; i++)
        {
            int score = _Model.Bias;
            string w1 = seg[i - 3];
            string w2 = seg[i - 2];
            string w3 = seg[i - 1];
            string w4 = seg[i];
            string w5 = seg[i + 1];
            string w6 = seg[i + 2];
            string c1 = ctype[i - 3];
            string c2 = ctype[i - 2];
            string c3 = ctype[i - 1];
            string c4 = ctype[i];
            string c5 = ctype[i + 1];
            string c6 = ctype[i + 2];
            int inc;
            _Model.UP1.TryGetValue(p1, out inc); score += inc;
            _Model.UP2.TryGetValue(p2, out inc); score += inc;
            _Model.UP3.TryGetValue(p3, out inc); score += inc;
            _Model.BP1.TryGetValue(p1 + p2, out inc); score += inc;
            _Model.BP2.TryGetValue(p2 + p3, out inc); score += inc;
            _Model.UW1.TryGetValue(w1, out inc); score += inc;
            _Model.UW2.TryGetValue(w2, out inc); score += inc;
            _Model.UW3.TryGetValue(w3, out inc); score += inc;
            _Model.UW4.TryGetValue(w4, out inc); score += inc;
            _Model.UW5.TryGetValue(w5, out inc); score += inc;
            _Model.UW6.TryGetValue(w6, out inc); score += inc;
            _Model.BW1.TryGetValue(w2 + w3, out inc); score += inc;
            _Model.BW2.TryGetValue(w3 + w4, out inc); score += inc;
            _Model.BW3.TryGetValue(w4 + w5, out inc); score += inc;
            _Model.TW1.TryGetValue(w1 + w2 + w3, out inc); score += inc;
            _Model.TW2.TryGetValue(w2 + w3 + w4, out inc); score += inc;
            _Model.TW3.TryGetValue(w3 + w4 + w5, out inc); score += inc;
            _Model.TW4.TryGetValue(w4 + w5 + w6, out inc); score += inc;
            _Model.UC1.TryGetValue(c1, out inc); score += inc;
            _Model.UC2.TryGetValue(c2, out inc); score += inc;
            _Model.UC3.TryGetValue(c3, out inc); score += inc;
            _Model.UC4.TryGetValue(c4, out inc); score += inc;
            _Model.UC5.TryGetValue(c5, out inc); score += inc;
            _Model.UC6.TryGetValue(c6, out inc); score += inc;
            _Model.BC1.TryGetValue(c2 + c3, out inc); score += inc;
            _Model.BC2.TryGetValue(c3 + c4, out inc); score += inc;
            _Model.BC3.TryGetValue(c4 + c5, out inc); score += inc;
            _Model.TC1.TryGetValue(c1 + c2 + c3, out inc); score += inc;
            _Model.TC2.TryGetValue(c2 + c3 + c4, out inc); score += inc;
            _Model.TC3.TryGetValue(c3 + c4 + c5, out inc); score += inc;
            _Model.TC4.TryGetValue(c4 + c5 + c6, out inc); score += inc;
            _Model.UQ1.TryGetValue(p1 + c1, out inc); score += inc;
            _Model.UQ2.TryGetValue(p2 + c2, out inc); score += inc;
            _Model.UQ1.TryGetValue(p3 + c3, out inc); score += inc;
            _Model.BQ1.TryGetValue(p2 + c2 + c3, out inc); score += inc;
            _Model.BQ2.TryGetValue(p2 + c3 + c4, out inc); score += inc;
            _Model.BQ3.TryGetValue(p3 + c2 + c3, out inc); score += inc;
            _Model.BQ4.TryGetValue(p3 + c3 + c4, out inc); score += inc;
            _Model.TQ1.TryGetValue(p2 + c1 + c2 + c3, out inc); score += inc;
            _Model.TQ2.TryGetValue(p2 + c2 + c3 + c4, out inc); score += inc;
            _Model.TQ3.TryGetValue(p3 + c1 + c2 + c3, out inc); score += inc;
            _Model.TQ4.TryGetValue(p3 + c2 + c3 + c4, out inc); score += inc;
            string p = "O";
            if (score > 0)
            {
                result.Add(word);
                word = "";
                p = "B";
            }
            p1 = p2;
            p2 = p3;
            p3 = p;
            word += seg[i];
        }
        result.Add(word);

        return result;
    }
/*
    static void Main(string[] args)
    {
        tinysegmenter segmenter = new tinysegmenter();

        string line;
        while ((line = Console.In.ReadLine()) != null)
        {
            List<string> segments = segmenter.Segment(line);

            for(int i = 0; i < segments.Count; i++)
            {
                Console.WriteLine(segments[i]);
            }
        }
    }
*/
}
