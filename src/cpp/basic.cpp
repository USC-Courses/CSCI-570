#include <errno.h>
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
#include <istream>
#include <memory>
#include <string>
#include <vector>

extern int error;

struct TestCase {
 public:
  TestCase() = default;

  static void GenerateString(std::istream& is, std::string& s) {
    char c;
    int32_t idx;
    std::vector<int32_t> positions;
    (is >> s).ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    while (isdigit(c = is.get())) {
      (is.unget() >> idx).ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      positions.emplace_back(idx + 1);
    }
    is.unget();

    s.reserve(s.size() * (2 << positions.size()));
    for (auto& pos : positions) {
      int32_t n = s.size();
      s.resize(n + n);
      copy(s.rbegin() + n, s.rbegin() + 2 * n, s.rbegin() + n - pos);
      copy(s.rbegin() + n - pos, s.rbegin() + (n - pos) * 2, s.rbegin());
    }
  }

  friend std::istream& operator>>(std::istream& is, TestCase& test_case) {
    GenerateString(is, test_case.s1);
    GenerateString(is, test_case.s2);
    return is;
  }

  friend std::ostream& operator<<(std::ostream& os, const TestCase& test_case) {
    os << test_case.alignment_cost << "\n"  // alignment cost
       << test_case.alignment1 << "\n"      // alignment of s1
       << test_case.alignment2 << "\n"      // alignment of s2
       << test_case.time_elapsed << "\n"    // time elapsed
       << test_case.memory_used;            // memory used
    return os;
  }

 public:
  std::string s1;
  std::string s2;
  std::string alignment1;
  std::string alignment2;
  int32_t alignment_cost = 0;
  double time_elapsed = 0;
  double memory_used = 0;
};

class FileIO {
 public:
  FileIO() = default;
  explicit FileIO(const std::string& input_path, const std::string& output_path)
      : input_path_(input_path), output_path_(output_path) {}

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

    fin >> test_case;
    fin.close();
  }

  void WriteResult(const TestCase& test_case) {
    std::ofstream fout(output_path_);
    if (!fout.is_open()) {
      std::cout << "Failed to open the output file: " << output_path_ << std::endl;
      exit(2);
    }

    fout << test_case;
    fout.close();
  }

 private:
  std::string input_path_;
  std::string output_path_;
};

class BasicSolution {
 public:
  static const int32_t kGapPenalty;
  static const int32_t kCharToIndex[26];
  static const int32_t kMismatchPenalty[4][4];

 public:
  BasicSolution() = default;

  void Solve(TestCase& test_case) {
    auto start = std::chrono::system_clock::now();
    auto before = GetTotalMemory();

    SolveInternal(test_case);

    auto after = GetTotalMemory();
    auto end = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    test_case.memory_used = after - before;
    test_case.time_elapsed = duration.count() * 1e-6;
  }

 private:
  void SolveInternal(TestCase& test_case) {
    auto& s1 = test_case.s1;
    auto& s2 = test_case.s2;
    int32_t n = s1.size();
    int32_t m = s2.size();
    std::vector<std::vector<int32_t>> dp(n + 1, std::vector<int32_t>(m + 1, 0));

    for (int32_t i = 1; i <= n; ++i) {
      dp[i][0] = dp[i - 1][0] + kGapPenalty;
    }
    for (int32_t i = 1; i <= m; ++i) {
      dp[0][i] = dp[0][i - 1] + kGapPenalty;
    }

    // DP
    for (int32_t i = 0; i < n; ++i) {
      for (int32_t j = 0; j < m; ++j) {
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
    for (int32_t i = n - 1, j = m - 1; i >= 0 || j >= 0;) {
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

  double GetTotalMemory() {
    struct rusage usage;
    int returnCode = getrusage(RUSAGE_SELF, &usage);
    if (returnCode == 0) {
#if __APPLE__
      // the units of the ru_maxrss on macOS are bytes
      return usage.ru_maxrss / 1024.0;
#else
      // the units of the ru_maxrss on Linux are kilobytes
      return usage.ru_maxrss;
#endif
    } else {
      // It should never occur. Check man getrusage for more info to debug.
      printf("error %d", errno);
      exit(3);
    }
  }
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
