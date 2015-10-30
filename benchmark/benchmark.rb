require './tinysegmenter'
require 'benchmark'

text = ''
File.open('timemachineu8j.txt') do |f|
  text = f.read
end

Benchmark.bm 10 do |r|
  r.report "segment" do
    100.times do
      TinySegmenter.segment(text)
    end
  end
end
