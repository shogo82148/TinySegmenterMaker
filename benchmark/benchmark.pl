use tinysegmenter;
use Encode;

open my $fh, '<', 'timemachineu8j.txt';
my $text = decode_utf8(do { local $/; <$fh> });

my $start = time;
for (1..100) {
    tinysegmenter->segment($text);
}
my $end = time;

printf "%d sec\n", $end-$start;
