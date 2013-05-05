/*
 * CsvMapReader.cpp
 *
 *  Created on: 06.06.2011
 *      Author: wbam
 */

#include "CsvMapReader.hpp"
#include "../src-lib/Error.hpp"
#include <boost/tokenizer.hpp>
#include <iostream>

using namespace boost;
using namespace std;

namespace cpprob
{

  CsvMapReader::CsvMapReader(const string& file_name)
      : is()
  {
    is.exceptions(ifstream::badbit);
    is.open(file_name.c_str());
    if (!is)
      cpprob_throw_runtime_error(
          "Could not open the file " << file_name << ".");
  }

  CsvMapReader::~CsvMapReader()
  {
  }

  std::vector<std::map<std::string, std::string> >
  CsvMapReader::read_rows()
  {
    string line;
    vector<map<string, string> > rows;

    // Read the header
    getline(is, line);
    tokenizer<> headerTok(line);
    vector<string> header(headerTok.begin(), headerTok.end());

    cout << "CSV header: ";
    copy(header.begin(), header.end(), ostream_iterator<string>(cout, " "));
    cout << endl;

    // Read the body
    while (getline(is, line))
      {
        tokenizer<> bodyTok(line);
        vector<string>::const_iterator headerIt = header.begin();
        tokenizer<>::const_iterator bodyIt = bodyTok.begin();
        map<string, string> row;
        for (; bodyIt != bodyTok.end(); ++bodyIt, ++headerIt)
          {
          row.insert(make_pair(*headerIt, *bodyIt));
        }
      rows.push_back(row);
    }

  return rows;
}

}
