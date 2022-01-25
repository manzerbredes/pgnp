#include "HalfMove.hpp"
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
  /// @brief COntains the parsed PGN moves
  HalfMove *moves;
  /// @brief Contains the PGN data
  std::string pgn_content;

public:
  ~PGN();
  void FromFile(std::string);
  void FromString(std::string);
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
  int ParseNextTag(int);
  /// @brief Get the next non-blank char location starting from location in
  /// argument
  int NextNonBlank(int);
  /// @brief Parse a HalfMove at a specific location into @a pgn_content
  int ParseHalfMove(int, HalfMove *);
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

struct UnexpectedCharacter : public std::exception {
  std::string msg;
  UnexpectedCharacter(char actual, char required, int loc) {
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

} // namespace pgnp
