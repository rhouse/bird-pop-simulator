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

On Windows, to compile and run parsim, type these at the command prompt:

    c
    parsim exampleinputfile.txt >out.txt
    comp out.txt parsim.txt

The last line above should show that parsim.out and out.txt are identical,
meaning that the parsim program is doing the right thing.

On Linux, type these at the command prompt:

    ./c
    ./parsim exampleinputfile.txt >out.txt

Compare out.txt with parsim.out.  These files should have the same content,
BUT lines in out.txt are terminated by a single LF character, while the file
parsim.out has lines terminated by a CRLF combination.  Various utilities
exist on both Windows and Linux to convert back and forth.
