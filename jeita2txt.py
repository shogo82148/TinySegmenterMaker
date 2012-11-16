#!/usr/bin/env python
# -*- coding:utf-8 -*-

import xml.sax
import xml.sax.handler
import sys
import codecs

sys.stdout = codecs.getwriter('utf-8')(sys.stdout)

class Handler(xml.sax.handler.ContentHandler):
    def startDocument(self):
        self.text = []
        self.m = False
        self.nest = 0

    def startElement(self, name, attrs):
        self.m = (name == 'm')
        if name == 'text':
            self.nest = 0

    def endElement(self, name):
        self.m = False
        if name == 'text':
            self.outputSentence()

    def characters(self, content):
        if self.m:
            self.text.append(content)
            if self.nest == 0 and content in u'。．.!！?？':
                self.outputSentence()
            elif content in u'「『【［[〈《（(｛{':
                self.nest += 1
            elif content in u'」』】］]〉》）)｝}':
                self.nest -= 1
        return

    def outputSentence(self):
        if len(self.text) == 0:
            return
        print ' '.join(self.text)
        self.text = []

def main():
    parser = xml.sax.make_parser()
    parser.setContentHandler(Handler())
    parser.parse(sys.stdin)
    return

if __name__=="__main__":
    main()
