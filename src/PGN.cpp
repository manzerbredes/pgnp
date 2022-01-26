
#include "pgnp.hpp"
#include <iostream>
#include <string>

#define IS_BLANK(c) (c == ' ' || c == '\n' || c == '\t' || c == '\r')
#define IS_DIGIT(c)                                                            \
  (c == '0' || c == '1' || c == '2' || c == '3' || c == '4' || c == '5' ||     \
   c == '6' || c == '7' || c == '8' || c == '9')
#define IS_EOF (pgn_content.IsEOF())
#define EOF_CHECK(loc)                                                         \
  {                                                                            \
    if (IS_EOF)                                                                \
      throw UnexpectedEOF();                                                   \
  }

namespace pgnp {

PGN::PGN() : LastGameEndLoc(0), moves(NULL) {}

PGN::~PGN() {
  if (moves != NULL)
    delete moves;
}

std::string PGN::GetResult() { return (result); }

void PGN::FromFile(std::string filepath) { pgn_content.FromFile(filepath); }

void PGN::FromString(std::string pgn_content) {
  this->pgn_content.FromString(pgn_content);
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

  // Search for new game
  if (IS_EOF) {
    throw NoGameFound();
  }
  ull loc = GotoNextToken(LastGameEndLoc);
  if (IS_EOF) {
    throw NoGameFound();
  }

  // Parse game
  while (!IS_EOF) {
    char c = pgn_content[loc];
    if (!IS_BLANK(c)) {
      if (c == '[') {
        loc = ParseNextTag(loc);
      } else if (IS_DIGIT(c)) {
        LastGameEndLoc = ParseHalfMove(loc, moves);
        break;
      } else if (c == '{') {
        loc = ParseComment(loc, moves);
        continue; // No need loc++
      } else if (c == '%' || c == ';') {
        loc = GotoEOL(loc);
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

ull PGN::ParseComment(ull loc, HalfMove *hm) {
  // Goto next char
  loc = GotoNextToken(loc);
  EOF_CHECK(loc);
  char c = pgn_content[loc];

  // Parse a sequence of comment
  while (!IS_EOF && c == '{') {
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

    // Goto next non blank to look for another comment token
    loc = GotoNextToken(loc);
    if (!IS_EOF) {
      c = pgn_content[loc];
    }
  }
  return (loc);
}

ull PGN::ParseHalfMove(ull loc, HalfMove *hm) {
  // Goto next char
  loc = GotoNextToken(loc);
  EOF_CHECK(loc);
  char c = pgn_content[loc];

  // Check if we reach score entry (* or 1-0 or 0-1 or 1/2-1/2)
  if (c == '*') {
    result = "*";
    return (loc + 1);
  }

  // Parse move number and check if end of game
  if (IS_DIGIT(c)) {
    std::string move_nb;
    char first_digit = c;
    while (IS_DIGIT(c)) {
      move_nb += c;
      loc++;
      c = pgn_content[loc];
      EOF_CHECK(loc);
      if (c == '/' || c == '-') {
        if (c == '/') {
          result = "1/2-1/2";
          return (loc + 6);
        } else if (first_digit == '1') {
          result = "1-0";
          return (loc + 2);
        } else {
          result = "0-1";
          return (loc + 2);
        }
      }
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

  // Parse the HalfMove
  loc = GotoNextToken(loc);
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

  // Check for NAG
  loc = GotoNextToken(loc);
  EOF_CHECK(loc);
  c = pgn_content[loc];
  if (c == '$') {
    hm->NAG += c;
    loc++;
    EOF_CHECK(loc);
    c = pgn_content[loc];
    while (IS_DIGIT(c)) {
      hm->NAG += c;
      loc++;
      EOF_CHECK(loc);
      c = pgn_content[loc];
    }
  }

  // Parse comment
  loc = ParseComment(loc, hm);

  // Check for variations
  loc = GotoNextToken(loc);
  while (!IS_EOF && pgn_content[loc] == '(') {
    loc++; // Skip '('
    HalfMove *var = new HalfMove;
    loc = ParseHalfMove(loc, var);
    hm->variations.push_back(var);
    loc++; // Skip ')'
    // Goto next var
    loc = GotoNextToken(loc);
    EOF_CHECK(loc);
    c = pgn_content[loc];
  }

  // Skip end of variation
  loc = GotoNextToken(loc);
  EOF_CHECK(loc);
  c = pgn_content[loc];
  if (c == ')') {
    return (loc);
  }

  // Parse next HalfMove
  loc = GotoNextToken(loc);
  if (!IS_EOF) {
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

ull PGN::ParseNextTag(ull start_loc) {
  // Parse key
  std::string key;
  ull keyloc = start_loc + 1;
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
  ull valueloc = GotoNextToken(keyloc) + 1;
  EOF_CHECK(keyloc);
  c = pgn_content[valueloc];
  while (c != '"' or IS_EOF) {
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

ull PGN::GotoNextToken(ull loc) {
  char c = pgn_content[loc];
  while (IS_BLANK(c)) {
    loc++;
    if (IS_EOF) {
      return (loc);
    }
    c = pgn_content[loc];
    if (c == '%' || c == ';') {
      loc = GotoEOL(loc);
      if (IS_EOF) {
        return (loc);
      }
    }
  }

  return (loc);
}

ull PGN::GotoEOL(ull loc) {
  char c = pgn_content[loc];
  while (true) {
    loc++;
    if (IS_EOF) {
      return (loc);
    }
    c = pgn_content[loc];
    if (c == '\n') {
      return (loc);
    }
  }
}

} // namespace pgnp