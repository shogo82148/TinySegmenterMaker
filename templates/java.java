import java.util.Map;
import java.util.HashMap;
import java.util.ArrayList;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.PrintStream;

public class tinysegmenter {
    __MODEL__

    private Model model;
    public tinysegmenter() {
        model = newModel();
    }

    public tinysegmenter(Model model) {
        this.model = model;
    }

    public String[] segment(String s) {
        final Model m = model;

        if(s.length() == 0) {
            return new String[0];
        }

        ArrayList<String> result = new ArrayList<String>();
        int len = s.codePointCount(0, s.length());

        String[] seg = new String[len + 6];
        String[] ctype = new String[len + 6];
        seg[0] = "B3"; ctype[0] = "O";
        seg[1] = "B2"; ctype[1] = "O";
        seg[2] = "B1"; ctype[2] = "O";
        for(int i = 0, j = 3; i < s.length(); i = s.offsetByCodePoints(i, 1), ++j) {
            seg[j] = s.substring(i, s.offsetByCodePoints(i, 1));
            ctype[j] = ctype_(s.codePointAt(i));
        }
        seg[len+3] = "E1"; ctype[len+3] = "O";
        seg[len+4] = "E2"; ctype[len+4] = "O";
        seg[len+5] = "E3"; ctype[len+5] = "O";
        StringBuilder word = new StringBuilder();
        word.append(seg[3]);
        String p1 = "U", p2 = "U", p3 = "U";

        for(int i = 4; i < seg.length - 3; ++i) {
            int score = m.BIAS;
            final String w1 = seg[i-3];
            final String w2 = seg[i-2];
            final String w3 = seg[i-1];
            final String w4 = seg[i];
            final String w5 = seg[i+1];
            final String w6 = seg[i+2];
            final String c1 = ctype[i-3];
            final String c2 = ctype[i-2];
            final String c3 = ctype[i-1];
            final String c4 = ctype[i];
            final String c5 = ctype[i+1];
            final String c6 = ctype[i+2];

            score += getScore(m.UP1, p1);
            score += getScore(m.UP2, p2);
            score += getScore(m.UP3, p3);
            score += getScore(m.BP1, p1 + p2);
            score += getScore(m.BP2, p2 + p3);
            score += getScore(m.UW1, w1);
            score += getScore(m.UW2, w2);
            score += getScore(m.UW3, w3);
            score += getScore(m.UW4, w4);
            score += getScore(m.UW5, w5);
            score += getScore(m.UW6, w6);
            score += getScore(m.BW1, w2 + w3);
            score += getScore(m.BW2, w3 + w4);
            score += getScore(m.BW3, w4 + w5);
            score += getScore(m.TW1, w1 + w2 + w3);
            score += getScore(m.TW2, w2 + w3 + w4);
            score += getScore(m.TW3, w3 + w4 + w5);
            score += getScore(m.TW4, w4 + w5 + w6);
            score += getScore(m.UC1, c1);
            score += getScore(m.UC2, c2);
            score += getScore(m.UC3, c3);
            score += getScore(m.UC4, c4);
            score += getScore(m.UC5, c5);
            score += getScore(m.UC6, c6);
            score += getScore(m.BC1, c2 + c3);
            score += getScore(m.BC2, c3 + c4);
            score += getScore(m.BC3, c4 + c5);
            score += getScore(m.TC1, c1 + c2 + c3);
            score += getScore(m.TC2, c2 + c3 + c4);
            score += getScore(m.TC3, c3 + c4 + c5);
            score += getScore(m.TC4, c4 + c5 + c6);
            score += getScore(m.UQ1, p1 + c1);
            score += getScore(m.UQ2, p2 + c2);
            score += getScore(m.UQ3, p3 + c3);
            score += getScore(m.BQ1, p2 + c2 + c3);
            score += getScore(m.BQ2, p2 + c3 + c4);
            score += getScore(m.BQ3, p3 + c2 + c3);
            score += getScore(m.BQ4, p3 + c3 + c4);
            score += getScore(m.TQ1, p2 + c1 + c2 + c3);
            score += getScore(m.TQ2, p2 + c2 + c3 + c4);
            score += getScore(m.TQ3, p3 + c1 + c2 + c3);
            score += getScore(m.TQ4, p3 + c2 + c3 + c4);

            String  p = "O";
            if(score > 0) {
                result.add(word.toString());
                word.setLength(0);
                p = "B";
            }
            p1 = p2;
            p2 = p3;
            p3 = p;
            word.append(seg[i]);
        }

        result.add(word.toString());

        String[] array = new String[result.size()];
        result.toArray(array);
        return array;
    }

    private static String ctype_(int codePoint) {
        if(codePoint == 0x4e00 || codePoint == 0x4e8c ||
           codePoint == 0x4e09 || codePoint == 0x56db ||
           codePoint == 0x4e94 || codePoint == 0x516d ||
           codePoint == 0x4e03 || codePoint == 0x516b ||
           codePoint == 0x4e5d || codePoint == 0x5341 ||
           codePoint == 0x767e || codePoint == 0x5343 ||
           codePoint == 0x4e07 || codePoint == 0x5104 ||
           codePoint == 0x5146) return "M";

        if(0x4e00 <= codePoint && codePoint <= 0x9fa0 ||
           codePoint == 0x3005 || codePoint == 0x3006 ||
           codePoint == 0x30f5 || codePoint == 0x30f6) return "H";

        if(0x3041 <= codePoint && codePoint <= 0x3093) return "I";

        if(0x30a1 <= codePoint && codePoint <= 0x30f4 ||
           codePoint == 0x30fc ||
           0xff71 <= codePoint && codePoint <= 0xff9d ||
           codePoint == 0xff9e || codePoint == 0xff70) return "K";

        if(0x61 <= codePoint && codePoint <= 0x7a ||
           0x41 <= codePoint && codePoint <= 0x5a ||
           0xff41 <= codePoint && codePoint <= 0xff5a ||
           0xff21 <= codePoint && codePoint <= 0xff3a) return "A";

        if(0x30 <= codePoint && codePoint <= 0x39 ||
           0xff10 <= codePoint && codePoint <= 0xff19) return "N";

        return "O";
    }

    private static int getScore(Map<String, Integer> m, String key) {
        Integer val = m.get(key);
        return val != null ? val : 0;
    }

    public static void main(String[] args) throws Exception {
        String encoding = "utf-8";
        tinysegmenter segmenter = new tinysegmenter();
        BufferedReader stdReader =
            new BufferedReader(new InputStreamReader(System.in, encoding));
        PrintStream out = new PrintStream(System.out, true, encoding);
        String line;
        while ((line = stdReader.readLine()) != null) {
            if (line.equals("")) continue;
            String[] s = segmenter.segment(line);
            for(int i = 0; i < s.length; ++i) {
                out.println(s[i]);
            }
        }
        stdReader.close();
    }
}
