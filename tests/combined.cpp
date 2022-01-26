#include "pgnp.hpp"
#include <catch_amalgamated.hpp>

using namespace pgnp;

TEST_CASE("Hartwig PGN", "[combined/hartwig]") {
  // PGN source: https://www.angelfire.com/games3/smartbridge/

  pgnp::PGN pgn;
  pgn.FromFile("pgn_files/combined/hartwig.pgn");

  // Count games
  REQUIRE_NOTHROW([&]() {
    char i = 0;
    try {
      while (true) {
        pgn.ParseNextGame();
        i++;
      }
    } catch (const NoGameFound &e) {
      CHECK(i == 29);
    }
  }());

  SECTION("Check comments of a game") {
    pgnp::PGN pgn;
    pgn.FromFile("pgn_files/combined/hartwig.pgn");
    pgn.ParseNextGame();
    pgn.ParseNextGame();
    pgn.ParseNextGame();
    pgn.ParseNextGame();
    pgn.ParseNextGame(); // Load game 5

    HalfMove *m = new HalfMove();
    pgn.GetMoves(m);
    std::cout << m->comment;
    CHECK(m->comment ==
          "I had actually prepared 1.d4 for the tournament, but I backed out "
          "in every (!) game for various different reasons. In this case it "
          "was because things were in such a rut I would only be cheered by "
          "winning in crushing style.  Thankfully it worked!");
  }
}