FILE=$1
echo "set mouse; plot '${FILE}' with linespoints; pause mouse" | gnuplot
