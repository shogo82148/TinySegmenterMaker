TinySegmenterMaker
=====

[TinySegmenter](http://chasen.org/~taku/software/TinySegmenter/)用の学習モデルを自作するためのツール．

## 学習方法

スペースで分かち書きしたコーパスをあらかじめ準備しておきます．
コーパスから分かち書きの情報と素性を取り出します．

``` bash
$ ./extract < corpus.txt > features.txt
```

AdaBoostを用いて学習します．
新しい弱分類器の分類精度が0.001以下，繰り返し回数が10000回以上となったら学習を終了します．

``` bash
$ g++ -O3 -o train train.cpp # コンパイル
$ ./train -t 0.001 -n 10000 features.txt model # 学習
```

きちんと分割できるが実際に試してみます．

``` bash
$ ./segment model
私の名前は中野です
私 の 名前 は 中野 です
```

## 学習済みモデル

JEITA\_Genpaku\_ChaSen\_IPAdic.model は
電子情報技術産業協会(JEITA)が公開している[形態素解析済みコーパス](http://anlp.jp/NLP_Portal/jeita_corpus/index.html)
を学習したものです．
[プロジェクト杉田玄白](http://www.genpaku.org/)を茶筌+IPAdicで解析したものを使用しました．

RWCP.model は オリジナルの [TinySegmenter](http://chasen.org/~taku/software/TinySegmenter/) から
モデルの部分のみを取り出したものです．


## 再学習

学習済みのモデルとコーパスから学習を再開し，性能を向上させることができます．
コーパスは学習に使用したものを想定していますが，別のコーパスを使っても動作はするはずです．

``` bash
$ ./train -t 0.0001 -n 10000 -M model features.txt model_new
```

## ライブラリの作成

makerコマンドで各種言語用のライブラリを作れます．
allを指定することで，対応しているすべての言語向けのライブラリを出力します．

``` bash
$ ./maker javascript < model
$ ./maker perl < model
$ ./maker ruby < medel
$ ./maker python < model
$ ./maker cpp < model
$ ./maker all < model # 上のライブラリをすべて作成します
```


### JavaScript

``` javascript
<script src="tinysegmenter.js"></script>
<script>

var segmenter = new TinySegmenter();                 // インスタンス生成

var segs = segmenter.segment("私の名前は中野です");  // 単語の配列が返る

alert(segs.join(" | "));  // 表示

</script>
```

もちろん node.js でも使えます．

``` javascript
var TinySegmenter = require('./tinysegmenter.js').TinySegmenter;

var segmenter = new TinySegmenter();                 // インスタンス生成

var segs = segmenter.segment("私の名前は中野です");  // 単語の配列が返る

console.log(segs);
```

### Perl

``` perl
use utf8;
use tinysegmenter;

my $str = '私の名前は中野です';
my @words = tinysegmenter->segment($str);
```

### Ruby

``` ruby
require './tinysegmenter'
puts TinySegmenter.segment("私の名前は中野です");
```

### Python

``` python
from tinysegmenter import TinySegmenter
segmenter = TinySegmenter()
print segmenter.segment(u'私の名前は中野です')
```

### C++

``` c++
#include <iostream>
#include <string>
#include <vector>
#include "tinysegmenter.hpp"

using namespace std;

int main() {
    TinySegmenter segmenter;
    string s = "私の名前は中野です";
    vector<string> v = segmenter.segment(s);
    for(int i = 0; i < v.size(); ++i) {
        cout << v[i] << endl;
    }
}
```
