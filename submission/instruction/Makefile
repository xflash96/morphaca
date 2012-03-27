direct:
	../program/morphaca/morph/morph ../test/source/01-1m.jpg ../test/destination/06-1m.jpg -SL ../test/source/01-1m.asf -DL ../test/destination/06-1m.asf -T 5
	mv *.png ../test/result
bilinear:
	../program/morphaca/morph/morph ../test/source/01-1m.jpg ../test/destination/06-1m.jpg -SL ../test/source/01-1m.asf -DL ../test/destination/06-1m.asf -T 5 -t 1
	mv *.png ../test/result
gaussian:
	../program/morphaca/morph/morph ../test/source/01-1m.jpg ../test/destination/06-1m.jpg -SL ../test/source/01-1m.asf -DL ../test/destination/06-1m.asf -T 5 -t 2
	mv *.png ../test/result
poly:
	../program/morphaca/polymorph/polymorph 3 ../test/poly/01-1m.jpg ../test/poly/06-1m.jpg ../test/poly/07-1m.jpg ../test/poly/01-1m.asf ../test/poly/06-1m.asf ../test/poly/07-1m.asf
	../program/morphaca/polymorph/polymorph 3 ../test/poly/01-1m.jpg ../test/poly/06-1m.jpg ../test/poly/07-1m.jpg ../test/poly/01-1m.asf ../test/poly/06-1m.asf ../test/poly/07-1m.asf -t 1
	../program/morphaca/polymorph/polymorph 3 ../test/poly/01-1m.jpg ../test/poly/06-1m.jpg ../test/poly/07-1m.jpg ../test/poly/01-1m.asf ../test/poly/06-1m.asf ../test/poly/07-1m.asf -t 2
	mv *.png ../test/result

auto:
	../program/morphaca/brutelee/brutelee ../test/source/01-1m.jpg ../test/source/02-1m.jpg
	mv *.asf ../test/result
	mv auto.jpg ../test/result
