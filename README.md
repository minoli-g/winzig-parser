# WinZigC Parser

This is a C++ program which parses a WinZigC program and generates its Abstract Syntax Tree.

To build the project :

    make

To run the project on Linux:

    ./winzigc -ast winzig_test_programs/winzig_01

To run the project on Windows:

    winzigc -ast winzig_test_programs/winzig_01

where  `winzig_test_programs/winzig_01` is the path to the WinZigC code file.

The output will be displayed in the console.

To save the output to a file, append "` > tree.01`" to the above command. E.g. for Linux,

    ./winzigc -ast winzig_test_programs/winzig_01 > tree.01

This creates a file called `tree.01` containing the output AST.

To compare the results of the file with the provided one,

On Linux:

    diff tree.01 winzig_test_programs/winzig_01.tree

On Windows:

    fc winzig_test_programs\winzig_01.tree tree.01

