
#include "pgnp.hpp"
#include <iostream>
#include <string>

#define IS_BLANK(c) (c == ' ' || c == '\n' || c == '\t')
#define IS_DIGIT(c)                                                            \
  (c == '0' || c == '1' || c == '2' || c == '3' || c == '4' || c == '5' ||     \
   c == '6' || c == '7' || c == '8' || c == '9')
#define IS_EOF(loc) (loc >= pgn_content.size())
#define EOF_CHECK(loc)                                                         \
  {                                                                            \
    if (IS_EOF(loc))                                                           \
      throw UnexpectedEOF();                                                   \
  }

namespace pgnp {

HalfMove::HalfMove() : isBlack(false), MainLine(NULL) {}

HalfMove::~HalfMove() {
  for (auto *move : variations) {
    delete move;
  }
}

void HalfMove::NestedDump(HalfMove *m, int indent) {
  for (int i = 0; i < indent; i++) {
    std::cout << "   ";
  }
  std::cout << "  "
            << " Move=" << m->move << " Count=" << m->count << " Comment=\""
            << m->comment << "\""
            << " IsBlack=" << m->isBlack
            << " Variations=" << m->variations.size() << std::endl;

  for (auto *var : m->variations) {
    NestedDump(var, indent + 1);
  }

  if (m->MainLine != NULL) {
    NestedDump(m->MainLine, indent);
  }
}

void HalfMove::Dump() { NestedDump(this, 0); }

int HalfMove::GetLength() {
  int length = 0;
  HalfMove *m = this;
  while (m != NULL) {
    length++;
    m = m->MainLine;
  }
  return length;
}

PGN::~PGN() {
  if (moves != NULL)
    delete moves;
}

std::string PGN::GetResult() { return (result); }

void PGN::FromFile(std::string filepath) {
  std::ifstream file(filepath);

  std::string content((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
  FromString(content);
}

void PGN::FromString(std::string pgn_content) {
  this->pgn_content = pgn_content;
  moves = NULL;
  int loc = 0;
  while (!IS_EOF(loc)) {
    char c = pgn_content[loc];
    if (!IS_BLANK(c)) {
      if (c == '[') {
        loc = ParseNextTag(loc);
      } else if (IS_DIGIT(c)) {
        moves = new HalfMove();
        loc = ParseLine(loc, moves);
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

int PGN::ParseLine(int loc, HalfMove *hm) {
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
        } else {
          result = "0-1";
        }
      } else {
        result = "1/2-1/2";
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

  // Skip end of variation
  if (c == ')') {
    loc++;
    return (loc);
  }

  // Check for comment
  loc = NextNonBlank(loc);
  if (!IS_EOF(loc) && pgn_content[loc] == '{') {
    loc++; // Skip '{'
    c = pgn_content[loc];
    while (c != '}') {
      hm->comment += c;
      loc++;
      EOF_CHECK(loc);
      c = pgn_content[loc];
    }
    loc++; // Skip '}'
  }

  // Check for variations
  loc = NextNonBlank(loc);
  while (!IS_EOF(loc) && pgn_content[loc] == '(') {
    loc++; // Skip '('
    HalfMove *var = new HalfMove;
    loc = ParseLine(loc, var);
    hm->variations.push_back(var);
    loc++; // Skip ')'
  }

  // Parse next HalfMove
  loc = NextNonBlank(loc);
  if (!IS_EOF(loc)) {
    HalfMove *next_hm = new HalfMove;
    next_hm->count = hm->count;
    loc = ParseLine(loc, next_hm);
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

HalfMove *PGN::GetMoves() { return (moves); }

std::vector<std::string> PGN::GetTagList() { return tagkeys; }

std::string PGN::GetTagValue(std::string key) {
  if (tags.find(key) == tags.end()) {
    throw InvalidTagName();
  }
  return tags[key];
}

void PGN::Dump() {
  std::cout << "---------- PGN DUMP ----------" << std::endl;
  std::cout << "Tags:" << std::endl;
  for (auto &tag : GetTagList()) {
    std::cout << "  " << tag << "=" << GetTagValue(tag) << std::endl;
  }
  std::cout << "Moves:" << std::endl;

  if (moves != NULL)
    moves->Dump();
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