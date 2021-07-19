#include "hamming/hamming.hh"
#include "tests.hh"
#include <fstream>
#include <cstdio>

using namespace hamming;

static constexpr std::array<char, 4> valid_chars{'A', 'C', 'G', 'T'};
static constexpr std::array<char, 6> invalid_chars{' ', 'N', '*', '?', 'a', '.'};

static int dist(char c1, char c2) {
  std::vector<std::string> v{std::string{c1}, std::string{c2}};
  return from_stringlist(v)[{0, 1}];
}

TEST_CASE("distance between two equal valid characters is 0", "[distance]") {
  for (auto c : valid_chars) {
    CAPTURE(c);
    REQUIRE(dist(c, c) == 0);
  }
}

TEST_CASE("distance between any valid char & '-' is 0", "[distance]") {
  for (auto c : valid_chars) {
    CAPTURE(c);
    REQUIRE(dist(c, '-') == 0);
    REQUIRE(dist('-', c) == 0);
  }
}

TEST_CASE("distance between two different valid characters is 1",
          "[distance]") {
  for (auto c1 : valid_chars) {
    for (auto c2 : valid_chars) {
      if (c1 != c2) {
        CAPTURE(c1);
        CAPTURE(c2);
        REQUIRE(dist(c1, c2) == 1);
      }
    }
  }
}

TEST_CASE("distance between valid & invalid characters is 1", "[distance]") {
  for (auto c1 : valid_chars) {
    for (auto c2 : invalid_chars) {
      CAPTURE(c1);
      CAPTURE(c2);
      CAPTURE((int)c2);
      REQUIRE(dist(c1, c2) == 1);
    }
  }
}

TEST_CASE("distance between two invalid characters is 1", "[distance]") {
  for (auto c1 : invalid_chars) {
    for (auto c2 : invalid_chars) {
      CAPTURE(c1);
      CAPTURE(c2);
      REQUIRE(dist(c1, c2) == 1);
    }
  }
}

TEST_CASE("distance between any invalid char & '-' is 1", "[distance]") {
  for (auto c : invalid_chars) {
    CAPTURE(c);
    REQUIRE(dist(c, '-') == 1);
    REQUIRE(dist('-', c) == 1);
  }
}

TEST_CASE("two expressions with distance 2", "[hamming]") {
  std::vector<std::vector<std::string>> expr;
  expr.push_back({{"AC"}, {"CA"}});
  expr.push_back({{"AC"}, {"TG"}});
  expr.push_back({{"ACG"}, {"AGT"}});
  expr.push_back({{"ACG"}, {"-TT"}});
  expr.push_back({{"ACG"}, {"T-T"}});
  expr.push_back({{"ACG"}, {"TA-"}});
  expr.push_back({{"ACG"}, {"CCC"}});
  expr.push_back({{"ACGT"}, {"AGGG"}});
  expr.push_back({{"ACGTGTCGTGTCGACGTGTCG"}, {"ACGTGTCGTTTCGACGAGTCG"}});
  expr.push_back({{"ACGTGTCGTGTCGACGTGTCGT"}, {"ACGTGTCGTTTCGACGAGTCGT"}});
  expr.push_back({{"ACGTGTCGTGTCGACGTGTCGT-"}, {"ACGTGTCGTTTCGACGAGTCGTA"}});
  expr.push_back({{"ACGTGTCGTGTCGACGTGTCG----------"}, {"ACGTGTCGTTTCGACGAGTCGGGG-------"}});
  expr.push_back({{"ACGTGTCGTGTCGACGTGTCG----------A"}, {"ACGTGTCGTTTCGACGAGTCGGGG-------A"}});
  expr.push_back({{"ACGTGTCGTGTCGACGTGTCG----------AG"}, {"ACGTGTCGTTTCGACGAGTCGGGG-------AG"}});
  expr.push_back({{"ACGTGTCGTGTCGACGTGTCG---ACGTGTCGTGTCGACGTGTCG---ACGTGTCGTGTCGACGTGTCG---"}, {"ACGTGTCGTTTCGACGAGTCGGGGACGTGTCGTGTCGACGTGTCG---ACGTGTCGTGTCGACGTGTCG---"}});
  for (auto &v : expr) {
    auto d = from_stringlist(v);
    REQUIRE(d[{0, 0}] == 0);
    REQUIRE(d[{0, 1}] == 2);
    REQUIRE(d[{1, 0}] == 2);
    REQUIRE(d[{1, 1}] == 0);
  }
}

TEST_CASE("from_fasta single line sequences", "[hamming]") {
    char tmp_file_name[L_tmpnam];
    REQUIRE(std::tmpnam(tmp_file_name) != nullptr);
    CAPTURE(tmp_file_name);
    std::ofstream of(tmp_file_name);
    of << ">seq0\n";
    of << "ACGTGTCGTGTCGACGTGTCG\n";
    of << ">seq1\n";
    of << "ACGTGTCGTTTCGACGAGTCG\n";
    of.close();
    for(bool remove_duplicates : {false, true}){
      for(bool include_x : {false, true}){
        CAPTURE(include_x);
        CAPTURE(remove_duplicates);
        for(int n : {0, 2, 3, 8}){
            auto d = from_fasta(tmp_file_name, include_x, remove_duplicates, n);
            REQUIRE(d[{0, 0}] == 0);
            REQUIRE(d[{0, 1}] == 2);
            REQUIRE(d[{1, 0}] == 2);
            REQUIRE(d[{1, 1}] == 0);
        }
      }
    }
    std::remove(tmp_file_name);
}

TEST_CASE("from_fasta single line sequences with duplicates", "[hamming]") {
    char tmp_file_name[L_tmpnam];
    REQUIRE(std::tmpnam(tmp_file_name) != nullptr);
    CAPTURE(tmp_file_name);
    std::ofstream of(tmp_file_name);
    of << ">seq0\n";
    of << "ACGTGTCGTGTCGACGTGTCG\n";
    of << ">seq1\n";
    of << "ACGTGTCGTTTCGACGAGTCG\n";
    of << ">seq2\n";
    of << "ACGTGTCGTTTCGACGAGTCG\n";
    of << ">seq3\n";
    of << "ACGTGTCGTTTCGACGAGTCG\n";
    of << ">seq4\n";
    of << "ACGTGTCGTGTCGACGTGTCG\n";
    of << ">seq5\n";
    of << "ACGTGTCGTATCGACGTGTCG\n";
    of.close();
    std::vector<std::size_t> sequence_indices{0, 1, 1, 1, 0, 2};
    for(int n : {0, 6, 22}){
      for(bool include_x : {false, true}){
        CAPTURE(include_x);
        auto d = from_fasta(tmp_file_name, include_x, true, n);
        REQUIRE(d.nsamples == 3);
        REQUIRE(d.sequence_indices == sequence_indices);
        REQUIRE(d[{0, 0}] == 0);
        REQUIRE(d[{0, 1}] == 2);
        REQUIRE(d[{0, 2}] == 1);
        REQUIRE(d[{1, 0}] == 2);
        REQUIRE(d[{1, 1}] == 0);
        REQUIRE(d[{1, 2}] == 2);
        REQUIRE(d[{2, 0}] == 1);
        REQUIRE(d[{2, 1}] == 2);
        REQUIRE(d[{2, 2}] == 0);
      }
    }
    std::remove(tmp_file_name);
}

TEST_CASE("from_fasta multi-line sequences", "[hamming]") {
    char tmp_file_name[L_tmpnam];
    REQUIRE(std::tmpnam(tmp_file_name) != nullptr);
    CAPTURE(tmp_file_name);
    std::ofstream of(tmp_file_name);
    of << ">seq0\n";
    of << "ACGTGTCGTGTCGACGTGTCG\n";
    of << "ACGTGTCGTGTCGACGTGTCG\n";
    of << "ACGTGTCGTGTCG\n";
    of << ">seq1\n";
    of << "ACGTGTCGTTTCGACGAGTCG\n";
    of << "ACGTGTCGTGTCGACGTGTCG\n";
    of << "ACGTGTCGTGTCG\n";
    of.close();
    for(bool remove_duplicates : {false, true}){
      for(bool include_x : {false, true}){
        CAPTURE(include_x);
        for(int n : {0, 2, 3, 8}){
            auto d = from_fasta(tmp_file_name, include_x, remove_duplicates, 2);
            REQUIRE(d[{0, 0}] == 0);
            REQUIRE(d[{0, 1}] == 2);
            REQUIRE(d[{1, 0}] == 2);
            REQUIRE(d[{1, 1}] == 0);
        }
      }
    }
    std::remove(tmp_file_name);
}

TEST_CASE("invalid input data: empty sequence", "[invalid]") {
    std::vector<std::vector<std::string>> expr;
    expr.push_back({});
    expr.push_back({{""}, {""}});
    std::string msg{"Error: Empty sequence"};
    for (auto &v : expr) {
        REQUIRE_THROWS_WITH(from_stringlist(v), msg);
    }
}

TEST_CASE("invalid input data: inconsistent sequence lengths", "[invalid]") {
    std::vector<std::vector<std::string>> expr;
    expr.push_back({{"ACGT"}, {"A"}});
    expr.push_back({{"AC"}, {"ACGTCG"}});
    expr.push_back({{"ACGT"}, {"ACGT"}, {"ACGT"}, {"A"}, {"ACGT"}, {"ACGT"}});
    expr.push_back({{"ACGT"}, {"A-----"}});
    expr.push_back({{"ACGT"}, {""}});
    std::string msg{"Error: Sequences do not all have the same length"};
    for (auto &v : expr) {
        REQUIRE_THROWS_WITH(from_stringlist(v), msg);
    }
}

TEST_CASE("from_csv reproduces correct data", "[hamming]") {
    std::mt19937 gen(12345);
    std::vector<std::string> data(10);
    for(auto& d : data)
        d = make_test_string(201, gen);

    DataSet ref(data);
    char tmp_file_name[L_tmpnam];
    REQUIRE(std::tmpnam(tmp_file_name) != nullptr);
    ref.dump(std::string(tmp_file_name));

    auto restore = from_csv(std::string(tmp_file_name));
    REQUIRE(ref.nsamples == restore.nsamples);
    for(std::size_t i=0; i<ref.nsamples; ++i) {
        for(std::size_t j=0; j<ref.nsamples; ++j) {
            REQUIRE(ref[{i, j}] == restore[{i, j}]);
        }
    }
    std::remove(tmp_file_name);
}


// skip this test of openmp is enabled: affects exception handling
#ifndef HAMMING_WITH_OPENMP
TEST_CASE("throws on distance integer overflow", "[hamming]") {
    auto n = std::numeric_limits<DistIntType>::max() + 1;
    std::mt19937 gen(12345);
    std::vector<std::string> data(2);
    data[0] = std::string(n, 'A');
    data[1] = std::string(n, 'T');
    std::string msg{"Error: Distance is too large for chosen integer type"};
    REQUIRE_THROWS_WITH(DataSet(data), msg);
}
#endif

TEST_CASE("from_lower_triangular reproduces correct data", "[hamming]") {
    std::mt19937 gen(12345);
    char tmp_file_name[L_tmpnam];
    REQUIRE(std::tmpnam(tmp_file_name) != nullptr);
    for (auto n : {1, 2, 3, 5, 9, 10, 19, 100, 207}) {
      CAPTURE(n);
      std::vector<std::string> data(n);
      for(auto& d : data)
          d = make_test_string(24, gen);

      DataSet ref(data);
      REQUIRE(ref.nsamples == n);
      ref.dump_lower_triangular(std::string(tmp_file_name));

      auto restore = from_lower_triangular(std::string(tmp_file_name));
      REQUIRE(ref.nsamples == restore.nsamples);
      for(std::size_t i=0; i<ref.nsamples; ++i) {
          for(std::size_t j=0; j<ref.nsamples; ++j) {
              REQUIRE(ref[{i, j}] == restore[{i, j}]);
          }
      }
    }
    std::remove(tmp_file_name);
}
