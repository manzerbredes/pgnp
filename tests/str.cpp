#include "pgnp.hpp"
#include <catch_amalgamated.hpp>

using namespace pgnp;

TEST_CASE("Seven Tag Roster", "[std/pgn1]") {
  PGN pgn;
  REQUIRE_NOTHROW(pgn.FromFile("pgn_files/str/pgn1.pgn"));
  REQUIRE_NOTHROW(pgn.ParseNextGame());

  REQUIRE_NOTHROW(pgn.STRCheck());
  HalfMove *m = new HalfMove();
  pgn.GetMoves(m);
  REQUIRE(m->GetLength() == 85);
  CHECK(pgn.GetResult() == "1/2-1/2");
  REQUIRE_THROWS_AS(pgn.ParseNextGame(),NoGameFound);
}
