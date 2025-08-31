######################################################################
# Portable Big Integer Library project
# Directions to Users
######################################################################

***************
1. Overview
***************

In this project, you can build a portable big integer calculator and use
the test file to test the correctness.Also, the files are divided into 3 parts:
kernel module, math module and external memory support module.
Each module has its unique features.

***********
2. Files
***********

Makefile                Builds the entire project.
README.txt              This file.
pbk.h                   Kernel Module's related data structures and macros.
pbk.c                   Kernel Function implements.
pbm.h                   Mathematical Module's function declarations.
pbm.c                   Mathematical Function implements.
pbx.h                   External Memory Support Module's data structures and function definitions.
pbx.c                   External Memory Support Module's function implements.

*************************
3. Building the Project
*************************

Step 1: Modify the macro and type mentioned in README.

Step 2: Type "make clean; make" to build the project.

Step 3: Type "make test" to check the calculator with test cases in ../Samples.

************
4. Finish 
************

Try to write your own main function to use this project functions, enjoy!

