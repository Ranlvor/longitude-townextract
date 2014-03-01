
#include "Lookup.h"
#include "stringToNumber.h"
#include "Output.h"

#include <iostream>
#include <string>
#include <vector>

//string split code from http://www.cplusplus.com/faq/sequences/strings/split/

template <typename Container>
Container& split(
  Container&                                 result,
  const typename Container::value_type&      s,
  typename Container::value_type::value_type delimiter)
{
  result.clear();
  std::istringstream ss( s );
  while (!ss.eof())
  {
    typename Container::value_type field;
    getline( ss, field, delimiter );
    result.push_back( field );
  }
  return result;
}


int main() {
    std::vector<std::string> fields;
    Lookup l;
    for (std::string line; std::getline(std::cin, line);) {
        Output::debug("line: %s", line.c_str());
        split( fields, line, ',' );
        if(fields.size() != 3) {
            Output::warn("invalid number of arguments on line '%s'", line.c_str());
            break;
        }
        std::string id = fields.at(0);
        double latitude = StringToNumber<double>(fields.at(1));
        double longitude = StringToNumber<double>(fields.at(2));
        Output::debug("id: %s, latitude: %f, lontidue: %f", id.c_str(), latitude, longitude);
        std::string result = l.lookup(latitude, longitude);
        fprintf(stdout, "%s,%s\n", id.c_str(), result.c_str());
    }
    return 0;
}
