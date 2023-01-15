#pragma once

#include "HalfMove.hpp"
#include "LargeFileStream.hpp"
#include "Types.hpp"
#include <algorithm>
#include <exception>
#include <fstream>
#include <unordered_map>

namespace pgnp {

class PGN {
private:
  /// @brief Contains tags data
  std::unordered_map<std::string, std::string> tags;
  /// @brief Contains the tags list in parsed order
  std::vector<std::string> tagkeys;
  /// @brief Contains game result (last PGN word)
  std::string result;
  /// @brief Contains the parsed PGN moves
  HalfMove *moves;
  /// @brief Contains the PGN data
  LargeFileStream pgn_content;
  /// @brief Contains the location of the end of the last parsed game (1 PGN
  /// file may have multiple games)
  loctype LastGameEndLoc;

public:
  PGN();
  ~PGN();
  void FromFile(std::string);
  void FromString(std::string);
  /**
   * Parse the next available game. Note that it raises a @a NoGameFound
   * exception if no more game is available. A call to this method flush all the
   * last parsed game data. Be careful.
   */
  void ParseNextGame();
  /// @brief Goto the next game while avoiding to parse entire game moves
  void GotoNextGame();
  /// @brief Check if PGN contains a specific tag
  bool HasTag(std::string);
  /// @brief Perform a Seven Tag Roster compliance check
  void STRCheck();
  /// @brief Dump parsed PGN into a string
  std::string Dump();
  /// @brief Retrieve parsed tag list
  std::vector<std::string> GetTagList();
  /// @brief Access to the value of a tag
  std::string GetTagValue(std::string);
  /// @brief Get game result based on the last PGN word
  std::string GetResult();
  /// @brief Fetch PGN moves as HalfMove structure
  void GetMoves(HalfMove *);

private:
  /// @brief Populate @a tags with by parsing the one starting at location in
  /// argument
  loctype ParseNextTag(loctype);
  /// @brief Parse a HalfMove at a specific location into @a pgn_content
  loctype ParseHalfMove(loctype, HalfMove *);
  /// @brief Parse a consecutive sequence of comment
  loctype ParseComment(loctype, HalfMove *);
  /// @brief Get the next non-blank char location ignoring line comments ('%'
  /// and ';')
  loctype GotoNextToken(loctype);
  /// @brief Goto the end of the current line
  loctype GotoEOL(loctype);
};

struct UnexpectedEOF : public std::exception {
  const char *what() const throw() { return "Unexpected end of pgn file"; }
};

struct InvalidTagName : public std::exception {
  const char *what() const throw() { return "Invalid tag name"; }
};

struct InvalidGameResult : public std::exception {
  const char *what() const throw() { return "Invalid game result"; }
};

struct NoGameFound : public std::exception {
  const char *what() const throw() { return "No game (or more game) found"; }
};

struct UnexpectedCharacter : public std::exception {
  std::string msg;
  UnexpectedCharacter(char actual, char required, loctype loc) {
    std::stringstream ss;
    ss << "Expected \'" << required << "\' at location " << loc
       << " but read \'" << actual << "\'";
    msg = ss.str();
  }
  const char *what() const throw() { return msg.c_str(); }
};

struct STRCheckFailed : public std::exception {
  const char *what() const throw() {
    return "Seven Tag Roster compliance check failed";
  }
};

/**
 * @brief Parse a simple sequence of SAN moves
 *
 * @param sequence e.g: 1.e4 e5 2.Nf3
 * @param moves Returned moves sequence
 */
void ParseSANMoves(const std::string &sequence,HalfMove *moves);

} // namespace pgnp
