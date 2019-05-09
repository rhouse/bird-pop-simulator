          A bird population simulator written in C++ by Roger House

This bird population simulator, parsim, led to an academic paper; for more 
details click <a href="http://rogerfhouse.com/programming/parsim/index.html">here</a>.

The parsim program consists of the following source files:

	critter.cpp and .h
	distrib.cpp and .h
	random.cpp  and .h
	parsim.cpp

Other files of interest:

	c.bat                   Windows BAT file for compiling with MS C++
	c                       bash script for compiling with g++ on Linux
	parsim.exe              Windows executable
	exampleinputfile.txt    Example parameter file
	parsim.out              Output from exampleinputfile.txt

To compile on Windows type "c" at the command prompt to run c.bat.

To compile on Linux type "./c" at the command prompt to run the c script.

To execute the program on the example file under Windows:

	parsim exampleinputfile.txt >out.txt

This will take a few minutes to execute; be patient.

The files out.txt and parsim.out should compare equal.

To execute the program on the example file under Linux:

	./parsim exampleinputfile.txt >out.txt

The files out.txt and parsim.out are equivalent, but lines in out.txt are 
terminated by a single LF character, while the file parsim.out has lines 
terminated by a CRLF combination.  Various utilities exist on both Windows 
and Linux to convert back and forth.
