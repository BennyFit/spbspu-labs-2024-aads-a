#include <iostream>
#include <functional>
#include <fstream>
#include "commands.hpp"

int main()
{
  using namespace piyavkin;
  dic_t dicts;
  Tree< std::string, std::function< void(std::istream&, const dic_t&) > > cmdsForOutput;
  cmdsForOutput["printdictionary"] = print;
  cmdsForOutput["topfreq"] = std::bind(topFreq, std::placeholders::_1, std::ref(std::cout), std::placeholders::_2);
  Tree< std::string, std::function< iterator(std::istream&, dic_t&) > > cmdsForCreate;
  cmdsForCreate["adddictionary"] = addDict;
  cmdsForCreate["changedictionary"] = cmdChange;
  cmdsForCreate["makedictionary"] = makeDict;
  cmdsForCreate["intersectdictionaries"] = intersect;
  cmdsForCreate["uniondictionaries"] = unionD;
  cmdsForCreate["uniquedictionaries"] = uniqueD;
  cmdsForCreate["adddictionarytodictionary"] = addDtoD;
  cmdsForCreate["subdictionarytodictionary"] = subD;
  std::string name = "";
  while (std::cin >> name)
  {
    try
    {
      cmdsForCreate.at(name)(std::cin, dicts);
    }
    catch (const std::out_of_range&)
    {
      try
      {
        cmdsForOutput.at(name)(std::cin, dicts);
      }
      catch (const std::out_of_range&)
      {
        std::cout << "<INVALID COMMAND>\n";
      }
      catch (const std::exception& e)
      {
        std::cerr << e.what() << '\n';
        return 1;
      }
    }
    std::cin.clear();
    std::cin.ignore(std::numeric_limits< std::streamsize >::max(), '\n');
  }
}
