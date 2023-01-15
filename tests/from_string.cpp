#include "pgnp.hpp"
#include <catch_amalgamated.hpp>

using namespace pgnp;

TEST_CASE("FromString", "[string/FromString]") {
  std::string content=
"[Event \"Rated Rapid game\"]\n"
"[Site \"https://lichess.org/cMjillK3\"]\n"
"1. g3 { A00 Hungarian Opening } d5 2. Bg2 Nf6 3. c4 c6 4. Nf3 h6?! { (0.22 → 0.74) Inaccuracy. dxc4 was best. } (4... dxc4 5. O-O Nbd7 6. Qc2 Nb6 7. a4 a5 8. Na3 Be6 9. Ne5) 5. e3?! { (0.74 → -0.15) Inaccuracy. O-O was best. } (5. O-O Bf5 6. d3 e6 7. cxd5 cxd5 8. Qb3 Qb6 9. Nd4 Bg6) 5... Bf5 6. Nc3?! { (-0.15 → -0.75) Inaccuracy. d4 was best. } (6. d4 Nbd7 7. b3 e6 8. O-O Be7 9. Nc3 O-O 10. Bb2 a5 11. Qe2 Ne4 12. Nxe4 Bxe4) 6... e6 7. d4 Be7 8. Qe2 O-O 9. a3 a5 10. Bd2?! { (-0.33 → -0.84) Inaccuracy. b3 was best. } (10. b3 Re8) 10... Bc2 11. O-O Bb3 12. c5?? { (-0.22 → -3.09) Blunder. cxd5 was best. } (12. cxd5) 12... Bc4 13. Qd1 Bxf1 14. Bxf1 b6 15. cxb6 Qxb6 16. Na4 Qa7 17. Rc1 Rc8 18. Ne5 c5 19. dxc5 Bxc5 20. Bb5? { (-2.12 → -4.00) Mistake. Rc2 was best. } (20. Rc2 Nbd7) 20... Ne4?? { (-4.00 → -1.02) Blunder. Qb7 was best. } (20... Qb7 21. Be8) 21. Kg2?! { (-1.02 → -2.00) Inaccuracy. b4 was best. } (21. b4) 21... Qb7 22. f3?? { (-2.13 → -7.56) Blunder. Bd3 was best. } (22. Bd3 Ba7) 22... Nxd2 23. Bc6 Nxc6 24. Nxc5 Qxb2 25. Ned3 Qxa3 26. Qxd2 Nb4 27. Nb2 Rc7 28. Kh3 Rac8 29. Nba4 Na6 30. Rc3 Qa1 31. Rc1 Qf6 32. Qxa5 Nxc5 33. Nb6 Nb3 { White resigns. } 0-1";


  PGN pgn;
  REQUIRE_NOTHROW(pgn.FromString(content));
  REQUIRE_NOTHROW(pgn.ParseNextGame());
  CHECK(pgn.GetTagValue("Event") == "Rated Rapid game");
  CHECK(pgn.GetTagValue("Site") == "https://lichess.org/cMjillK3");
  REQUIRE_THROWS(pgn.STRCheck());

  // Code from valid.cpp:
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


TEST_CASE("ParseSANMoves", "[string/ParseSANMoves]") {
  HalfMove *m = new HalfMove();
  REQUIRE_NOTHROW(ParseSANMoves("1. e4  e5 2.Nf3",m));
  REQUIRE(m->GetLength() == 3);
  CHECK(m->move == "e4");
  CHECK(m->MainLine->move == "e5");
  CHECK(m->MainLine->MainLine->move == "Nf3");
}

