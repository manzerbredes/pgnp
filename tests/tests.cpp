#include "pgnp.hpp"
#include <catch_amalgamated.hpp>

using namespace pgnp;

TEST_CASE("Valid PGN", "[pgn1]") {
  PGN pgn;
  REQUIRE_NOTHROW(pgn.FromFile("pgn_files/valid/pgn1.pgn"));
  REQUIRE_THROWS(pgn.STRCheck());
  REQUIRE(pgn.GetMoves()->GetLength() == 6);
}

TEST_CASE("Valid PGN", "[pgn2]") {
  PGN pgn;
  REQUIRE_NOTHROW(pgn.FromFile("pgn_files/valid/pgn2.pgn"));
  REQUIRE_THROWS(pgn.STRCheck());
  REQUIRE(pgn.GetMoves()->GetLength() == 66);
}

TEST_CASE("Seven Tag Roster", "[pgn1]") {
  PGN pgn;
  REQUIRE_NOTHROW(pgn.FromFile("pgn_files/str/pgn1.pgn"));
  REQUIRE_NOTHROW(pgn.STRCheck());
  REQUIRE(pgn.GetMoves()->GetLength() == 85);
}
