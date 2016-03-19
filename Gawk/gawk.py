#!/usr/bin/env python

import os
import re
import sys
import ConfigParser

def log(msg, *args):
    if args:
        msg = msg % args
    print(msg)

class Token:
    def __init__(self, name, ptn):
        self.name = name
        self.ptn = ptn

class Value(Token):
    def __init__(self, name, ptn):
        self.name = name
        self.ptn = ptn

def compile_syntax(fn):
    cfg = ConfigParser.RawConfigParser()
    cfg.read(fn)
    get_options = lambda section: cfg.options(section) if cfg.has_section(section) else []
    token_names, value_names, space_names = map(get_options, ('token', 'value', 'space'))
    syntax_names, root_names = map(get_options, ('syntax', 'root'))
    assert len(root_names) == 1

def main():
    if len(sys.argv) == 2:
        compile_syntax(sys.argv[1])

if __name__ == '__main__':
    main()

