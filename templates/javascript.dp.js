// TinySegmenter 0.1 -- Super compact Japanese tokenizer in Javascript
// (c) 2008 Taku Kudo <taku@chasen.org>
// TinySegmenter is freely distributable under the terms of a new BSD licence.
// For details, see http://chasen.org/~taku/software/TinySegmenter/LICENCE.txt

(function(global) {
    global.TinySegmenter = TinySegmenter;
    var default_model = __MODEL__;
    function TinySegmenter(model) {
        this.model = model || default_model;
    }

    var patterns = {
        "[一二三四五六七八九十百千万億兆]":"M",
        "[一-龠々〆ヵヶ]":"H",
        "[ぁ-ん]":"I",
        "[ァ-ヴーｱ-ﾝﾞｰ]":"K",
        "[a-zA-Zａ-ｚＡ-Ｚ]":"A",
        "[0-9０-９]":"N"
    };
    var chartype = [];
    for (var i in patterns) {
        var regexp = new RegExp;
        regexp.compile(i);
        chartype.push([regexp, patterns[i]]);
    }

    function getctype(str) {
        for (var i in chartype) {
            if (str.match(chartype[i][0])) {
                return chartype[i][1];
            }
        }
        return "O";
    }

    TinySegmenter.prototype.segment = function(input) {
        if (input == null || input == undefined || input == "") {
            return [];
        }
        var m = this.model;
        var result = [];
        var seg = ["B3","B2","B1"];
        var ctype = ["O","O","O"];
        var o = input.split("");
        for (i = 0; i < o.length; ++i) {
            seg.push(o[i]);
            ctype.push(getctype(o[i]));
        }
        seg.push("E1");
        seg.push("E2");
        seg.push("E3");
        ctype.push("O");
        ctype.push("O");
        ctype.push("O");
        var nodes = {'UUU':{score:0, prev:null, pos:3, p: 'B'}};
        for (var i = 4; i < seg.length - 3; ++i) {
            var next_nodes = {};
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
            var base = m.BIAS;
            base += m.UW1[w1]||0;
            base += m.UW2[w2]||0;
            base += m.UW3[w3]||0;
            base += m.UW4[w4]||0;
            base += m.UW5[w5]||0;
            base += m.UW6[w6]||0;
            base += m.BW1[w2 + w3]||0;
            base += m.BW2[w3 + w4]||0;
            base += m.BW3[w4 + w5]||0;
            base += m.TW1[w1 + w2 + w3]||0;
            base += m.TW2[w2 + w3 + w4]||0;
            base += m.TW3[w3 + w4 + w5]||0;
            base += m.TW4[w4 + w5 + w6]||0;
            base += m.UC1[c1]||0;
            base += m.UC2[c2]||0;
            base += m.UC3[c3]||0;
            base += m.UC4[c4]||0;
            base += m.UC5[c5]||0;
            base += m.UC6[c6]||0;
            base += m.BC1[c2 + c3]||0;
            base += m.BC2[c3 + c4]||0;
            base += m.BC3[c4 + c5]||0;
            base += m.TC1[c1 + c2 + c3]||0;
            base += m.TC2[c2 + c3 + c4]||0;
            base += m.TC3[c3 + c4 + c5]||0;
            base += m.TC4[c4 + c5 + c6]||0;
            for(var key in nodes) {
                var p1 = key.charAt(0);
                var p2 = key.charAt(1);
                var p3 = key.charAt(2);
                var score = base;
                score += m.UP1[p1]||0;
                score += m.UP2[p2]||0;
                score += m.UP3[p3]||0;
                score += m.BP1[p1 + p2]||0;
                score += m.BP2[p2 + p3]||0;
                score += m.UQ1[p1 + c1]||0;
                score += m.UQ2[p2 + c2]||0;
                score += m.UQ3[p3 + c3]||0;
                score += m.BQ1[p2 + c2 + c3]||0;
                score += m.BQ2[p2 + c3 + c4]||0;
                score += m.BQ3[p3 + c2 + c3]||0;
                score += m.BQ4[p3 + c3 + c4]||0;
                score += m.TQ1[p2 + c1 + c2 + c3]||0;
                score += m.TQ2[p2 + c2 + c3 + c4]||0;
                score += m.TQ3[p3 + c1 + c2 + c3]||0;
                score += m.TQ4[p3 + c2 + c3 + c4]||0;

                var next_key, next_score;

                next_key = p2 + p3 + 'O';
                next_score = nodes[key].score - score;
                if(!next_nodes[next_key] || next_nodes[next_key].score < next_score) {
                    next_nodes[next_key] = {score: next_score, prev: nodes[key], pos: i, p: 'O'};
                }

                next_key = p2 + p3 + 'B';
                next_score = nodes[key].score + score;
                if(!next_nodes[next_key] || next_nodes[next_key].score < next_score) {
                    next_nodes[next_key] = {score: next_score, prev: nodes[key], pos: i, p: 'B'};
                }
            }
            nodes = next_nodes;
        }

        var max_score = -1e100;
        var max_key = undefined;
        for(key in nodes) {
            if(nodes[key].score > max_score) {
                max_score = nodes[key].score;
                max_key = key;
            }
        }
        var node = nodes[max_key];
        var word = '';
        while(node) {
            word = seg[node.pos] + word;
            if(node.p=='B') {
                result.push(word);
                word = '';
            }
            node = node.prev;
        }
        result.reverse();
        return result;
    };
})(this);

