
#include "pgnp.hpp"
#include <iostream>
#include <string>

#define IS_BLANK(c) (c == ' ' || c == '\n' || c == '\t')
#define IS_DIGIT(c)                                                            \
  (c == '0' || c == '1' || c == '2' || c == '3' || c == '4' || c == '5' ||     \
   c == '6' || c == '7' || c == '8' || c == '9')
#define IS_EOF(loc) ((loc) >= pgn_content.size())
#define EOF_CHECK(loc)                                                         \
  {                                                                            \
    if (IS_EOF(loc))                                                           \
      throw UnexpectedEOF();                                                   \
  }

namespace pgnp {

PGN::PGN() : LastGameEndLoc(0), moves(NULL) {}

PGN::~PGN() {
  if (moves != NULL)
    delete moves;
}

std::string PGN::GetResult() { return (result); }

void PGN::FromFile(std::string filepath) {
  std::ifstream inFile;
  inFile.open(filepath);
  std::stringstream strStream;
  strStream << inFile.rdbuf();

  this->pgn_content = strStream.str();
}

void PGN::FromString(std::string pgn_content) {
  this->pgn_content = pgn_content;
}

void PGN::ParseNextGame() {
  // Clean previous parse
  if (moves != NULL) {
    delete moves;
  }
  result = "";
  tagkeys.clear();
  tags.clear();

  moves = new HalfMove();
  int loc = NextNonBlank(LastGameEndLoc);
  if (IS_EOF(loc)) {
    throw NoGameFound();
  }
  while (!IS_EOF(loc)) {
    char c = pgn_content[loc];
    if (!IS_BLANK(c)) {
      if (c == '[') {
        loc = ParseNextTag(loc);
      } else if (IS_DIGIT(c)) {
        loc = ParseHalfMove(loc, moves);
        LastGameEndLoc = loc + 1; // Next game start 1 char after the last one
        break;
      } else if (c == '{') {
        loc = ParseComment(loc, moves);
        continue; // No need loc++
      }
    }
    loc++;
  }
  if (result.size() <= 0) {
    throw InvalidGameResult();
  }
}

void PGN::STRCheck() {
  int i = 0;
  // Locate Event tag
  while (i < tagkeys.size()) {
    if (tagkeys[i] == "Event") {
      break;
    }
    i++;
  }

  // Check tags
  if (i + 6 < tagkeys.size()) {
    bool valid = (tagkeys[i] == "Event") && (tagkeys[i + 1] == "Site") &&
                 (tagkeys[i + 2] == "Date") && (tagkeys[i + 3] == "Round") &&
                 (tagkeys[i + 4] == "White") && (tagkeys[i + 5] == "Black") &&
                 (tagkeys[i + 6] == "Result");
    if (!valid) {
      throw STRCheckFailed();
    }
  } else {
    throw STRCheckFailed();
  }
}

bool PGN::HasTag(std::string key) {
  auto tags = GetTagList();
  return (std::find(tags.begin(), tags.end(), key) != tags.end());
}

int PGN::ParseComment(int loc, HalfMove *hm) {
  // Goto next char
  loc = NextNonBlank(loc);
  EOF_CHECK(loc);
  char c = pgn_content[loc];

  if (c == '{') {
    loc++;
    EOF_CHECK(loc);
    c = pgn_content[loc];
    while (c != '}') {
      hm->comment += c;
      loc++;
      EOF_CHECK(loc);
      c = pgn_content[loc];
    }
    loc++; // Skip '}'
  }
  return (loc);
}

int PGN::ParseHalfMove(int loc, HalfMove *hm) {
  // Goto next char
  loc = NextNonBlank(loc);
  EOF_CHECK(loc);
  char c = pgn_content[loc];

  // Check if we reach score entry (* or 1-0 or 0-1 or 1/2-1/2)
  if (!IS_EOF(loc + 1)) {
    char nc = pgn_content[loc + 1]; // Next c
    if ((IS_DIGIT(c) && nc == '-') or (IS_DIGIT(c) && nc == '/') or c == '*') {
      if (c == '*') {
        result = "*";
      } else if (nc == '-') {
        if (c == '1') {
          result = "1-0";
          loc += 2;
        } else {
          result = "0-1";
          loc += 2;
        }
      } else {
        result = "1/2-1/2";
        loc += 6;
      }
      return (loc);
    }
  }

  // Parse (move number
  if (IS_DIGIT(c)) {
    std::string move_nb;
    while (IS_DIGIT(c)) {
      move_nb += c;
      loc++;
      c = pgn_content[loc];
      EOF_CHECK(loc);
    }
    hm->count = std::stoi(move_nb);
    loc++;
    EOF_CHECK(loc);
    if (pgn_content[loc] == '.') {
      hm->isBlack = true;
      loc += 2; // Skip two dots
      EOF_CHECK(loc);
    }
  } else {
    hm->isBlack = true;
  }

  // Parse comment entries (various comment could appear during HalfMove
  // parsing)
  loc = ParseComment(loc, hm);

  // Parse the HalfMove
  loc = NextNonBlank(loc);
  EOF_CHECK(loc);
  c = pgn_content[loc];
  std::string move;
  while (!IS_BLANK(c) && c != ')') {
    move += c;
    loc++;
    c = pgn_content[loc];
    EOF_CHECK(loc);
  }
  hm->move = move;

  // Parse comment
  loc = ParseComment(loc, hm);

  // Skip end of variation
  if (c == ')') {
    loc++;
    return (loc);
  }

  // Parse comment
  loc = ParseComment(loc, hm);

  // Check for variations
  loc = NextNonBlank(loc);
  while (!IS_EOF(loc) && pgn_content[loc] == '(') {
    loc++; // Skip '('
    HalfMove *var = new HalfMove;
    loc = ParseHalfMove(loc, var);
    hm->variations.push_back(var);
    loc++; // Skip ')'
  }

  // Parse comment
  loc = ParseComment(loc, hm);

  // Parse next HalfMove
  loc = NextNonBlank(loc);
  if (!IS_EOF(loc)) {
    HalfMove *next_hm = new HalfMove;
    next_hm->count = hm->count;
    loc = ParseHalfMove(loc, next_hm);
    // Check if move parsed successfuly
    if (next_hm->move.size() > 0) {
      hm->MainLine = next_hm;
    } else {
      delete next_hm;
    }
  }

  return (loc);
}

int PGN::ParseNextTag(int start_loc) {
  // Parse key
  std::string key;
  int keyloc = start_loc + 1;
  EOF_CHECK(keyloc);
  char c = pgn_content[keyloc];
  while (!IS_BLANK(c)) {
    key += c;
    keyloc++;
    EOF_CHECK(keyloc);
    c = pgn_content[keyloc];
  }

  // Parse value
  std::string value;
  int valueloc = NextNonBlank(keyloc) + 1;
  EOF_CHECK(keyloc);
  c = pgn_content[valueloc];
  while (c != '"' or IS_EOF(valueloc)) {
    value += c;
    valueloc++;
    EOF_CHECK(keyloc);
    c = pgn_content[valueloc];
  }

  // Add tag
  tags[key] = value;
  tagkeys.push_back(key);

  EOF_CHECK(valueloc + 1);
  c = pgn_content[valueloc + 1];
  if (c != ']') {
    throw UnexpectedCharacter(c, ']', valueloc + 1);
  }

  return (valueloc + 1); // +1 For the last char of the tag which is ']'
}

void PGN::GetMoves(HalfMove *copy) { moves->Copy(copy); }

std::vector<std::string> PGN::GetTagList() { return tagkeys; }

std::string PGN::GetTagValue(std::string key) {
  if (tags.find(key) == tags.end()) {
    throw InvalidTagName();
  }
  return tags[key];
}

std::string PGN::Dump() {
  std::stringstream ss;
  ss << "---------- PGN DUMP ----------" << std::endl;
  ss << "Tags:" << std::endl;
  for (auto &tag : GetTagList()) {
    ss << "  " << tag << "=" << GetTagValue(tag) << std::endl;
  }
  ss << "Moves:" << std::endl;

  if (moves != NULL)
    ss << moves->Dump();
  return (ss.str());
}

int PGN::NextNonBlank(int loc) {
  char c = pgn_content[loc];
  while (IS_BLANK(c)) {
    loc++;
    if (IS_EOF(loc))
      return (loc);
    c = pgn_content[loc];
  }
  return (loc);
}

} // namespace pgnp