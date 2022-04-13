#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

struct TestCase {
  string s1;
  string s2;
  string alignment1;
  string alignment2;
  int alignment_cost = 0;
  int time_elapsed = 0;
  int memory_used = 0;
};

class FileIO {
 public:
  void Parse(int argc, char* argv[]) {
    if (argc != 3) {
      string name = argv[0];
      int pos = name.find_last_of('/');
      name = name.substr(pos == string::npos ? 0 : pos + 1);
      cout << "OPTIONS: " << name << " [ <INPUT_FILE> <OUTPUT_FILE> ]" << endl;
      exit(1);
    } else {
      input_path_ = argv[1];
      output_path_ = argv[2];
    }
  }

  void ReadData(TestCase& test_case) {
    ifstream fin(input_path_);
    if (!fin.is_open()) {
      cout << "Failed to open the input file: " << input_path_ << endl;
      exit(2);
    }

    GenerateString(fin, test_case.s1);
    GenerateString(fin, test_case.s2);
    fin.close();
  }

  void WriteResult(TestCase& test_case) {
    ofstream fout(output_path_);
    if (!fout.is_open()) {
      cout << "Failed to open the output file: " << output_path_ << endl;
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
  void GenerateString(ifstream& fin, string& s) {
    char c;
    int pos;
    fin >> s;
    fin.ignore(numeric_limits<streamsize>::max(), '\n');
    while (isdigit(c = fin.get())) {
      fin.unget();
      fin >> pos;
      fin.ignore(numeric_limits<streamsize>::max(), '\n');

      int n = s.size();
      pos += 1;
      s.resize(n + n);
      copy(s.rbegin() + n, s.rbegin() + 2 * n, s.rbegin() + n - pos);
      copy(s.rbegin() + n - pos, s.rbegin() + (n - pos) * 2, s.rbegin());
    }
    fin.unget();
  }

 private:
  string input_path_;
  string output_path_;
};

class BasicSolution {
 public:
  void solve(TestCase& test_case) {}

 private:
  static const int kGapPenalty;
  static const int kMismatchPenalty[4][4];
};

const int BasicSolution::kGapPenalty = 30;
const int BasicSolution::kMismatchPenalty[4][4] = {{0, 110, 48, 94},   // A
                                                   {110, 0, 118, 48},  // C
                                                   {48, 118, 0, 110},  // G
                                                   {94, 48, 110, 0}};  // T

int main(int argc, char* argv[]) {
  FileIO file_io;
  TestCase test_case;
  BasicSolution solution;
  file_io.Parse(argc, argv);
  file_io.ReadData(test_case);
  solution.solve(test_case);
  file_io.WriteResult(test_case);
  return 0;
}
