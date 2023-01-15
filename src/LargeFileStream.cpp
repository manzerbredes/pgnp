#include "LargeFileStream.hpp"

namespace pgnp {
using namespace std;

LargeFileStream::LargeFileStream()
    : chuck_count(-1), last_read_size(0), last_loc(0), use_string(false),
      eof(false) {}

void LargeFileStream::FromFile(std::string filepath) {
  file.open(filepath);
  ReadNextChunk();
}

void LargeFileStream::FromString(std::string content) {
  use_string = true;
  this->content = content;
}

void LargeFileStream::ReadNextChunk() {
  chuck_count++;
  file.read(buffer, FILE_BUFFER_SIZE);
  last_read_size = file.gcount();
}

char LargeFileStream::operator[](loctype loc) {
  // Perform various checks
  if (eof) {
    throw ReadToFar();
  }
  if (loc < last_loc) {
    throw BackwardRead();
  }
  last_loc = loc; // Keep track

  // Shortcut the operator for string content
  if (use_string) {
    if (loc >= content.size()) {
      eof = true;
      return ('?');
    }
    return (content[loc]);
  }

  // Goto the right memory chuck
  loctype loc_chunk_count = loc / FILE_BUFFER_SIZE;
  while (chuck_count < loc_chunk_count) {
    ReadNextChunk();
  }
  loctype offset = loc - (loc_chunk_count * FILE_BUFFER_SIZE);
  
  // Ensure for EOF
  if (!file && offset >= last_read_size) {
    eof = true;
    return ('?');
  }

  // Return character
  return buffer[offset];
}

bool LargeFileStream::IsEOF() { return (eof); }

} // namespace pgnp