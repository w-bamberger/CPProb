/*
 * main.cpp
 *
 *  Created on: 13.05.2011
 *      Author: wbam
 */

#include <boost/program_options.hpp>
#include <boost/test/unit_test.hpp>
#include <iostream>

#ifdef __GNUC__
extern "C" void
__libc_freeres(void);
#endif

using namespace boost;
using namespace std;

program_options::options_description options_desc("Usage of the tests");
program_options::variables_map options_map;

bool init_unit_test()
{
  assign_op(unit_test::framework::master_test_suite().p_name.value,
      BOOST_TEST_STRINGIZE( CPProb ).trim("\""), 0);

  int& argc = unit_test::framework::master_test_suite().argc;
  char** argv = unit_test::framework::master_test_suite().argv;

  program_options::store(
      program_options::parse_command_line(argc, argv, options_desc),
      options_map);
  program_options::notify(options_map);

  return true;
}

int
main(int argc, char **argv)
{
#ifdef __GNUC__
  atexit(__libc_freeres);
#endif

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
