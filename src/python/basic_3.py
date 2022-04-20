import time
import psutil
import argparse
from collections import defaultdict

parser = argparse.ArgumentParser()
parser.add_argument('input_path', help='input file path', type=str)
parser.add_argument('output_path', help='output file path', type=str)
args = parser.parse_args()


class Solution:
    # mismatch cost
    a = [
        [0, 110, 48, 94],  # A
        [110, 0, 118, 48],  # C
        [48, 118, 0, 110],  # G
        [94, 48, 110, 0]  # T
    ]
    a_index = {'A': 0, 'C': 1, 'G': 2, 'T': 3}
    # gap penalty
    delta = 30

    def __init__(self, input_file_path, output_file_path):
        self.result = defaultdict(str)
        self.input_path = input_file_path
        self.output_path = output_file_path
        self.s1 = ''
        self.s2 = ''

    def process_memory(self):
        process = psutil.Process()
        memory_info = process.memory_info()
        memory_consumed = int(memory_info.rss / 1024)  # in Kilobytes
        self.result['memory_consumed'] = str(memory_consumed)

    def time_wrapper(self):
        start_time = time.time()
        self.sequence_alignment_basic()
        end_time = time.time()
        time_taken = (end_time - start_time) * 1000  # in Milliseconds
        self.result['time_taken'] = str(time_taken)

    def read_input(self):  # read input and generate strings
        with open(self.input_path, 'r') as f:
            lines = f.readlines()
            s = []
            for line in lines:
                line = line.replace('\n', '')  # 'ACTG\n',get rid of '\n'
                if str.isalpha(line):
                    s.append(line)
                elif str.isdigit(line):
                    position = int(line)
                    string = s.pop()
                    string = string[:position + 1] + string + string[position + 1:]
                    s.append(string)
            self.s1 = s[0]
            self.s2 = s[1]

    def output(self):
        with open(self.output_path, 'w') as f:
            f.write(self.result['alignment_cost'] + '\n')
            f.write(self.result['alignment1'] + '\n')
            f.write(self.result['alignment2'] + '\n')
            f.write(self.result['time_taken'] + '\n')
            f.write(self.result['memory_consumed'])

    @classmethod
    def get_mismatch_cost(cls, char1, char2):
        return cls.a[cls.a_index[char1]][cls.a_index[char2]]

    def sequence_alignment_basic(self):
        n, m = len(self.s1), len(self.s2)  # (X..n, Y..m)
        dp = [[0] * (m + 1) for _ in range(n + 1)]

        # Base cases
        for i in range(1, n + 1):
            dp[i][0] = dp[i - 1][0] + Solution.delta
        for j in range(1, m + 1):
            dp[0][j] = dp[0][j - 1] + Solution.delta

        # DP
        for i in range(1, n + 1):
            for j in range(1, m + 1):
                char1 = self.s1[i - 1]
                char2 = self.s2[j - 1]
                dp[i][j] = min(dp[i - 1][j - 1] + Solution.get_mismatch_cost(char1, char2),
                               dp[i - 1][j] + Solution.delta, dp[i][j - 1] + Solution.delta)

        self.process_memory()

        # Backtracking
        alignment1 = []
        alignment2 = []
        i, j = n, m
        while i > 0 or j > 0:
            char1 = self.s1[i - 1]
            char2 = self.s2[j - 1]
            if i > 0 and j > 0 and dp[i][j] == dp[i - 1][j - 1] + Solution.get_mismatch_cost(
                    char1, char2):
                alignment1.append(char1)
                alignment2.append(char2)
                i -= 1
                j -= 1
            elif j > 0 and dp[i][j] == dp[i][j - 1] + Solution.delta:
                alignment1.append('_')
                alignment2.append(char2)
                j -= 1
            elif i > 0 and dp[i][j] == dp[i - 1][j] + Solution.delta:
                alignment1.append(char1)
                alignment2.append('_')
                i -= 1

        alignment1.reverse()
        alignment2.reverse()

        self.result['alignment_cost'] = str(dp[n][m])
        self.result['alignment1'] = "".join(alignment1)
        self.result['alignment2'] = "".join(alignment2)


if __name__ == '__main__':
    solution = Solution(args.input_path, args.output_path)
    solution.read_input()
    solution.time_wrapper()
    solution.output()
