[![pipeline](https://gitlab.com/manzerbredes/pgnp/badges/master/pipeline.svg)](https://gitlab.com/manzerbredes/pgnp/-/commits/master)
[![license](https://img.shields.io/badge/License-LGPL_v3-blue.svg)](https://www.gnu.org/licenses/lgpl-3.0)

## PGNP: PGN Parser

PGNP is a Portable Game Notation (PGN) parser. More details about the
PGN specification can be found [here](https://www.chessclub.com/help/PGN-spec).

# Features
- Basic PGN parsing (tags, move, comments, variations, NAG, etc.)
- Parse PGN files that contains multiple games
- Handle very large files: up to 2^(sizeof(unsigned long long)*8) bytes
- Efficiency:

<b></b>

    # cpu: Intel(R) Core(TM) i7-10750H CPU @ 2.60GHz
    # file name: lichess_db_standard_rated_2017-06.pgn
    # file sha1sum: af8e01ab9709950be1804880d22d094324df5338
    # file source: https://database.lichess.org/#standard_games
    # file number of games: 11512600
    # file size: 24.6GB
    # pgnp FILE_BUFFER_SIZE: (1024 * 1024 / 2)

	> User time (seconds): 599.81
	> System time (seconds): 2.72
	> Percent of CPU this job got: 99%
	> Elapsed (wall clock) time (h:mm:ss or m:ss): 10:03.17
	> Average shared text size (kbytes): 0
	> Average unshared data size (kbytes): 0
	> Average stack size (kbytes): 0
	> Average total size (kbytes): 0
	> Maximum resident set size (kbytes): 4484
	> Average resident set size (kbytes): 0
	> Major (requiring I/O) page faults: 0
	> Minor (reclaiming a frame) page faults: 784
	> Voluntary context switches: 4
	> Involuntary context switches: 6649
	> Swaps: 0
	> File system inputs: 0
	> File system outputs: 0
	> Socket messages sent: 0
	> Socket messages received: 0
	> Signals delivered: 0
	> Page size (bytes): 4096
	> Exit status: 0


# How to use it ?
PGNP can be used as a shared library in your project.
You only need to include `pgnp.hpp` and linking the .so file to your
executable.

# Example
Somewhere at the beginning of the file:

    #include "pgnp.hpp"
Load PGN from file/string:

    pgnp::PGN pgn;
    try {
        pgn.FromFile("pgn.txt"); // Or pgn.FromString("YOUR PGN CONTENT HERE");
        pgn.ParseNextGame();
    }
    catch(...){
        // Handle exceptions
    }
Various API calls:

    bool hasRound=pgn.HasTag("Round"); // Check if a tag exists
    try {
        pgn.STRCheck(); // Perform a Seven Tag Roster check
    }
    catch(...){
        // Handle exceptions
    }
    std::vector<std::string> tags=pgn.GetTagList(); // Get the list of tags in current game
    std::string tagValue=GetTagValue("Date"); // Get the value of a tag in current game
Access to moves:

    pgnp::HalfMove *moves=new pgnp::HalfMove();
    pgn.GetMoves(moves); // Get the tree of half moves
    int length=moves->GetLength(); // Get the number of half move in the current MainLine
    // Public members:
    // moves->variations contains variations of the current move
    // moves->isBlack boolean that says if current half move is for the black side
    // Check pgnp.hpp for more infos for the other fields (comments, count, etc.)

# CMake Integration
By using the `add_subdirectory()` directive on this repository, you will be able to use the following cmake calls in your project:

    target_link_libraries(<YOUR_TARGET> pgnp)


