SignalTemplateLibraryAutogen: From STL to Simulink blocks autogenerator
=================================

Author: Alessio Balsini


Copyright: ReTiS Laboratory, Scuola Superiore Sant'Anna
email:     a.balsini@sssup.it

======================================================================

SUMMARY

1. Introduction
2. System Requirements
3. Compiling
  3.1. Installing CMake
  3.2 Windows Compiling
    3.2.1 Installing Flex and Bison
    3.2.2 Compiling Project
  3.3 Unix Compiling
4. Using the tool
5. Bugs & Contributions

======================================================================

## 1. Introduction


### 1.1 Language structure and syntax

#### STL formulas structure

| Id | Is |
| ---           | ---    |
| STLFormula    | BoolExpr \| !STLFormula \| STLFormula AND STLFormula \| STLUntil \| STLAlways \| STLEventually |
| STLAlways     | []_TimeExpr STLFormula \| [] STLFormula |
| STLEventually | <>_TimeExpr STLFormula \| <> STLFormula      |
| STLUntil      | STLFormula U_TimeExpr STLFormula \| STLFormula U STLFormula      |

#### Expressions

| Id | Is |
| ---        | ---    |
| TimeExpr   | [Expr, Expr] |
| Expr       | Val \| Expr ExprOp Val |
| BoolExpr   | Expr CmpOp Expr \| BoolExpr BoolOp BoolExpr \| BoolFunction \| BoolVal | 

#### Operators

| Id | Is |
| ---        | ---    |
| ExprOp     | \+ \| - \| * \| / |
| CmpOp      | \> \| < \| >= \| <= \| == \| != |
| BoolOp     | && \| \|\| |

#### Values


| Id | Is |
| ---        | ---    |
| BoolVal    | TRUE \| FALSE |
| Val        | ConstantValue \| ModelSignal |

#### Functions

| Id | Is |
| ---          | ---    |
| BoolFunction | isStep() ...|

## 2. System Requirements

This software has been tested under Linux, and Windows. Supported (tested)
compilers are:

	- g++ 
	- clang++ 
	- Microsoft Visual Studio 14 (2015)

We assume the user has the compiler already installed on his machine.

## 3. Compiling

The software can be compiled on any platform with the appropriate compiler as 
listed in section 2.

SignalTemplateLibraryAutogen build system is based on CMake, the 
cross-platform, open-source build system.
To compile SignalTemplateLibraryAutogen, CMake (ver. >= 3.1) is required.

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

#### 3.2.2 Compiling Project

Run CMake, set the `source code folder` as the main project folder 
and the `build` directory to `source code folder`/`build`.

Click `Configure`, then `Generate`, then `Open Project`.

Build the just opened project (`Ctrl + Shift + B`).

### 3.3 Unix Compiling

The steps to build the software are the usual ones:

    cd SignalTemplateLibraryAutogen
    mkdir build
    cd build
    ccmake ..
    make 

## 4. Using the tool

The following sections describe how to add a probe to the example simulink 
model `\Matlab\SimulinkModelExample.slx`.

### 4.1 Generating the ports

Open Matlab and add the `SignalTemplateLibraryAutogen\Matlab\` folder
to the Matlab paths.

Navigate to the `SignalTemplateLibraryAutogen\Matlab\` and, in the Matlab
console, run the function

>  mainPortList('SimulinkModelExample');

This will generate the file `AUTOGEN_portList.txt`.

### 4.2 Generating the Matlab code

Run the SignalTemplateLibraryAutogen passing as first parameter the desired 
STL file and as second parameter the path containing the 
`AUTOGEN_portList.txt` file.

Remember to close the `AUTOGEN_portList.txt` path with the proper `"/"` (or
`"\"` on Windows) marker.

>  .\SignalTemplateLibraryAutogen.exe \
>  C:\{something}\SignalTemplateLibraryAutogen\examples\example_01.stl \
C:\{something}\SignalTemplateLibraryAutogen\Matlab\

This generates the `AUTOGEN_testBlock.m` file, containing the autogenerated 
Matlab code to generate the Simulink test block.
In the Matlab console, run

> main('SimulinkModelExample', 'STL_TEST', [100,400,130,430]);

This will add the test block to the Simulink model and create all the 
connections.

## 5. Bugs & Contributions

For bug reports, feature requests and other issues you may encounter,
please refer to the appropriate pages on the Github site:

  https://github.com/balsini/SignalTemplateLibraryAutogen

Alessio Balsini
a.balsini@sssup.it