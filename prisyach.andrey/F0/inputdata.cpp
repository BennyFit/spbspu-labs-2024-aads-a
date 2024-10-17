#include "inputdata.hpp"

#include <utility>
#include <iostream>

void prisyach::makeDict(mainDic& finres, std::ifstream& file, std::istream& in)
{
  std::string name = "";
  std::string text = "";
  in >> name >> text;
  dic result;
  if (text != "")
  {
    result[text]++;
  }
  if (file.eof())
  {
    throw std::logic_error("EMPTY FILE");
  }
  while (!in.eof())
  {
    in >> text;
    result[text]++;
    if (in.get() == '\n')
    {
      break;
    }
  }
  finres.insert(name, result);
}
void prisyach::makeDictLen(mainDic& finres, std::ifstream& file, std::istream& in)
{
  std::string name = "";
  std::string text = "";
  size_t len = 0;
  in >> name >> text >> len;
  dic result;
  std::string word = "";
  if (file.eof())
  {
    throw std::logic_error("NO TEXT");
  }
  while (file >> word)
  {
    if (word == text)
    {
      break;
    }
  }
  while (file)
  {
    if (file.get() == '\n')
    {
      break;
    }
    file >> word;
    if (word.length() == len)
    {
      result[word]++;
    }
  }
  finres.insert(name, result);
}
void prisyach::read_dictionaries(std::ifstream& in, mainDic& dictionaries)
{
  std::string dataset = "";
  std::string value = "";
  size_t key = 0;
  while (in >> dataset)
  {
    dic dict;
    if (in.get() == '\n')
    {
       dictionaries.insert(dataset, dict);
       continue;
    }
    while (in >> key >> value)
    {
      dict.insert(value, key);
      if (in.get() == '\n')
      {
        break;
      }
    }
    dictionaries.insert(dataset, dict);
  }
}
