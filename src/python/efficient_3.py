import time
import psutil
import argparse

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
        self.result = {'memory_consumed': 0}
        self.input_path = input_file_path
        self.output_path = output_file_path
        self.s1 = ''
        self.s2 = ''

    def process_memory(self):
        process = psutil.Process()
        memory_info = process.memory_info()
        memory_consumed = int(memory_info.rss / 1024)  # in Kilobytes
        self.result['memory_consumed'] = max(memory_consumed, self.result['memory_consumed'])

    def time_wrapper(self):
        start_time = time.time()
        self.result.update(self.divide_and_conquer(self.s1, self.s2))
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
                    string = string[: position + 1] + string + string[position + 1:]
                    s.append(string)
            self.s1 = s[0]
            self.s2 = s[1]

    def output(self):
        with open(self.output_path, 'w') as f:
            f.write(str(self.result['cost']) + '\n')
            f.write(self.result['alignment1'] + '\n')
            f.write(self.result['alignment2'] + '\n')
            f.write(self.result['time_taken'] + '\n')
            f.write(str(self.result['memory_consumed']))

    @classmethod
    def get_mismatch_cost(cls, char1, char2):
        return cls.a[cls.a_index[char1]][cls.a_index[char2]]

    @classmethod
    def sequence_alignment_efficient(cls, s1, s2):
        n, m = len(s1), len(s2)  # (X..n, Y..m)
        dp = [0] * (m + 1)

        # Base cases
        for j in range(1, m + 1):
            dp[j] = dp[j - 1] + Solution.delta

        # Memory Efficient DP
        for i in range(1, n + 1):
            top_left = dp[0]
            dp[0] += Solution.delta
            for j in range(1, m + 1):
                char1 = s1[i - 1]
                char2 = s2[j - 1]
                tmp = dp[j]
                dp[j] = min(top_left + Solution.get_mismatch_cost(char1, char2),
                            dp[j] + Solution.delta, dp[j - 1] + Solution.delta)
                top_left = tmp
        return dp

    @classmethod
    def find_divide_index(cls, s1, s2):
        n, m = len(s1), len(s2)

        s1_mid = n // 2
        s1_left = s1[:s1_mid]
        s1_right_reverse = s1[s1_mid:][::-1]
        dp = [Solution.sequence_alignment_efficient(s1_left, s2),
              Solution.sequence_alignment_efficient(s1_right_reverse, s2[::-1])]

        min_cost = float('INF')
        idx = -1
        for i in range(m + 1):
            curr_cost = dp[0][i] + dp[1][m - i]
            if curr_cost < min_cost:
                min_cost = curr_cost
                idx = i
        return idx

    def divide_and_conquer(self, s1, s2):
        n, m = len(s1), len(s2)
        alignment1 = ''
        alignment2 = ''
        if n <= 1 or m <= 1:
            dp = [[0] * (m + 1) for _ in range(n + 1)]

            # base cases
            for i in range(1, n + 1):
                dp[i][0] = dp[i - 1][0] + Solution.delta
            for j in range(1, m + 1):
                dp[0][j] = dp[0][j - 1] + Solution.delta

            # basic DP
            for i in range(1, n + 1):
                for j in range(1, m + 1):
                    dp[i][j] = min(dp[i - 1][j - 1] + Solution.get_mismatch_cost(s1[i - 1], s2[j - 1]),
                                   dp[i][j - 1] + Solution.delta, dp[i - 1][j] + Solution.delta)
            cost = dp[n][m]

            # backtrack
            i, j = n, m
            while i > 0 or j > 0:
                if i > 0 and j > 0 and \
                        dp[i][j] == dp[i - 1][j - 1] + Solution.get_mismatch_cost(s1[i - 1], s2[j - 1]):
                    alignment1 += s1[i - 1]
                    alignment2 += s2[j - 1]
                    i -= 1
                    j -= 1
                elif j > 0 and dp[i][j] == dp[i][j - 1] + Solution.delta:
                    alignment1 += '_'
                    alignment2 += s2[j - 1]
                    j -= 1
                elif i > 0 and dp[i][j] == dp[i - 1][j] + Solution.delta:
                    alignment1 += s1[i - 1]
                    alignment2 += '_'
                    i -= 1
            alignment1 = alignment1[::-1]
            alignment2 = alignment2[::-1]

        else:
            # Divide
            s1_mid = n // 2
            s2_mid = Solution.find_divide_index(s1, s2)

            # Conquer
            s1_left = s1[:s1_mid]
            s2_left = s2[:s2_mid]
            s1_right = s1[s1_mid:]
            s2_right = s2[s2_mid:]
            left = self.divide_and_conquer(s1_left, s2_left)
            right = self.divide_and_conquer(s1_right, s2_right)

            self.process_memory()

            # Merge
            cost = left['cost'] + right['cost']
            alignment1 = alignment1 + left['alignment1'] + right['alignment1']
            alignment2 = alignment2 + left['alignment2'] + right['alignment2']
        return {'cost': cost, 'alignment1': alignment1, 'alignment2': alignment2}


if __name__ == '__main__':
    solution = Solution(args.input_path, args.output_path)
    solution.read_input()
    solution.time_wrapper()
    solution.output()
