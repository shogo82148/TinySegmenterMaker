#!/usr/bin/env python

import sys
import codecs
import re

sys.stdin = codecs.getreader('utf-16')(sys.stdin)
sys.stdout = codecs.getwriter('utf-8')(sys.stdout)

mail = re.compile('<([a-zA-Z0-9.]+@[a-zA-Z0-9.]+)>')
url = re.compile('<(https?://.*?)>')
for line in sys.stdin:
    line = line.strip()
    line = line.replace('&', '&amp;')
    line = mail.sub('&lt;\\1&gt;', line)
    line = url.sub('&lt;\\1&gt;', line)
    print line
