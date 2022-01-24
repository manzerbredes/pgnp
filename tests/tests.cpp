#include "pgnp.hpp"
#include <catch_amalgamated.hpp>

using namespace pgnp;

TEST_CASE("Valid PGN", "[valid/pgn1]") {
  PGN pgn;
  REQUIRE_NOTHROW(pgn.FromFile("pgn_files/valid/pgn1.pgn"));
  REQUIRE_THROWS(pgn.STRCheck());

  HalfMove *m = pgn.GetMoves();
  REQUIRE(m->GetLength() == 6);

  SECTION("Main line move checks") {
    CHECK(m->move == "g3");

    m = m->MainLine;
    CHECK(m->move == "d5");

    m = m->MainLine;
    CHECK(m->move == "Bg2");

    m = m->MainLine;
    CHECK(m->move == "Nf6");

    m = m->MainLine;
    CHECK(m->move == "c4");

    m = m->MainLine;
    CHECK(m->move == "c6");
  }

  SECTION("Main line color checks") {
    m = pgn.GetMoves();
    CHECK_FALSE(m->isBlack);

    m = m->MainLine;
    CHECK(m->isBlack);

    m = m->MainLine;
    CHECK_FALSE(m->isBlack);

    m = m->MainLine;
    CHECK(m->isBlack);
  }

  SECTION("Tag exists checks") {
    CHECK(pgn.HasTag("WhiteElo"));
    CHECK_FALSE(pgn.HasTag("Round"));
    CHECK(pgn.HasTag("TimeControl"));
  }

  SECTION("Tag values checks") {
    CHECK(pgn.GetTagValue("WhiteElo") == "1830");
    CHECK(pgn.GetTagValue("TimeControl") == "600+5");
    CHECK_THROWS_AS(pgn.GetTagValue("InvalidTagName"), InvalidTagName);
  }

  CHECK(pgn.GetResult() == "*");
}

TEST_CASE("Valid PGN", "[valid/pgn2]") {
  PGN pgn;
  REQUIRE_NOTHROW(pgn.FromFile("pgn_files/valid/pgn2.pgn"));
  REQUIRE_THROWS(pgn.STRCheck());
  REQUIRE(pgn.GetMoves()->GetLength() == 66);
  CHECK(pgn.GetResult() == "0-1");
}

TEST_CASE("Seven Tag Roster", "[std/pgn1]") {
  PGN pgn;
  REQUIRE_NOTHROW(pgn.FromFile("pgn_files/str/pgn1.pgn"));
  REQUIRE_NOTHROW(pgn.STRCheck());
  REQUIRE(pgn.GetMoves()->GetLength() == 85);
  CHECK(pgn.GetResult() == "1/2-1/2");
}
