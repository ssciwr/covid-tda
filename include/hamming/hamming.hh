#ifndef _HAMMING_HH
#define _HAMMING_HH

#include<array>
#include<cstdint>
#include<string>
#include<vector>

namespace hamming {

struct DataSet
{
  DataSet(const std::vector<std::string>&);
  DataSet(const std::string&);
  void dump(const std::string&);
  int operator[](const std::array<std::size_t, 2>&) const;

  std::size_t nsamples;
  std::vector<int> result;
};

DataSet from_stringlist(const std::vector<std::string>&);
DataSet from_csv(const std::string&);
DataSet from_fasta(const std::string&, std::size_t n = 0);

}

#endif
