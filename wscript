#!/usr/bin/env python

import sferes

def options(blah) : pass

def configure(blah): pass

def build(bld):
    bld.program(features='cxx',
                use='sferes2',
                uselib='BOOST TBB',
                includes=". ../../src ../ ../../ ../../../",
                source='test_cvt_map_elites.cpp',
                target='test_cvt_map_elites'
                )
