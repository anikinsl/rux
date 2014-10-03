rux
===

Hi! I''m glad to present you cyrillic text viewer and code page converter. This is
very simple but not less powerful utility.

*** Install

On BSD-systems simply run `make all install clean'. On other try `cc -lm -o rux
rux.c cpdetect.c'.

*** Description

The rux is a simple and at the same time powerful utility for texts recoding
from one encoding to another. At present time several the most popular Cyrillic
code pages such as koi8-r, cp866, cp1251, iso8859-5 and mac-cyrillic are
supported. Also rux gives the opportunity to recode box- drawing characters,
which exists in some Cyrillic code pages, to replace them by their non-graphic
analogues ('-', '+', etc.).

Besides, the rux can detect a code page of the input files automatically.

Usually rux is used without any options because some values are accepted by
default. There are -o koi8-r, -e, -t.

The expectancy value of the presence of every Cyrillic characters in the some
text were taken from the dump of Russian translation of the `FreeBSD Handbook'.

*** News

v Added partialy UTF-8 support.
v Corrected author's name in help message.
v Moved distribution files to rux-0.79 directory.
v Debug code was not removed from this release. It switchs on by DEBUG flag
  while compiling. Debug code contains memory debug.

- Vyacheslav Anikin (ansl)
anikinsl@gmail.com
