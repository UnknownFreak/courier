#include "stresstest.hpp"
#include <catch2/catch_session.hpp>
#include <catch2/internal/catch_clara.hpp>

int main( int argc, char* argv[] )
{
  Catch::Session session;

  bool stresstest = false;
  int numCores = -1;
  int numTimes = 3;
  size_t numItems = 1000000;

  using namespace Catch::Clara;
  auto cli = session.cli()
    | Opt(stresstest)["--stresstest"]("Run courier stresstest")
    | Opt(numCores, "cores")["--cores"]("Number of cores for stresstest")
    | Opt(numItems, "subscribers")["--subscribers"]("Number of subscribers for stresstest")
    | Opt(numTimes, "repeats")["--repeats"]("Number of times to re-run the stresstest");

  session.cli(cli);

  int returnCode = session.applyCommandLine( argc, argv );
  if( returnCode != 0 )
      return returnCode;
  if(stresstest)
  {
    stresstest::run(numCores, numItems, numTimes);
    return 0;
  }

  return session.run();
}