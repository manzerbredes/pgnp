#include "pgnp.hpp"
#include <catch_amalgamated.hpp>

using namespace pgnp;

TEST_CASE("Valid PGN", "[valid/pgn1]") {
  PGN pgn;
  REQUIRE_NOTHROW(pgn.FromFile("pgn_files/valid/pgn1.pgn"));
  REQUIRE_NOTHROW(pgn.ParseNextGame());
  REQUIRE_THROWS(pgn.STRCheck());

  HalfMove *m = new HalfMove();
  pgn.GetMoves(m);
  HalfMove *m_backup = m;
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
    m = m_backup;
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

  CHECK(m_backup->GetHalfMoveAt(4)->move == "c4");
  CHECK(pgn.GetResult() == "*");
  REQUIRE_THROWS_AS(pgn.ParseNextGame(),NoGameFound);
}

TEST_CASE("Valid PGN", "[valid/pgn2]") {
  PGN pgn;
  REQUIRE_NOTHROW(pgn.FromFile("pgn_files/valid/pgn2.pgn"));
  REQUIRE_NOTHROW(pgn.ParseNextGame());

  REQUIRE_THROWS(pgn.STRCheck());
  HalfMove *m = new HalfMove();
  pgn.GetMoves(m);
  REQUIRE(m->GetLength() == 66);
  CHECK(pgn.GetResult() == "0-1");
  CHECK(m->comment == " A00 Hungarian Opening ");
  CHECK(m->GetHalfMoveAt(65)->comment == " White resigns. ");
  CHECK(m->GetHalfMoveAt(7)->comment ==
        " (0.22 → 0.74) Inaccuracy. dxc4 was best. ");

  SECTION("Check Variations") {
    HalfMove *var = m->GetHalfMoveAt(7)->variations[0];
    REQUIRE(var->GetLength() == 10);
    CHECK(var->move == "dxc4");
    CHECK(var->GetHalfMoveAt(1)->move == "O-O");
  }
  REQUIRE_THROWS_AS(pgn.ParseNextGame(),NoGameFound);
}
