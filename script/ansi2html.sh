#!/bin/sh

# http://www.pixelbeat.org/docs/terminal_colours/

echo -n "
<head>
<style type=\"text/css\">
/* linux console palette */
.f0 { color: #000000; }
.f1 { color: #AA0000; }
.f2 { color: #00AA00; }
.f3 { color: #AA5500; }
.f4 { color: #0000AA; }
.f5 { color: #AA00AA; }
.f6 { color: #00AAAA; }
.f7 { color: #AAAAAA; }
.bf0 { color: #555555; }
.bf1 { color: #FF5555; }
.bf2 { color: #55FF55; }
.bf3 { color: #FFFF55; }
.bf4 { color: #5555FF; }
.bf5 { color: #FF55FF; }
.bf6 { color: #55FFFF; }
.bf7 { color: #FFFFFF; }
.b0 { background-color: #000000; }
.b1 { background-color: #AA0000; }
.b2 { background-color: #00AA00; }
.b3 { background-color: #AA5500; }
.b4 { background-color: #0000AA; }
.b5 { background-color: #AA00AA; }
.b6 { background-color: #00AAAA; }
.b7 { background-color: #AAAAAA; }
</style>
</head>

<html>
<body>

<pre>
"

cat -v |
#first line normalizes codes a little
sed '
s#\^\[\[1m\^\[\[\([34][0-7]m\)#^[[1;\1#; s#\^\[\[m#^[[0m#g; s#\^\[(B##g;
s#\^\[\[0m#</span>#g;
s#\^\[\[3\([0-7]\);4\([0-7]\)m#<span class=" f\1 b\2">#g;
s#\^\[\[1;3\([0-7]\);4\([0-7]\)m#<span class="bf\1 b\2">#g;
s#\^\[\[3\([0-7]\)m#<span class=" f\1">#g;
s#\^\[\[1;3\([0-7]\)m#<span class="bf\1">#g;'

echo "</pre>
</body>
</html>
"
