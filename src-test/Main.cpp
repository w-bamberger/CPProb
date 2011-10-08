/*
 * main.cpp
 *
 *  Created on: 13.05.2011
 *      Author: wbam
 */

#include "NetworkGenerator.hpp"
#include "../src-lib/RandomBoolean.hpp"
#include "../src-lib/IoUtils.hpp"
#include "../src-lib/Utils.hpp"
#include <boost/assign.hpp>
#include <boost/foreach.hpp>
#include <boost/program_options.hpp>
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/timer.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>

extern "C" void
__libc_freeres(void);

using namespace boost;
using namespace cpprob;
using namespace std;

program_options::options_description options_desc("Usage of the tests");
program_options::variables_map options_map;

#ifdef BOOST_TEST_ALTERNATIVE_INIT_API
bool init_unit_test()
{
#else
::boost::unit_test::test_suite*
init_unit_test_suite(int, char*[])
{
#endif

  assign_op(unit_test::framework::master_test_suite().p_name.value,
      BOOST_TEST_STRINGIZE( CPProb ).trim("\""), 0);

  int& argc = unit_test::framework::master_test_suite().argc;
  char** argv = unit_test::framework::master_test_suite().argv;

  program_options::store(
      program_options::parse_command_line(argc, argv, options_desc),
      options_map);
  program_options::notify(options_map);

#ifdef BOOST_TEST_ALTERNATIVE_INIT_API
  return true;
}
#else
  return 0;
}
#endif

int
main(int argc, char **argv)
{
  atexit(__libc_freeres);

  options_desc.add_options() //
  ("burn-in-iterations",
      program_options::value<unsigned int>()->default_value(5),
      "number of iterations samples of which are not saved") //
  ("collect-iterations",
      program_options::value<unsigned int>()->default_value(500),
      "number of iterations during which the samples are counted") //
  ("data-file", program_options::value<string>(),
      "CSV file with the data used for learning and inference") //
  ("test-mode",
      program_options::value<bool>()->default_value(false)->zero_tokens(),
      "reduce the output so it fits for automated testing with texttest") //
  ("with-debug-output",
      program_options::value<bool>()->default_value(false)->zero_tokens(),
      "print detailed information about the result of every step");

  for (int i = 0; i != argc; ++i)
  {
    if (strcmp("--help", argv[i]) == 0)
    {
      ::boost::unit_test::unit_test_main(&init_unit_test, argc, argv);
      cout << "\n";
      cout << options_desc << "\n";
      return 0;
    }
  }
  return ::boost::unit_test::unit_test_main(&init_unit_test, argc, argv);
}
