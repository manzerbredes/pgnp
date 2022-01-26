[![pipeline](https://gitlab.com/manzerbredes/pgnp/badges/master/pipeline.svg)](https://gitlab.com/manzerbredes/pgnp/-/commits/master)
[![license](https://img.shields.io/badge/License-LGPL_v3-blue.svg)](https://www.gnu.org/licenses/lgpl-3.0)

## PGNP: PGN Parser

PGNP is a Portable Game Notation (PGN) parser. More details about the
PGN specification can be found [here](https://www.chessclub.com/help/PGN-spec).

# Features
- Basic PGN parsing (tags, move, comments, variations, NAG, etc.)
- Merged PGN files parsing (several games in one file)
- Handle very large file (severals GB)
- Very efficient

# How to use it ?
PGNP can be used as a shared library in your project.
You only need to include `pgnp.hpp` and linking the .so file to your
executable.

# Example
Somewhere at the beginning of the file:

    #include "pgnp.hpp"
Load PGN from file:

    pgnp::PGN pgn;
    try {
        pgn.FromFile("pgn.txt");
        pgn.ParseNextGame();
    }
    catch(...){
        // Handle exceptions
    }
Load PGN from string:

    pgnp::PGN pgn;
    pgn.FromString("YOUR PGN CONTENT HERE");
    try {
        pgn.ParseNextGame();
    }
    catch(...){
        // Handle exceptions
    }
Various API calls:

    bool hasRound=pgn.HasTag("Round"); // Check if tag exists
    try {
        pgn.STRCheck(); // Perform a Seven Tag Roster check
    }
    catch(...){
        // Handle exceptions
    }
    std::vector<std::string> tags=pgn.GetTagList(); // Get a list of tags
    std::string tagValue=GetTagValue("Date"); // Get the value of a tag
Access to moves:

    pgnp::HalfMove *moves=new pgnp::HalfMove();
    pgn.GetMoves(moves); // Get the tree of half moves (do not forget to call "delete move" later on)
    int length=moves->GetLength(); // Get the number of half moves in the move MainLine
    // Public members:
    // moves->variations contains variations of the current move
    // moves->isBlack boolean that says if current half move is for the black side
    // Check pgnp.hpp for more infos for the other fields (comments, count, etc.)

# CMake Integration
By using the `add_subdirectory()` on this repository you will be able to use the following cmake calls in you project:

    include_directories(${PGNP_INCLUDE_DIR})
    target_link_libraries(<YOUR_TARGET> pgnp)


