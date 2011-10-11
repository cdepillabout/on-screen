#!/usr/bin/env python3

from distutils.core import setup
from distutils.extension import Extension

setup(name='installkernel',
      version='1.0',
      description='program to run programs with different options on different monitors',
      author='(cdep) illabout',
      author_email='cdep.illabout@gmail.com',
      #url='http://www.python.org/sigs/distutils-sig/',
      scripts=['scripts/on-screen'],
      ext_modules=[Extension('getscreen',
                             ['src/getscreenmodule.c'],
                             libraries=['X11', 'Xinerama'])],
     )


