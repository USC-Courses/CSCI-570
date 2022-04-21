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
#include <ios>
#include <iostream>
#include <istream>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
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

class EfficientSolution {
 public:
  static const int32_t kGapPenalty;
  static const int32_t kCharToIndex[26];
  static const int32_t kMismatchPenalty[4][4];

 public:
  EfficientSolution() = default;

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
  using Result = std::pair<int32_t, std::pair<std::string, std::string>>;

  void SolveInternal(TestCase& test_case) {
    std::string& s1 = test_case.s1;
    std::string& s2 = test_case.s2;
    std::string rev_s1(s1.rbegin(), s1.rend());
    std::string rev_s2(s2.rbegin(), s2.rend());

    // allocate memory for dp buffer
    dp.resize(2, std::vector<int32_t>(s2.size() + 1));
    auto ret = DivideAndConquer(s1, s2, rev_s1, rev_s2);

    test_case.alignment_cost = ret.first;
    test_case.alignment1 = ret.second.first;
    test_case.alignment2 = ret.second.second;
  }

  Result DivideAndConquer(std::string_view s1, std::string_view s2, std::string_view rev_s1, std::string_view rev_s2) {
    int32_t n = s1.size();
    int32_t m = s2.size();
    int32_t penalty_cost = 0;
    std::string alignment1;
    std::string alignment2;
    if (n == 0 && m == 0) {
      // do nothing
    } else if (!n) {
      penalty_cost = kGapPenalty * m;
      alignment1 = std::string(m, '_');
      alignment2 = s2;
    } else if (!m) {
      penalty_cost = kGapPenalty * n;
      alignment1 = s1;
      alignment2 = std::string(n, '_');
    } else if (n == 1) {
      char c = s1[0];
      auto comp = [&](const char lhs, const char rhs) -> bool {
        return GetMismatchPenalty(c, lhs) < GetMismatchPenalty(c, rhs);
      };
      auto it = std::min_element(s2.begin(), s2.end(), comp);
      int32_t pos = it - s2.begin();
      int32_t mismatch_penalty = GetMismatchPenalty(c, *it);
      if (mismatch_penalty < 2 * kGapPenalty) {
        penalty_cost = (m - 1) * kGapPenalty + mismatch_penalty;
        alignment1 = std::string(pos, '_') + c + std::string(m - pos - 1, '_');
        alignment2 = s2;
      } else {
        penalty_cost = (m + 1) * kGapPenalty;
        alignment1 = c + std::string(m, '_');
        (alignment2 = '_') += s2;
      }
    } else {
      // Divide
      int32_t s1_mid = n / 2;
      int32_t s2_mid = FindDivideIndex(s1, s2, rev_s1, rev_s2);

      // Conquer
      auto s1_left = s1.substr(0, s1_mid);
      auto s1_right = s1.substr(s1_mid);
      auto s2_left = s2.substr(0, s2_mid);
      auto s2_right = s2.substr(s2_mid);
      auto rev_s1_left = rev_s1.substr(n - s1_mid);
      auto rev_s1_right = rev_s1.substr(0, n - s1_mid);
      auto rev_s2_left = rev_s2.substr(m - s2_mid);
      auto rev_s2_right = rev_s2.substr(0, m - s2_mid);
      auto left_ret = DivideAndConquer(s1_left, s2_left, rev_s1_left, rev_s2_left);
      auto right_ret = DivideAndConquer(s1_right, s2_right, rev_s1_right, rev_s2_right);

      // Merge
      penalty_cost = left_ret.first + right_ret.first;
      (alignment1 = std::move(left_ret.second.first)) += std::move(right_ret.second.first);
      (alignment2 = std::move(left_ret.second.second)) += std::move(right_ret.second.second);
    }
    return {penalty_cost, {std::move(alignment1), std::move(alignment2)}};
  }

  int32_t FindDivideIndex(std::string_view s1, std::string_view s2, std::string_view rev_s1, std::string_view rev_s2) {
    int32_t n = s1.size();
    int32_t m = s2.size();
    int32_t s1_mid = s1.size() / 2;
    auto s1_left = s1.substr(0, s1_mid);
    auto rev_s1_right = rev_s1.substr(0, n - s1_mid);
    MemoryEfficientDP(dp[0], s1_left, s2);
    MemoryEfficientDP(dp[1], rev_s1_right, rev_s2);

    int32_t index = -1;
    int32_t min_cost = INT_MAX;
    for (int32_t i = 0; i <= m; ++i) {
      int32_t cost = dp[0][i] + dp[1][m - i];
      if (cost < min_cost) {
        min_cost = cost;
        index = i;
      }
    }
    return index;
  }

  void MemoryEfficientDP(std::vector<int32_t>& dp, std::string_view s1, std::string_view s2) {
    int32_t n = s1.size();
    int32_t m = s2.size();
    dp[0] = 0;
    for (int32_t i = 1; i <= m; ++i) {
      dp[i] = dp[i - 1] + kGapPenalty;
    }

    for (int32_t i = 0; i < n; ++i) {
      int32_t top_left = dp[0];
      dp[0] += kGapPenalty;
      for (int32_t j = 0; j < m; ++j) {
        int32_t mismatch_penalty = GetMismatchPenalty(s1[i], s2[j]);
        int32_t temp = dp[j + 1];  // keep dp[j+1] for further calculations
        dp[j + 1] = std::min(dp[j], dp[j + 1]) + kGapPenalty;
        dp[j + 1] = std::min(dp[j + 1], top_left + mismatch_penalty);
        top_left = temp;
      }
    }
  }

  inline int32_t GetMismatchPenalty(char lhs, char rhs) {
    return kMismatchPenalty[kCharToIndex[lhs - 'A']][kCharToIndex[rhs - 'A']];
  }

  double GetTotalMemory() {
    struct rusage usage;
    int returnCode = getrusage(RUSAGE_SELF, &usage);
    if (returnCode == 0) {
#if __APPLE__
      // the units of the ru_maxrss on macOS are bites
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

 private:
  std::vector<std::vector<int32_t>> dp;  // buffer
};

const int32_t EfficientSolution::kGapPenalty = 30;
const int32_t EfficientSolution::kCharToIndex[26] = {0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0,
                                                     0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0};
const int32_t EfficientSolution::kMismatchPenalty[4][4] = {{0, 110, 48, 94},   // A
                                                           {110, 0, 118, 48},  // C
                                                           {48, 118, 0, 110},  // G
                                                           {94, 48, 110, 0}};  // T

int main(int argc, char* argv[]) {
  FileIO file_io;
  TestCase test_case;
  EfficientSolution solution;
  file_io.Parse(argc, argv);
  file_io.ReadData(test_case);
  solution.Solve(test_case);
  file_io.WriteResult(test_case);
  return 0;
}
