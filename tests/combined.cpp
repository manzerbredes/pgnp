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

  SECTION("Check comments of a game 5") {
    pgnp::PGN pgn;
    pgn.FromFile("pgn_files/combined/hartwig.pgn");
    pgn.ParseNextGame();
    pgn.ParseNextGame();
    pgn.ParseNextGame();
    pgn.ParseNextGame();
    pgn.ParseNextGame(); // Load game 5

    HalfMove *m = new HalfMove();
    pgn.GetMoves(m);
    CHECK(m->comment ==
          "I had actually prepared 1.d4 for the tournament, but I backed out "
          "in every (!) game for various different reasons. In this case it "
          "was because things were in such a rut I would only be cheered by "
          "winning in crushing style.  Thankfully it worked!");

    HalfMove *m13b = (m->GetHalfMoveAt(25)); // move 13 black => m13b
    CHECK(m13b->comment ==
          "Definitely the most resiliant. My memory did not serve here so we "
          "were both on our own, but I used the process of elimination to "
          "guide me through the next couple moves, knowing the resulting "
          "position should be winning for white.");
    REQUIRE(m13b->variations.size() == 3);
    CHECK((m13b->variations[1])->comment ==
          "just loses thanks to the interpolation of 11. c3 fxe4:");
    
    HalfMove *m20w = (m->GetHalfMoveAt(38)); // move 20 white => m20w
    CHECK(m20w->NAG == 1);

    HalfMove *m21b = (m->GetHalfMoveAt(42)); // move 21 black => m21b
    CHECK(m21b->comment ==
          "After black resigned a spectator, who had been playing his own game "
          "one board down, shook my hand. I was quite cheered up, but still I "
          "had to wonder, with the advanced state of theory these days, if "
          "this whole game might not have been played before by someone else.");
  }
}

TEST_CASE("Kramnik PGN", "[combined/kramnik]") {
  // PGN source: https://www.angelfire.com/games3/smartbridge/

  pgnp::PGN pgn;
  pgn.FromFile("pgn_files/combined/kramnik.pgn");

  // Count games
  REQUIRE_NOTHROW([&]() {
    char i = 0;
    try {
      while (true) {
        pgn.ParseNextGame();
        i++;
      }
    } catch (const NoGameFound &e) {
      CHECK(i == 40);
    }
  }());

  SECTION("Check comments of a game") {
    pgnp::PGN pgn;
    pgn.FromFile("pgn_files/combined/kramnik.pgn");
    pgn.ParseNextGame(); // Load game 1

    HalfMove *m = new HalfMove();
    pgn.GetMoves(m);
    CHECK(m->comment == "E32: Nimzo-Indian: Classical (4 Qc2): 4...0-0");
  }
}

TEST_CASE("Goto Next Game Tests", "[combined/hartwig/GotoNextGame]") {
  // PGN source: https://www.angelfire.com/games3/smartbridge/

  pgnp::PGN pgn;
  pgn.FromFile("pgn_files/combined/hartwig.pgn");
  // First goto game 3
  pgn.GotoNextGame();
  pgn.GotoNextGame();
  // Parse game 3
  pgn.ParseNextGame();
  CHECK(pgn.GetTagValue("Event") == "Clichy");
  // Goto game 5
  pgn.GotoNextGame();
  // Parse game 5
  pgn.ParseNextGame();
  CHECK(pgn.GetTagValue("Event") == "World Open U2200");
  CHECK(pgn.GetTagValue("Site") == "Philadelphia");
  CHECK(pgn.GetTagValue("Black") == "Thomas, Rodney");
  // Goto game 8
  pgn.GotoNextGame(); // Goto game 7
  pgn.GotoNextGame(); // Goto game 8
  // Parse game 8
  pgn.ParseNextGame();
  CHECK(pgn.GetTagValue("Event") == "Hastings");
  CHECK(pgn.GetTagValue("White") == "Plaskett, James");
  CHECK(pgn.GetTagValue("Black") == "Shipov, Sergei");
}
