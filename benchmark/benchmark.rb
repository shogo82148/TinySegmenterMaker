require './tinysegmenter'
require 'benchmark'

text = ''
File.open('timemachineu8j.txt') do |f|
  text = f.read
end

ts = TinySegmenter.new

Benchmark.bm 10 do |r|
  r.report "segment" do
    100.times do
      ts.segment(text)
    end
  end
end
