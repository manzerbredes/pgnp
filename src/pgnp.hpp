#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <streambuf>
#include <string>
#include <unordered_map>
#include <vector>

namespace pgnp {

class HalfMove {
private:
  /// @brief Recursive dump
  void NestedDump(HalfMove *, int);

public:
  int count;
  bool isBlack;
  std::string move;
  std::string comment;
  HalfMove *MainLine;
  std::vector<HalfMove *> variations;

  HalfMove();
  ~HalfMove();
  /// @brief Get number of HalfMove in the MailLine
  int GetLength();
  /// @brief Dump move and all its variations
  void Dump();
  void Copy(HalfMove* copy);
};

class PGN {
private:
  std::unordered_map<std::string, std::string> tags;
  std::vector<std::string> tagkeys;
  std::string result;

  HalfMove *moves;
  std::string pgn_content;

public:
  ~PGN();
  void FromFile(std::string);
  void FromString(std::string);
  bool HasTag(std::string);
  /// @brief Perform a Seven Tag Roster compliance check
  void STRCheck();
  /// @brief Dump parsed PGN
  void Dump();
  std::vector<std::string> GetTagList();
  std::string GetTagValue(std::string);
  std::string GetResult();
  void GetMoves(HalfMove*);

private:
  /// @brief Populate @a tags with by parsing the one starting at location in
  /// argument
  int ParseNextTag(int);

  /// @brief Get the next non-blank char location starting from location in
  /// argument
  int NextNonBlank(int);

  int ParseLine(int, HalfMove *);
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
