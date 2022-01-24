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
Various API calls:

    pgn.HasTag("Round"); // Check if tag exists
    try {
        pgn.STRCheck(); // Perform a Seven Tag Roster check
    }
    catch(...){
        // Handle exceptions
    }
    std::vector<std::string> tags=pgn.GetTagList(); // Get a list of tags
    std::string tagValue=GetTagValue("Date"); // Get the value of a tag
Access to moves:

    pgnp::HalfMove *move=new pgnp::HalfMove();
    pgn.GetMoves(move); // Get the tree of half moves (do not forget to call "delete move")
    int length=move->GetLength(); // Get the number of half moves in the move MainLine
    // Public members:
    // move->variations contains variations of the current move
    // move->isBlack boolean that says if current half move is for the black side
    // Check pgnp.hpp for more infos for other field (comments, count, etc.)

