Prerequisites {#mainpage}
====
* Clang 3.1 or GCC 4.7+ or Visual Studio 10
* CMake (2.8+) - http://cmake.org/cmake/resources/software.html
* Boost (1.55+) - http://www.boost.org/users/download/
* Wt (3.3.3) - http://www.webtoolkit.eu/wt/download

Optional (used for documentation)
----
* Graphviz (2.28+) - http://graphviz.org/Download..php
* Doxygen (1.8+)- http://www.stack.nl/~dimitri/doxygen/download.html#latestsrc

* git clone git@github.com:trink/lpeg_tester.git


lpeg_tester - UNIX Build Instructions
====

    cd lpeg_tester 
    mkdir release
    cd release
    cmake -DCMAKE_BUILD_TYPE=release ..
    make

    # to run the web server locally 
    make install DESTDIR=install
    cd install/usr/local/lpeg_tester; ./run.sh
    # http://localhost:8889/

lpeg_tester - Windows Build Instructions (untested)
====
# in a VS2013 command prompt window

    cd lpeg_tester 
    mkdir release 
    cd release
    cmake -DCMAKE_BUILD_TYPE=release -G "NMake Makefiles" ..
    nmake

    # to run the web server locally 
    nmake install DESTDIR=install
    cd install\lpeg_tester; run.bat
    # http://localhost:8889/

