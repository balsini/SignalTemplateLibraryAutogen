STLAutogen: From STL to Simulink blocks autogenerator
=================================

Author: Alessio Balsini


Copyright: ReTiS Laboratory, Scuola Superiore Sant'Anna
email:     a.balsini@sssup.it

======================================================================

SUMMARY

1. INTRODUCTION
2. RELEASE NOTES
3. COMPILING
4. INSTALLING
5. RUNNING THE EXAMPLES
6. ADD A NEW EXAMPLE
7. BUGS AND CONTRIBUTIONS

======================================================================

## 1. INTRODUCTION

TODO

## 2. RELEASE NOTES

This software has been tested under Linux, and Windows. Supported (tested)
compilers are:

	- g++ 
	- clang++ 
	- Microsoft Visual Studio 14 (2015)

We assume the user has the compiler already installed on his machine.

## 3. COMPILING

The software can be compiled on any platform with the appropriate compiler as listed in
section 2.

STLAutogen build system is based on CMake, the cross-platform, open-source build system.
To compile STLAutogen CMake (ver. >= 3.1) is required.

If you altready have CMake installed on your machine you can skip the
following section and jump to section 3.2.

### 3.1. Installing CMake

Follow the instructions at http://www.cmake.org/cmake/help/install.html

### 3.2 Windows Compiling

#### 3.2.1 Installing Flex and Bison

Download the package `win_flex_bison-2.5.6.zip` from the following repository:

https://sourceforge.net/projects/winflexbison/

Extract the content to a desired folder, namely `{win_flex_bison}` and
the `win_flex_bison` folder to 
the Windows environment PATH.

In folder `win_flex_bison`, rename
`win_bison.exe` to `bison.exe` and
`win_flex.exe` to `flex.exe`.

#### 3.2.3 Compiling Project

Run CMake, set the `source code folder` as the main project folder 
and the `build` directory to `source code folder`/`build`.

Click `Configure`, then `Generate`, then `Open Project`.

Build the just opened project (`Ctrl + Shift + B`).

### 3.3 Unix Compiling

The steps to build and install the software are the usual ones:

    cd STLAutogen
    mkdir build
    cd build
    ccmake ..

The execution of "ccmake .." command cause CMake program execution.
To generate the makefile needed to compile the library, you have to press 'c' key twice
and then press 'g'. By pressing 'g' CMake will generate the makefile and will go back 
the the command line.
Now you can compile the library:

    make 

## 5. Generating the Matlab code

Every directory under one of the "examples" directories contains a
different example. Go inside the directory and run the executable
file.

## 6. Using the Matlab code

>> mainPortList('provaSimulink');
>> main('provaSimulink', 'STL_TEST', [60,20,90,60]);

## 7. BUGS AND CONTRIBUTIONS

For bug reports, feature requests and other issues you may encounter,
please refer to the appropriate pages on the github site:

  https://github.com/balsini/SignalTemplateLibraryAutogen

Alessio Balsini
a.balsini@sssup.it