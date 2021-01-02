#define CATCH_CONFIG_ENABLE_CHRONO_STRINGMAKER

#include "nuclear-data-reader/nubase_data.hpp"

#include <catch2/catch.hpp>


// TEST_CASE("", "[Isotope]")
//{
//}


TEST_CASE("Set Symbol", "[NUBASEData]")
{
  NUBASE::Data gs03_isotope("");

  gs03_isotope.setSymbol("Pb");
  REQUIRE_THAT(gs03_isotope.symbol, Catch::Matches("Pb"));
}


TEST_CASE("Set State", "[NUBASEData]")
{
  SECTION("Ground state")
  {
    const std::string gs03{
      "010 0030   10Li    33051       15                              2.0   zs 0.5    (1-,2-)       99 94Yo01tj  n=100"
    };
    NUBASE::Data gs03_isotope(gs03);

    gs03_isotope.setState();
    REQUIRE(gs03_isotope.level == 0);
  }

  SECTION("First Isomeric state")
  {
    const std::string isomer03{
      "010 0031   10Lim   33250       40       200      40     RQ     3.7   zs 1.5    1+               97Zi04t   IT=100"
    };
    NUBASE::Data isomer03_isotope(isomer03);

    auto anotherIsomer = std::move(isomer03_isotope);

    anotherIsomer.setState();
    REQUIRE(anotherIsomer.level == 1);
  }

  SECTION("An isomeric state is added to the correct ground state")
  {
    std::vector<NUBASE::Data> table;

    const std::string gs03{
      "010 0030   10Li    33051       15                              2.0   zs 0.5    (1-,2-)       99 94Yo01tj  n=100"
    };
    NUBASE::Data gs03_isotope(gs03);

    gs03_isotope.setA();
    gs03_isotope.setZ();
    gs03_isotope.setState();

    table.emplace_back(gs03_isotope);

    const std::string isomer03{
      "010 0031   10Lim   33250       40       200      40     RQ     3.7   zs 1.5    1+               97Zi04t   IT=100"
    };
    NUBASE::Data isomer03_isotope(isomer03);

    isomer03_isotope.setA();
    isomer03_isotope.setZ();
    isomer03_isotope.setState();

    isomer03_isotope.setIsomerData(table);
    REQUIRE(table.front().energy_levels.size() == 1);
    REQUIRE(table.front().energy_levels.front().energy == Approx(200));
    REQUIRE(table.front().energy_levels.front().error == Approx(40));
    REQUIRE(table.front().energy_levels.front().level == 1);
  }
}


TEST_CASE("State object is created correctly", "[NUBASEData]")
{
  auto level = NUBASE::Data::State(0, 1.2345, 0.321);

  REQUIRE(level.level == 0);
  REQUIRE(level.energy == Approx(1.2345));
  REQUIRE(level.error == Approx(0.321));
}


TEST_CASE("Set IsomerEnergy", "[NUBASEData]")
{
  NUBASE::Data isomer03_isotope("084 0351   84Brm  -77460      100       340     100     BD     6.0    m 0.2    (6-)   "
                                "       97           B-=100");

  double value{ 0.0 };
  isomer03_isotope.setIsomerEnergy(value);

  REQUIRE(value == Approx(340.0));
}


TEST_CASE("Set IsomerEnergyError", "[NUBASEData]")
{
  NUBASE::Data isomer03_isotope("206 0811   206Tlm -19610.0      1.4    2643.11    0.19         3.74   m 0.03   (12-)  "
                                "       99           IT=100");

  double value{ 0.0 };
  isomer03_isotope.setIsomerEnergyError(value);

  REQUIRE(value == Approx(0.19));
}


TEST_CASE("Read and set measured or theoretical", "[NUBASEData]")
{
  NUBASE::Data gs03_isotope("115 0470   115Ag  -84990       30                             20.0    m 0.5    1/2-       "
                            "   99           B-=100");

  NUBASE::Data theoretical03_isotope(
      "014 0090   14F     32660#     400#                                             2-#                        p ?");

  gs03_isotope.setExperimental();
  REQUIRE(gs03_isotope.exp == 1);
  gs03_isotope.setExperimental(0);
  REQUIRE(gs03_isotope.exp == 0);

  theoretical03_isotope.setExperimental();
  REQUIRE(theoretical03_isotope.exp == 0);
  theoretical03_isotope.setExperimental(1);
  REQUIRE(theoretical03_isotope.exp == 1);
}


TEST_CASE("Read spin parity of the state", "[NUBASEData]")
{
  NUBASE::Data gs03_isotope("028 0110   28Na     -989       13                             30.5   ms 0.4    1+         "
                            "   01           B-=100;B-n=0.58 12");

  NUBASE::Data isomer03_isotope("077 0321   77Gem  -71054.3      1.7     159.70    0.10        52.9    s 0.6    1/2-   "
                                "       97           B-=81 2;IT=19 2");

  gs03_isotope.setSpinParity();

  REQUIRE(gs03_isotope.J == Approx(1.0));
  REQUIRE(gs03_isotope.pi == 0);
  REQUIRE(gs03_isotope.J_tent == 0);

  isomer03_isotope.setSpinParity();

  REQUIRE(isomer03_isotope.J == Approx(0.5));
  REQUIRE(isomer03_isotope.pi == 1);
  REQUIRE(isomer03_isotope.J_tent == 0);
}


TEST_CASE("Read half-life value", "[NUBASEData]")
{
  NUBASE::Data gs03_isotope("100 0410   100Nb  -79939       26                              1.5    s 0.2    1+         "
                            "   97           B-=100");

  gs03_isotope.setHalfLifeValue();
  auto halflife = Converter::seconds{ 1.5 };

  REQUIRE(gs03_isotope.hl == halflife);
}


TEST_CASE("Read half-life unit", "[NUBASEData]")
{
  NUBASE::Data gs03_isotope("205 0820   205Pb  -23770.1      1.2                           15.3   My 0.7    5/2-       "
                            "   93           EC=100");

  gs03_isotope.setHalfLifeUnit();

  REQUIRE_THAT(gs03_isotope.halflife_unit, Catch::Matches("My"));
}


TEST_CASE("Read half-life error", "[NUBASEData]")
{
  NUBASE::Data gs03_isotope("140 0560   140Ba  -83271        8                             12.752  d 0.003  0+         "
                            "   98           B-=100");

  gs03_isotope.setHalfLifeErrorValue();
  auto hl_error = Converter::seconds{ 0.003 };

  REQUIRE(gs03_isotope.hl_error == hl_error);
}


TEST_CASE("Read and set half-life to double", "[NUBASEData]")
{
  NUBASE::Data gs03_isotope("056 0230   56V    -46080      200                            216     ms 4      (1+)       "
                            "   99 03Ma02tj  B-=100;B-n ?");

  gs03_isotope.setHalfLife();
  auto halflife = Converter::milliseconds{ 216 };
  REQUIRE(gs03_isotope.hl == halflife);
}


TEST_CASE("Read the major decay mode", "[NUBASEData]")
{
  NUBASE::Data gs03_isotope("229 0880   229Ra   32563       19                              4.0    m 0.2    5/2(+)     "
                            "   90           B-=100");

  NUBASE::Data isomer03_isotope("076 0371   76Rbm  -60162.9      1.9     316.93    0.08         3.050 us 0.007  (4+)   "
                                "       95 00Ch07t   IT=100");


  gs03_isotope.setDecayMode(2003);
  REQUIRE_THAT(gs03_isotope.decay, Catch::Matches("B-"));

  isomer03_isotope.setDecayMode(2003);
  REQUIRE_THAT(isomer03_isotope.decay, Catch::Matches("IT"));
}


TEST_CASE("Read the year", "[NUBASEData]")
{
  NUBASE::Data gs12_isotope("108 0460   108Pd  -89524.4      1.1                          stbl              0+         "
                            "   08          1935 IS=26.46 9");
  gs12_isotope.setYear();
  REQUIRE(gs12_isotope.year == 1935);

  NUBASE::Data isomer12_isotope("075 0322W  75Gen  -71664.78     0.09    192.18    0.07       216     ns 5      5/2+   "
                                "       99          1982 IT=100");

  isomer12_isotope.setYear();
  REQUIRE(isomer12_isotope.year == 1982);
}


TEST_CASE("Read mass number", "[NUBASEData]")
{
  NUBASE::Data gs03_isotope("103 0490   103In  -74630        9                             60      s 1      9/2+#      "
                            "   09 97Sz04t  1978 B+=100");

  gs03_isotope.setA();

  REQUIRE(gs03_isotope.A == 103);
}


TEST_CASE("Read proton number", "[NUBASEData]")
{
  NUBASE::Data gs03_isotope("120 0480   120Cd  -83957        4                             50.80   s 0.21   0+         "
                            "   02          1973 B-=100");

  gs03_isotope.setZ();

  REQUIRE(gs03_isotope.Z == 48);
}


TEST_CASE("Calculate neutron number", "[NUBASEData]")
{
  NUBASE::Data gs03_isotope("012 0080   12O     31915       24                             >6.3   zs        0+         "
                            "   00 12Ja11t  1978 2p=60 30");

  gs03_isotope.setA();
  gs03_isotope.setZ();

  gs03_isotope.setN();

  REQUIRE(gs03_isotope.N == 4);
}


TEST_CASE("Read mass excess", "[NUBASEData]")
{
  NUBASE::Data gs03_isotope("164 0670   164Ho  -64980.8      2.3                           29      m 1      1+         "
                            "   01          1938 EC=60 5;B-=40 5");

  gs03_isotope.setMassExcess();

  REQUIRE(gs03_isotope.mass_excess == Approx(-64980.8));
}


TEST_CASE("Read mass excess error", "[NUBASEData]")
{
  NUBASE::Data gs03_isotope("141 0620   141Sm  -75934        9                             10.2    m 0.2    1/2+       "
                            "   01          1967 B+=100");

  gs03_isotope.setMassExcessError();

  REQUIRE(gs03_isotope.dmass_excess == Approx(9));
}


TEST_CASE("Calculate relative error on mass excess", "[NUBASEData]")
{
  SECTION("Random isotope is processed correctly")
  {
    NUBASE::Data gs03_isotope("119 0530   119I   -83766       28                             19.1    m 0.4    5/2+     "
                              "     09          1954 e+=51 4;EC=49 4");

    // -83766
    gs03_isotope.setMassExcess();
    // 28
    gs03_isotope.setMassExcessError();

    // 15/33051 = 0.0005
    REQUIRE(gs03_isotope.getRelativeMassExcessError(1.0e-7) == Approx(std::fabs(28.0 / -83766.0)));
    REQUIRE(gs03_isotope.getRelativeMassExcessError(0.1) == Approx(0.1));
  }

  SECTION("12C and it's annoying 0.0 ME is handled correctly")
  {
    const std::string nubase03_12C{ "012 0060   12C         0.0      0.0                          stbl              0+ "
                                    "           00           IS=98.938 " };
    NUBASE::Data nubase03_12C_isotope(nubase03_12C);

    nubase03_12C_isotope.setA();
    nubase03_12C_isotope.setZ();
    nubase03_12C_isotope.setMassExcess();
    nubase03_12C_isotope.setMassExcessError();

    REQUIRE(nubase03_12C_isotope.getRelativeMassExcessError(1.0e-5) == Approx(1.0e-5));
  }
}


TEST_CASE("Set the neutron/proton rich value", "[NUBASEData]")
{
  NUBASE::Data data = NUBASE::Data("");

  SECTION("Proton rich")
  {
    data.setNeutronOrProtonRich(false);
    REQUIRE(data.rich == 2);
  }

  SECTION("Neutron rich")
  {
    data.setNeutronOrProtonRich(true);
    REQUIRE(data.rich == 3);
  }

  SECTION("Stable")
  {
    data.decay = "stable";
    data.setNeutronOrProtonRich(true);
    REQUIRE(data.rich == 6);
  }

  SECTION("Special cases of 96Tc and 144Pm that have no stable isotope")
  {
    SECTION("96Tc")
    {
      data.Z = 43;

      data.A = 90;
      data.setNeutronOrProtonRich(true);
      REQUIRE(data.rich == 2);

      data.A = 97;
      data.setNeutronOrProtonRich(true);
      REQUIRE(data.rich == 3);
    }

    SECTION("144Pm")
    {
      data.Z = 61;

      data.A = 140;
      data.setNeutronOrProtonRich(true);
      REQUIRE(data.rich == 2);

      data.A = 145;
      data.setNeutronOrProtonRich(true);
      REQUIRE(data.rich == 3);
    }
  }
}
