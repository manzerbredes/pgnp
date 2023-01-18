#include "pgnp.hpp"
#include <catch_amalgamated.hpp>

using namespace pgnp;

TEST_CASE("Valid PGN", "[valid/pgn1]") {
  PGN pgn;
  REQUIRE_NOTHROW(pgn.FromFile("pgn_files/valid/pgn1.pgn"));
  REQUIRE_NOTHROW(pgn.ParseNextGame());
  REQUIRE_THROWS_AS(pgn.STRCheck(),pgnp::STRCheckFailed);

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
  REQUIRE_THROWS_AS(pgn.ParseNextGame(), NoGameFound);
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
  REQUIRE_THROWS_AS(pgn.ParseNextGame(), NoGameFound);
}

TEST_CASE("Valid PGN", "[valid/pgn3]") {
  PGN pgn;
  REQUIRE_NOTHROW(pgn.FromFile("pgn_files/valid/pgn3.pgn"));
  REQUIRE_NOTHROW(pgn.ParseNextGame());
  REQUIRE_NOTHROW(pgn.STRCheck());

  REQUIRE(pgn.HasTag("Event"));
  REQUIRE(pgn.HasTag("Site"));
  REQUIRE(pgn.HasTag("Date"));
  REQUIRE(pgn.HasTag("Round"));
  REQUIRE(pgn.HasTag("White"));
  REQUIRE(pgn.HasTag("Black"));
  REQUIRE(pgn.HasTag("Result"));
  REQUIRE(pgn.HasTag("ECO"));
  REQUIRE(pgn.HasTag("Annotator"));
  REQUIRE(pgn.HasTag("PlyCount"));

  REQUIRE(pgn.GetTagValue("Event") == "Hastings");
  REQUIRE(pgn.GetTagValue("Site") == "?");
  REQUIRE(pgn.GetTagValue("Date") == "1999.01.??");
  REQUIRE(pgn.GetTagValue("Round") == "8");
  REQUIRE(pgn.GetTagValue("White") == "Plaskett, James");
  REQUIRE(pgn.GetTagValue("Black") == "Shipov, Sergei");
  REQUIRE(pgn.GetTagValue("Result") == "1-0");
  REQUIRE(pgn.GetTagValue("ECO") == "B23");
  REQUIRE(pgn.GetTagValue("Annotator") == "Hartwig,Th");
  REQUIRE(pgn.GetTagValue("PlyCount") == "53");

  REQUIRE(pgn.GetResult() == "1-0");

  HalfMove *m = new HalfMove();
  pgn.GetMoves(m);

  REQUIRE(m->GetLength() == 53);

  SECTION("Comments") {
    REQUIRE(m->comment ==
            "This week I have to apologise for not providing the sort of "
            "in-depth annotations you may be used to on this site.  My excuse "
            "is that I'm very busy playing chess at the U.S. Open! However, "
            "hopefully I have made up for it by choosing an excellent game, so "
            "it will still be worth your time to take a look. ");
    REQUIRE(m->GetHalfMoveAt(2)->comment ==
            "Certainly an annoying move order against Najdorf junkies.");
    REQUIRE(m->GetHalfMoveAt(19)->comment ==
            "10... Nh6 was probably better.  Far from stopping white's "
            "kingside play, this weakens e6, forming the basis for some nice "
            "knight maneuvers in the next few moves.");
    REQUIRE(m->GetHalfMoveAt(22)->comment ==
            "The knights are homing in on the weak e6 and b6 squares.");
    REQUIRE(
        m->GetHalfMoveAt(52)->comment ==
        "A very nice game.  Black was made to look like a complete beginner.");
    REQUIRE(m->GetHalfMoveAt(44)->variations[0]->comment ==
            "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do "
            "eiusmod "
            "tempor incididunt ut labore et dolore magna aliqua. ");
  }

  SECTION("NAGs") {
    REQUIRE(m->GetHalfMoveAt(2)->NAG == 1);
    REQUIRE(m->GetHalfMoveAt(8)->NAG == 5);
    REQUIRE(m->GetHalfMoveAt(8)->variations[0]->NAG == 14);
    REQUIRE(m->GetHalfMoveAt(19)->NAG == 2);
    REQUIRE(m->GetHalfMoveAt(22)->NAG == 1);
    REQUIRE(m->GetHalfMoveAt(23)->variations[0]->NAG == 4);
    REQUIRE(m->GetHalfMoveAt(23)->variations[0]->GetHalfMoveAt(3)->NAG == 1);
    REQUIRE(m->GetHalfMoveAt(23)->variations[0]->GetHalfMoveAt(9)->NAG ==
            18);
    REQUIRE(m->GetHalfMoveAt(24)->NAG == 1);
    REQUIRE(m->GetHalfMoveAt(24)->variations[0]->GetHalfMoveAt(5)->NAG ==
            13);
    REQUIRE(m->GetHalfMoveAt(30)->NAG == 1);
    REQUIRE(m->GetHalfMoveAt(31)->variations[0]->NAG == 2);
    REQUIRE(m->GetHalfMoveAt(31)->variations[0]->GetHalfMoveAt(3)->NAG ==
            18);
    REQUIRE(m->GetHalfMoveAt(32)->NAG == 1);
    REQUIRE(m->GetHalfMoveAt(37)->variations[0]->GetHalfMoveAt(3)->NAG ==
            18);
    REQUIRE(m->GetHalfMoveAt(38)->NAG == 1);
    REQUIRE(m->GetHalfMoveAt(41)->variations[0]->GetHalfMoveAt(1)->NAG ==
            18);
    REQUIRE(m->GetHalfMoveAt(44)->NAG == 1);
    REQUIRE(m->GetHalfMoveAt(44)->variations[0]->NAG == 4);
    REQUIRE(m->GetHalfMoveAt(44)->variations[0]->MainLine->NAG == 13);
    REQUIRE(m->GetHalfMoveAt(45)->variations[0]->GetHalfMoveAt(3)->NAG ==
            18);
    REQUIRE(m->GetHalfMoveAt(52)->variations[0]->MainLine->variations[0]->MainLine->NAG == 18);
  }
}

TEST_CASE("Goto Next Game Tests", "[valid/pgn3/GotoNextGame]") {
  PGN pgn;
  REQUIRE_NOTHROW(pgn.FromFile("pgn_files/valid/pgn3.pgn"));
  REQUIRE_THROWS_AS(pgn.GotoNextGame(),pgnp::NoGameFound);
}
