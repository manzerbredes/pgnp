#pragma once

#include "Types.hpp"
#include <sstream>
#include <string>
#include <vector>

namespace pgnp {

/**
 * Most members are public for conveniance sake
 */
class HalfMove {
private:
  /// @brief Recursive dump
  std::string NestedDump(HalfMove *, int);

public:
  /// @brief Contains current move count
  int count;
  /// @brief Is this move for black
  bool isBlack;
  /// @brief The SAN move
  std::string move;
  /// @brief Comment associated to the move
  std::string comment;
  std::string NAG;
  /// @brief Next HalfMove link to this line
  HalfMove *MainLine;
  /// @brief Next HalfMove links to variation of this line
  std::vector<HalfMove *> variations;

  HalfMove();
  ~HalfMove();
  /// @brief Get number of HalfMove in the MailLine
  int GetLength();
  /// @brief Dump move and all its variations
  std::string Dump();
  /// @brief Perform a deep copy of a HalfMove
  void Copy(HalfMove *copy);
  /// @brief Get HalfMove located x down the MainLine
  HalfMove *GetHalfMoveAt(int);
};

struct HalfMoveOutOfRange : public std::exception {
  const char *what() const throw() {
    return "HalfMove distance is out of range";
  }
};
} // namespace pgnp