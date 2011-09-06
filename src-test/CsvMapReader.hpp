/*
 * CsvMapReader.hpp
 *
 *  Created on: 06.06.2011
 *      Author: wbam
 */

#ifndef CSVMAPREADER_HPP_
#define CSVMAPREADER_HPP_

#include <fstream>
#include <map>
#include <string>
#include <vector>

namespace cpprob
{

  class CsvMapReader
  {

  public:

    typedef std::map<std::string, std::string> NamedAttributes;
    typedef std::vector<NamedAttributes> Records;

    CsvMapReader(const std::string& file_name);

    ~CsvMapReader();

    Records
    read_rows();

  private:

    std::ifstream is;

  };

}

#endif /* CSVMAPREADER_HPP_ */
