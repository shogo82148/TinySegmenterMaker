import time
import tinysegmenter

segmenter = tinysegmenter.TinySegmenter()
f = open('timemachineu8j.txt')
text = f.read().decode('utf-8')

start = time.time()
for _ in range(100):
    segmenter.segment(text)
elapsed_time = time.time() - start
print ("elapsed_time:{0}".format(elapsed_time)) + "[sec]"
