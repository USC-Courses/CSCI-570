#include <errno.h>
#include <stdio.h>
#include <sys/resource.h>

#include <algorithm>
#include <cctype>
#include <chrono>
#include <climits>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

extern int error;

struct TestCase {
  std::string s1;
  std::string s2;
  std::string alignment1;
  std::string alignment2;
  double time_elapsed = 0;
  int64_t memory_used = 0;
  int32_t alignment_cost = 0;
};

class FileIO {
 public:
  void Parse(int argc, char* argv[]) {
    if (argc != 3) {
      std::string name = argv[0];
      size_t pos = name.find_last_of('/');
      name = name.substr(pos == std::string::npos ? 0 : pos + 1);
      std::cout << "OPTIONS: " << name << " [ <INPUT_FILE> <OUTPUT_FILE> ]" << std::endl;
      exit(1);
    } else {
      input_path_ = argv[1];
      output_path_ = argv[2];
    }
  }

  void ReadData(TestCase& test_case) {
    std::ifstream fin(input_path_);
    if (!fin.is_open()) {
      std::cout << "Failed to open the input file: " << input_path_ << std::endl;
      exit(2);
    }

    GenerateString(fin, test_case.s1);
    GenerateString(fin, test_case.s2);
    fin.close();
  }

  void WriteResult(TestCase& test_case) {
    std::ofstream fout(output_path_);
    if (!fout.is_open()) {
      std::cout << "Failed to open the output file: " << output_path_ << std::endl;
      exit(2);
    }

    fout << test_case.alignment_cost << "\n"  // alignment cost
         << test_case.alignment1 << "\n"      // alignment of s1
         << test_case.alignment2 << "\n"      // alignment of s2
         << test_case.time_elapsed << "\n"    // time elapsed
         << test_case.memory_used;            // memory used
    fout.close();
  }

 private:
  void GenerateString(std::ifstream& fin, std::string& s) {
    char c;
    int32_t pos;
    fin >> s;
    fin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    while (isdigit(c = fin.get())) {
      fin.unget();
      fin >> pos;
      fin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

      int n = s.size();
      pos += 1;
      s.resize(n + n);
      std::copy(s.rbegin() + n, s.rbegin() + 2 * n, s.rbegin() + n - pos);
      std::copy(s.rbegin() + n - pos, s.rbegin() + (n - pos) * 2, s.rbegin());
    }
    fin.unget();
  }

 private:
  std::string input_path_;
  std::string output_path_;
};

class BasicSolution {
 public:
  void Solve(TestCase& test_case) {
    auto start = std::chrono::system_clock::now();

    SolveInternal(test_case);

    test_case.memory_used = GetTotalMemory();
    auto end = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    test_case.time_elapsed = duration.count() * 1e-6;
  }

 private:
  void SolveInternal(TestCase& test_case) {
    auto& s1 = test_case.s1;
    auto& s2 = test_case.s2;
    int32_t n = s1.size();
    int32_t m = s2.size();
    std::vector<std::vector<int32_t>> dp(n + 1, std::vector<int32_t>(m + 1, 0));

    for (int i = 1; i <= n; ++i) {
      dp[i][0] = dp[i - 1][0] + kGapPenalty;
    }
    for (int i = 1; i <= m; ++i) {
      dp[0][i] = dp[0][i - 1] + kGapPenalty;
    }

    // DP
    for (int i = 0; i < n; ++i) {
      for (int j = 0; j < m; ++j) {
        int32_t mismatch_penalty = GetMismatchPenalty(s1[i], s2[j]);
        dp[i + 1][j + 1] = std::min(dp[i + 1][j], dp[i][j + 1]) + kGapPenalty;
        dp[i + 1][j + 1] = std::min(dp[i + 1][j + 1], dp[i][j] + mismatch_penalty);
      }
    }
    test_case.alignment_cost = dp[n][m];

    // Backtracking
    auto& alignment1 = test_case.alignment1;
    auto& alignment2 = test_case.alignment2;
    alignment1.reserve(std::max(n, m));
    alignment2.reserve(std::max(n, m));
    for (int i = n - 1, j = m - 1; i >= 0 || j >= 0;) {
      if (i >= 0 && j >= 0 && dp[i + 1][j + 1] == dp[i][j] + GetMismatchPenalty(s1[i], s2[j])) {
        alignment1 += s1[i--];
        alignment2 += s2[j--];
      } else if (j >= 0 && dp[i + 1][j + 1] == dp[i + 1][j] + kGapPenalty) {
        alignment1 += '_';
        alignment2 += s2[j--];
      } else if (i >= 0 && dp[i + 1][j + 1] == dp[i][j + 1] + kGapPenalty) {
        alignment1 += s1[i--];
        alignment2 += '_';
      }
    }
    std::reverse(alignment1.begin(), alignment1.end());
    std::reverse(alignment2.begin(), alignment2.end());
  }

  inline int32_t GetMismatchPenalty(char lhs, char rhs) {
    return kMismatchPenalty[kCharToIndex[lhs - 'A']][kCharToIndex[rhs - 'A']];
  }

  int64_t GetTotalMemory() {
    struct rusage usage;
    int returnCode = getrusage(RUSAGE_SELF, &usage);
    if (returnCode == 0) {
      return usage.ru_maxrss;
    } else {
      // It should never occur. Check man getrusage for more info to debug.
      printf("error %d", errno);
      exit(3);
    }
  }

 private:
  static const int32_t kGapPenalty;
  static const int32_t kCharToIndex[26];
  static const int32_t kMismatchPenalty[4][4];
};

const int32_t BasicSolution::kGapPenalty = 30;
const int32_t BasicSolution::kCharToIndex[26] = {0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0,
                                                 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0};
const int32_t BasicSolution::kMismatchPenalty[4][4] = {{0, 110, 48, 94},   // A
                                                       {110, 0, 118, 48},  // C
                                                       {48, 118, 0, 110},  // G
                                                       {94, 48, 110, 0}};  // T

int main(int argc, char* argv[]) {
  FileIO file_io;
  TestCase test_case;
  BasicSolution solution;
  file_io.Parse(argc, argv);
  file_io.ReadData(test_case);
  solution.Solve(test_case);
  file_io.WriteResult(test_case);
  return 0;
}
