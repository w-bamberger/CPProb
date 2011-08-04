/*
 * csv_map_reader.h
 *
 *  Created on: 06.06.2011
 *      Author: wbam
 */

#ifndef CSV_MAP_READER_H_
#define CSV_MAP_READER_H_

#include <fstream>
#include <map>
#include <string>
#include <vector>

namespace vanet
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

#endif /* CSV_MAP_READER_H_ */
