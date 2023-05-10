
#include "pgnp.hpp"
#include <iostream>
#include <string>

#define IS_BLANK(c) (c == ' ' || c == '\n' || c == '\t' || c == '\r')
#define IS_ALPHA(c)                                                            \
  ((c == 'a' || c == 'b' || c == 'c' || c == 'd' || c == 'e' || c == 'f' ||    \
    c == 'g' || c == 'h' || c == 'i' || c == 'j' || c == 'k' || c == 'l' ||    \
    c == 'm' || c == 'n' || c == 'o' || c == 'p' || c == 'q' || c == 'r' ||    \
    c == 's' || c == 't' || c == 'u' || c == 'v' || c == 'w' || c == 'x' ||    \
    c == 'y' || c == 'z') ||                                                   \
   (c == 'A' || c == 'B' || c == 'C' || c == 'D' || c == 'E' || c == 'F' ||    \
    c == 'G' || c == 'H' || c == 'I' || c == 'J' || c == 'K' || c == 'L' ||    \
    c == 'M' || c == 'N' || c == 'O' || c == 'P' || c == 'Q' || c == 'R' ||    \
    c == 'S' || c == 'T' || c == 'U' || c == 'V' || c == 'W' || c == 'X' ||    \
    c == 'Y' || c == 'Z'))
#define IS_DIGIT(c)                                                            \
  (c == '0' || c == '1' || c == '2' || c == '3' || c == '4' || c == '5' ||     \
   c == '6' || c == '7' || c == '8' || c == '9')
#define IS_EOF (pgn_content.IsEOF())
#define IS_TOKEN(c)                                                            \
  (IS_DIGIT(c) || IS_ALPHA(c) || c == '{' || c == '}' || c == '(' ||           \
   c == ')' || c == '[' || c == ']' || c == '$' || c == '"' || c == '*')
#define EOF_CHECK(loc)                                                         \
  {                                                                            \
    if (IS_EOF)                                                                \
      throw UnexpectedEOF();                                                   \
  }

namespace pgnp {

PGN::PGN() : moves(NULL), LastGameEndLoc(0) {}

PGN::~PGN() {
  if (moves != NULL)
    delete moves;
}

std::string PGN::GetResult() { return (result); }

void PGN::FromFile(std::string filepath) { pgn_content.FromFile(filepath); }

void PGN::FromString(std::string pgn_content) {
  this->pgn_content.FromString(pgn_content);
}

void PGN::GotoNextGame(){
  // Search for new game
  if (IS_EOF) {
    throw NoGameFound();
  }
  loctype loc = GotoNextToken(LastGameEndLoc);
  if (IS_EOF) {
    throw NoGameFound();
  }
  // First skip current game tags
  while (!IS_EOF) {
    char c = pgn_content[loc];
    if (!IS_BLANK(c)) {
      if (c == '[') {
        loc = ParseNextTag(loc); // Here we are at ']' after the call to ParseNextTag
      } else
        break;
    }
    loc++;
  }
  // Goto next game '[' by skipping the entire game
  while (!IS_EOF) {
    char c = pgn_content[loc];
    if (!IS_BLANK(c)) {
      if (c == '[') {
        LastGameEndLoc=loc;
        return;
      } else if(c== '{'){
        // We skip the comments as they can contains '['
        while(!IS_EOF && c != '}'){
          loc++;
          c = pgn_content[loc];
        }
      }
    }
    loc++;
  }
  throw NoGameFound();
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
  loctype loc = GotoNextToken(LastGameEndLoc);
  if (IS_EOF) {
    throw NoGameFound();
  }

  // Parse game
  while (!IS_EOF) {
    char c = pgn_content[loc];
    if (!IS_BLANK(c)) {
      if (c == '[') {
        loc = ParseNextTag(loc);
      } else {
        LastGameEndLoc = ParseHalfMove(loc, moves);
        break;
      }
    }
    loc++;
  }

  if (result.size() <= 0) {
    throw InvalidGameResult();
  }
}

void PGN::STRCheck() {
  long unsigned int i = 0;
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

loctype PGN::ParseComment(loctype loc, HalfMove *hm) {
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

loctype PGN::ParseHalfMove(loctype loc, HalfMove *hm) {
  // Goto next char
  loc = GotoNextToken(loc);
  EOF_CHECK(loc);
  char c = pgn_content[loc];

  // Check if we reach score entry (* or 1-0 or 0-1 or 1/2-1/2)
  if (c == '*') {
    result = "*";
    return (loc + 1);
  }

  // Parse comment
  if (c == '{') {
    loc = ParseComment(loc, hm);
    EOF_CHECK(loc);
    c = pgn_content[loc];
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

  // Parse Comments, Variations and NAG
  loc = GotoNextToken(loc);
  EOF_CHECK(loc);
  c = pgn_content[loc];
  while (c == '{' || c == '$' || c == '(') {
    if (c == '{') {
      // Parse comment
      loc = ParseComment(loc, hm);
    } else if (c == '$') {
      // Check for NAG
      loc = GotoNextToken(loc);
      EOF_CHECK(loc);
      c = pgn_content[loc];
      if (c == '$') {
        //hm->NAG += c;
        std::string NAG;
        loc++;
        EOF_CHECK(loc);
        c = pgn_content[loc];
        while (IS_DIGIT(c)) {
          NAG += c;
          loc++;
          EOF_CHECK(loc);
          c = pgn_content[loc];
        }
        if(NAG.size()>0)
          hm->NAG=std::stoi(NAG);
      }
    } else if (c == '(') {
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
    }
    loc = GotoNextToken(loc);
    EOF_CHECK(loc);
    c = pgn_content[loc];
  }

  // Skip end of variation
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

loctype PGN::ParseNextTag(loctype start_loc) {
  // Parse key
  std::string key;
  loctype keyloc = start_loc + 1;
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
  loctype valueloc = GotoNextToken(keyloc) + 1;
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

loctype PGN::GotoNextToken(loctype loc) {
  char c = pgn_content[loc];
  while (IS_BLANK(c) || !IS_TOKEN(c)) {
    loc++;
    if (IS_EOF) {
      return (loc);
    }
    c = pgn_content[loc];
    if (c == '%' || c == ';') {
      loc = GotoEOL(loc);
      if (!IS_EOF) {
        c = pgn_content[loc];
      } else {
        return (loc);
      }
    }
  }
  return (loc);
}

loctype PGN::GotoEOL(loctype loc) {
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

void ParseSANMoves(const std::string &sequence,HalfMove *moves) {
  PGN parser;
  // Note that PGN need a results (* at the end)
  // Otherwise an InvalidGameResult exception is raised
  parser.FromString(sequence+" *");
  parser.ParseNextGame();
  parser.GetMoves(moves);
}

} // namespace pgnp