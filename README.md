## PGNP: PGN Parser

PGNP is a Portable Game Notation (PGN) parser. More details about the
PGN specification can be found [here](https://www.chessclub.com/help/PGN-spec).

# How to use it ?
PGNP can be used as a shared library in your project.
You only need to include the header file and linking the .so file to your
executable.

# Example
Load PGN from file:

    pgnp::PGN pgn;
    try {
        pgn.FromFile("pgn.txt");
    }
    catch(...){
        // Handle exceptions
    }
Load PGN from string:

    pgnp::PGN pgn;
    try {
        pgn.FromString("YOUR PGN CONTENT HERE");
    }
    catch(...){
        // Handle exceptions
    }

// TODO: How to use the PGN object with the parsed data