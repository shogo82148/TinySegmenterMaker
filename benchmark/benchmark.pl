use tinysegmenter;

open my $fh, '<', 'timemachineu8j.txt';
my $text = do { local $/; <$fh> };

warn "Perl version is too slow... so only repeat 10 times\n";
my $start = time;
for (1..10) {
    tinysegmenter->segment($text);
}
my $end = time;

printf "%d sec\n", $end-$start;
