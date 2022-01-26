#define BUFFER_SIZE (1024 * 1024 / 2)

#include <fstream>
#include <iostream>
#include <string>

namespace pgnp {
using namespace std;

class LargeFileStream {
  /// @brief File to load
  ifstream file;
  /// @brief In memory buffer
  char buffer[BUFFER_SIZE];
  /// @brief Number of chuck read minus 1
  long chuck_count;
  /// @brief Number of byte read during the last file access
  long last_read_size;
  /// @brief Keep track of the file offset (to prevent backward read)
  long last_loc;
  /// @brief Use a string as file content
  std::string content;
  /// @brief Use to shortcut some methods
  bool use_string;
  /// @brief End Of File ?
  bool eof;

  /// @brief Load the next chuck of data from disk to memory
  void ReadNextChunk();

public:
  LargeFileStream();
  void FromFile(std::string filepath);
  /// @brief Emulate file access with a string
  void FromString(std::string content);
  /// @brief Allow array like access to the file
  char operator[](long loc);
  /// @brief Check if we reach the EOF
  bool IsEOF(long loc);

  // Various Exceptions
  struct BackwardRead : public std::exception {
    const char *what() const throw() {
      return "LargeFileStream cannot read backward";
    }
  };
  struct ReadToFar : public std::exception {
    const char *what() const throw() { return "You reach the end of the file"; }
  };
};

} // namespace pgnp