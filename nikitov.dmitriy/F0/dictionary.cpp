#include "dictionary.hpp"
#include <limits>
#include "delimiter.hpp"

void nikitov::Dictionary::addTranslation(const std::string& word, const std::string& translation)
{
  auto value = data_.insert({ word, detail::Word(translation) });
  if (!value.second)
  {
    value.first->second.getSecondaryTranslation() = translation;
  }
}

void nikitov::Dictionary::addAntonym(const std::string& word, const std::string& antonym)
{
  auto iterToWord = data_.find(word);
  auto iterToAntonym = data_.find(antonym);
  if (iterToWord != data_.end() && iterToAntonym != data_.end())
  {
    if (iterToWord->second.getAntonym().empty() && iterToAntonym->second.getAntonym().empty())
    {
      iterToWord->second.getAntonym() = antonym;
      iterToAntonym->second.getAntonym() = word;
    }
    else
    {
      throw std::logic_error("<ANTONYM ALREADY EXIST>");
    }
  }
  else
  {
    throw std::logic_error("<INVALID COMMAND>");
  }
}

void nikitov::Dictionary::editPrimaryTranslation(const std::string& word, const std::string& translation)
{
  auto iterToWord = data_.find(word);
  if (iterToWord != data_.end())
  {
    iterToWord->second.getPrimaryTranslation() = translation;
  }
  else
  {
    throw std::logic_error("<INVALID COMMAND>");
  }
}

void nikitov::Dictionary::editSecondaryTranslation(const std::string& word, const std::string& translation)
{
  auto iterToWord = data_.find(word);
  if (iterToWord != data_.end())
  {
    iterToWord->second.getSecondaryTranslation() = translation;
  }
  else
  {
    throw std::logic_error("<INVALID COMMAND>");
  }
}

void nikitov::Dictionary::deletePrimaryTranslation(const std::string& word)
{
  auto iterToWord = data_.find(word);
  if (iterToWord != data_.end())
  {
    if (iterToWord->second.getSecondaryTranslation().empty())
    {
      deleteAntonym(word);
      data_.erase(iterToWord);
    }
    else
    {
      std::swap(iterToWord->second.getPrimaryTranslation(), iterToWord->second.getSecondaryTranslation());
      iterToWord->second.getSecondaryTranslation() = {};
    }
  }
  else
  {
    throw std::logic_error("<INVALID COMMAND>");
  }
}

void nikitov::Dictionary::deleteSecondaryTranslation(const std::string& word)
{
  auto iterToWord = data_.find(word);
  if (iterToWord != data_.end())
  {
    if (iterToWord->second.getSecondaryTranslation().empty())
    {
      throw std::logic_error("<NOTHIND TO DO>");
    }
    else
    {
      iterToWord->second.getSecondaryTranslation() = {};
    }
  }
  else
  {
    throw std::logic_error("<INVALID COMMAND>");
  }
}

void nikitov::Dictionary::deleteAntonym(const std::string& word)
{
  auto iterToWord = data_.find(word);
  if (iterToWord != data_.end())
  {
    auto iterToAntonym = data_.find(iterToWord->second.getAntonym());
    iterToWord->second.getAntonym() = {};
    if (iterToAntonym != data_.end() && iterToAntonym->second.getAntonym() == word)
    {
      iterToAntonym->second.getAntonym() = {};
    }
  }
  else
  {
    throw std::logic_error("<INVALID COMMAND>");
  }
}

void nikitov::Dictionary::printTranslation(const std::string& word, std::ostream& output) const
{
  auto iterToWord = data_.find(word);
  if (iterToWord != data_.end())
  {
    output << word << " - " << iterToWord->second << '\n';
  }
  else
  {
    throw std::logic_error("<NO MATCHES>");
  }
}

void nikitov::Dictionary::printAntonym(const std::string& word, std::ostream& output) const
{
  auto iterToWord = data_.find(word);
  if (iterToWord != data_.end())
  {
    if (!iterToWord->second.getAntonym().empty())
    {
      printTranslation(iterToWord->second.getAntonym(), output);
    }
    else
    {
      throw std::logic_error("<NO ANTONYM>");
    }
  }
  else
  {
    throw std::logic_error("<NO MATCHES>");
  }
}

void nikitov::Dictionary::printDictionary(std::ostream& output) const
{
  for (auto i = data_.cbegin(); i != data_.cend(); ++i)
  {
    output << i->first << " - " << i->second << '\n';
  }
}

std::string nikitov::Dictionary::getTranslation(const std::string& word) const
{
  return data_.find(word)->second.getPrimaryTranslation();
}

std::istream& nikitov::operator>>(std::istream& input, Dictionary& dict)
{
  while (input)
  {
    std::string word = {};
    detail::Word translation;
    input >> word >> DelimiterChar({'-'}) >> translation;
    if (input)
    {
      dict.data_.insert({ word, translation });
    }
    else if (!input.eof())
    {
      input.clear();
      input.ignore(std::numeric_limits< std::streamsize >::max(), '\n');
    }
  }
  return input;
}