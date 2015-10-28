# Copyright (c) 2008, Taku Kudo
# Copyright (c) Jiro Nishiguchi <jiro@cpan.org>
# Copyright (c) Shogo Ichinose <shogo82148@gmail.com>

package tinysegmenter;
use 5.8.1;
use strict;
use warnings;
use utf8;

our $VERSION = '0.01';

__MODEL__

sub _ctype {
    my $str = shift;
    return "M" if $str =~ /[一二三四五六七八九十百千万億兆]/;
    return "H" if $str =~ /[一-龠々〆ヵヶ]/;
    return "I" if $str =~ /[ぁ-ん]/;
    return "K" if $str =~ /[ァ-ヴーｱ-ﾝﾞｰ]/;
    return "A" if $str =~ /[a-zA-Zａ-ｚＡ-Ｚ]/;
    return "N" if $str =~ /[0-9０-９]/;

    return "O";
}

sub _ts {
    $_[0] || 0;
}

sub segment {
    my ($class, $input) = @_;
    if (!defined $input || $input eq '') {
        return wantarray ? () : [];
    }
    my @result;
    my @seg = ("B3","B2","B1");
    my @ctype = ("O","O","O");
    my @o = split //, $input;
    for my $c (@o) {
        push @seg, $c;
        push @ctype, _ctype($c);
    }
    push @seg, "E1";
    push @seg, "E2";
    push @seg, "E3";
    push @ctype, "O";
    push @ctype, "O";
    push @ctype, "O";
    my $word = $seg[3];
    my $p1 = "U";
    my $p2 = "U";
    my $p3 = "U";
    for (my $i = 4; $i < @seg - 3; ++$i) {
        my $score = $BIAS;
        my $w1 = $seg[$i-3];
        my $w2 = $seg[$i-2];
        my $w3 = $seg[$i-1];
        my $w4 = $seg[$i];
        my $w5 = $seg[$i+1];
        my $w6 = $seg[$i+2];
        my $c1 = $ctype[$i-3];
        my $c2 = $ctype[$i-2];
        my $c3 = $ctype[$i-1];
        my $c4 = $ctype[$i];
        my $c5 = $ctype[$i+1];
        my $c6 = $ctype[$i+2];
        $score += _ts($UP1{$p1});
        $score += _ts($UP2{$p2});
        $score += _ts($UP3{$p3});
        $score += _ts($BP1{$p1 . $p2});
        $score += _ts($BP2{$p2 . $p3});
        $score += _ts($UW1{$w1});
        $score += _ts($UW2{$w2});
        $score += _ts($UW3{$w3});
        $score += _ts($UW4{$w4});
        $score += _ts($UW5{$w5});
        $score += _ts($UW6{$w6});
        $score += _ts($BW1{$w2 . $w3});
        $score += _ts($BW2{$w3 . $w4});
        $score += _ts($BW3{$w4 . $w5});
        $score += _ts($TW1{$w1 . $w2 . $w3});
        $score += _ts($TW2{$w2 . $w3 . $w4});
        $score += _ts($TW3{$w3 . $w4 . $w5});
        $score += _ts($TW4{$w4 . $w5 . $w6});
        $score += _ts($UC1{$c1});
        $score += _ts($UC2{$c2});
        $score += _ts($UC3{$c3});
        $score += _ts($UC4{$c4});
        $score += _ts($UC5{$c5});
        $score += _ts($UC6{$c6});
        $score += _ts($BC1{$c2 . $c3});
        $score += _ts($BC2{$c3 . $c4});
        $score += _ts($BC3{$c4 . $c5});
        $score += _ts($TC1{$c1 . $c2 . $c3});
        $score += _ts($TC2{$c2 . $c3 . $c4});
        $score += _ts($TC3{$c3 . $c4 . $c5});
        $score += _ts($TC4{$c4 . $c5 . $c6});
        $score += _ts($UQ1{$p1 . $c1});
        $score += _ts($UQ2{$p2 . $c2});
        $score += _ts($UQ1{$p3 . $c3});
        $score += _ts($BQ1{$p2 . $c2 . $c3});
        $score += _ts($BQ2{$p2 . $c3 . $c4});
        $score += _ts($BQ3{$p3 . $c2 . $c3});
        $score += _ts($BQ4{$p3 . $c3 . $c4});
        $score += _ts($TQ1{$p2 . $c1 . $c2 . $c3});
        $score += _ts($TQ2{$p2 . $c2 . $c3 . $c4});
        $score += _ts($TQ3{$p3 . $c1 . $c2 . $c3});
        $score += _ts($TQ4{$p3 . $c2 . $c3 . $c4});
        my $p = "O";
        if ($score > 0) {
            push @result, $word;
            $word = "";
            $p = "B";
        }
        $p1 = $p2;
        $p2 = $p3;
        $p3 = $p;
        $word .= $seg[$i];
    }
    push @result, $word;

    return wantarray ? @result : \@result;
}

__END__

=head1 NAME

    Text::TinySegmenter - Super compact Japanese tokenizer

=head1 SYNOPSIS

    use Text::TinySegmenter;

  my @words = Text::TinySegmenter->segment($string);

=head1 DESCRIPTION

Perl version of TinySegmenter.

It is available at: http://chasen.org/~taku/software/TinySegmenter/

=head1 METHODS

=over 4

=item segment($string)

Segments string into words.

=back

=head1 AUTHOR

Jiro Nishiguchi E<lt>jiro@cpan.orgE<gt>

And

Taku Kudo E<lt>taku@chasen.orgE<gt>

And

Shogo Ichinose E<lt>shogo82148@gmail.comE<gt>

=head1 COPYRIGHT AND LICENSE

  Copyright (c) 2008, Taku Kudo

  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

      * Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
      * Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.
      * Neither the name of the <ORGANIZATION> nor the names of its
  contributors may be used to endorse or promote products derived from this
  software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=head1 SEE ALSO

L<http://chasen.org/~taku/software/TinySegmenter/>

=cut
