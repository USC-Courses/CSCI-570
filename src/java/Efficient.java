import java.io.*;
import java.util.*;

class Efficient {
    public static ArrayList<String> filestrflow = new ArrayList<String>();
    public static String s1;
    public static String s2;
    public static List<Integer> num1 = new ArrayList<Integer>();
    public static List<Integer> num2 = new ArrayList<Integer>();
    public static char[] t1;
    public static char[] t2;
    public static List<Integer> test2 = new ArrayList<Integer>();
    public static int metrix[][] = {{0, 110, 48, 94},
            {110, 0, 118, 48},
            {48, 118, 0, 110},
            {94, 48, 110, 0}};
    public static int opt[][];
    public static int opt2[][];
    public static int num[][];

    public int getpenaltyvalue(char a, char b) {
        int indexa = 0;
        int indexb = 0;
        switch (a) {
            case 'A':
                indexa = 0;
                break;
            case 'C':
                indexa = 1;
                break;
            case 'G':
                indexa = 2;
                break;
            case 'T':
                indexa = 3;
                break;
        }
        switch (b) {
            case 'A':
                indexb = 0;
                break;
            case 'C':
                indexb = 1;
                break;
            case 'G':
                indexb = 2;
                break;
            case 'T':
                indexb = 3;
                break;
        }
        return metrix[indexa][indexb];
    }

    String generateNumber(String input, int num[]) {
        String res = input;
        int l = num.length;
        for (int i = 0; i < num.length; i++) {
            String start = res.substring(0, num[i] + 1);
            String end = res.substring(num[i] + 1, res.length());
            res = start + res + end;
        }
        return res;
    }

    int SequnceAlignmentEfficient(String a, String b) {
        int n = a.length();
        int m = b.length();
        num = new int[m + 1][2];
        int gap = 30;
        for (int i = 0; i <= m; i++) {
            num[i][0] = i * gap;
        }
        for (int j = 1; j <= n; j++) {
            num[0][1] = j * gap;
            for (int i = 1; i <= m; i++) {
                int penalty = getpenaltyvalue(b.charAt(i - 1), a.charAt(j - 1));
                num[i][1] = Math.min(Math.min(penalty + num[i - 1][0], num[i - 1][1] + gap), num[i][0] + gap);
            }
            for (int i = 0; i <= m; i++) {
                num[i][0] = num[i][1];
            }
        }
        return num[m][1];
    }

    String[] divideConquerAlignment(String x, String y) {
        int m = x.length();
        int n = y.length();
        String reversex = new StringBuilder(x).reverse().toString();
        String reversey = new StringBuilder(y).reverse().toString();
        if (m <= 2 || n <= 2) {
            int mincost = SequenceAlignment(x, y);
            return new String[]{String.valueOf(t1), String.valueOf(t2), String.valueOf(mincost)};
        }
        String left = y.substring(0, n / 2);
        String test2 = reversey.substring(0, n / 2);
        int leftcost = SequnceAlignmentEfficient(y.substring(0, n / 2), x);
        int leftopt[][] = num;
        int rightcost = SequnceAlignmentEfficient(reversey.substring(0, n - (n / 2)), reversex);
        int rightopt[][] = num;
        int mincost = Integer.MAX_VALUE / 2;
        int index = -1;
        for (int i = 0; i <= m; i++) {
            int sumCost = rightopt[m - i][0] + leftopt[i][0];
            if (sumCost < mincost) {
                mincost = sumCost;
                index = i;
            }
        }
        String[] charlist1 = divideConquerAlignment(y.substring(0, n / 2), x.substring(0, index));
        String[] charlist2 = divideConquerAlignment(y.substring(n / 2), x.substring(index));
        String[] mergestring = new String[3];
        mergestring[0] = charlist1[0] + charlist2[0];
        mergestring[1] = charlist1[1] + charlist2[1];
        mergestring[2] = String.valueOf(Integer.parseInt(charlist1[2]) + Integer.parseInt(charlist2[2]));
        return mergestring;
    }

    int SequenceAlignment(String a, String b) {
        if (a.equals(b)) {
            t1 = t2 = a.toCharArray();
            return 0;
        }
        int l1 = a.length();
        int l2 = b.length();
        int opt[][] = new int[l1 + 1][l2 + 1];
        int gap = 30;
        for (int i = 1; i <= l1; i++) {
            opt[i][0] = opt[i - 1][0] + gap;
        }
        for (int i = 1; i <= l2; i++) {
            opt[0][i] = opt[0][i - 1] + gap;
        }
        for (int i = 1; i <= l1; i++) {
            for (int j = 1; j <= l2; j++) {
                int flag = 0;
                if (a.charAt(i - 1) != b.charAt(j - 1)) {
                    flag = getpenaltyvalue(a.charAt(i - 1), b.charAt(j - 1));
                }
                opt[i][j] = Math.min(Math.min(opt[i - 1][j - 1] + flag, opt[i - 1][j] + gap), opt[i][j - 1] + gap);
            }
        }
        int cost = opt[l1][l2];
        int indexi = l1;
        int indexj = l2;
        List<Character> arr1 = new ArrayList<Character>();
        List<Character> arr2 = new ArrayList<Character>();
        while (cost != 0) {
            int penalty = -1;
            if (indexi - 1 >= 0 && indexj - 1 >= 0) {
                penalty = getpenaltyvalue(a.charAt(indexi - 1), b.charAt(indexj - 1));
            }
            if (indexi != 0 && indexj != 0) {
                if (cost == opt[indexi - 1][indexj - 1] + penalty) {
                    arr1.add(a.charAt(indexi - 1));
                    arr2.add(b.charAt(indexj - 1));
                    cost = cost - penalty;
                    indexi--;
                    indexj--;
                } else if (cost == opt[indexi][indexj - 1] + gap) {
                    arr1.add('_');
                    arr2.add(b.charAt(indexj - 1));
                    cost = cost - gap;
                    indexj--;
                } else {
                    arr1.add(a.charAt(indexi - 1));
                    arr2.add('_');
                    cost = cost - gap;
                    indexi--;
                }
            } else {
                if (indexi == 0 && indexj != 0) {
                    for (int i = 0; i < indexj; i++) {
                        arr2.add(b.charAt(i));
                        arr1.add('_');
                    }
                    break;
                }
                if (indexi != 0 && indexj == 0) {
                    for (int i = 0; i < indexi; i++) {
                        arr1.add(a.charAt(i));
                        arr2.add('_');
                    }
                    break;
                }
            }
            if (cost == 0 && (indexi != 0 || indexj != 0)) {
                while (indexi - 1 >= 0) {
                    arr1.add(a.charAt(indexi - 1));
                    arr2.add(b.charAt(indexi - 1));
                    indexi--;
                }
            }
        }
        t1 = convertListtoChar(arr1);
        t2 = convertListtoChar(arr2);
        return opt[l1][l2];
    }

    void readFile(String filename) {
        try {
            BufferedReader in = new BufferedReader(new FileReader(filename));
            String str;
            while ((str = in.readLine()) != null) {
                filestrflow.add(str);
            }
        } catch (IOException e) {
        }
    }

    void readData() {
        int length = filestrflow.size();
        int count = 0;
        for (int i = 0; i < length; i++) {
            if (!Character.isDigit(filestrflow.get(i).charAt(0))) {
                if (count == 0) {
                    s1 = filestrflow.get(i);
                    count++;
                } else {
                    s2 = filestrflow.get(i);
                    count++;
                }
            } else {
                if (count == 1) {
                    num1.add(Integer.parseInt(filestrflow.get(i)));
                } else {
                    num2.add(Integer.parseInt(filestrflow.get(i)));
                }
            }
        }

    }

    int[] convertListtoInt(List<Integer> l) {


        int[] ret = new int[l.size()];
        for (int i = 0; i < ret.length; i++) {
            ret[i] = l.get(i).intValue();
        }
        return ret;
    }

    void filewrite(String[] res, String filepath) {
        try {
            BufferedWriter out = new BufferedWriter(new FileWriter(filepath));
            for (int i = 0; i < res.length; i++) {
                out.write(res[i]);
                out.newLine();
            }
            out.close();

        } catch (IOException e) {
        }

    }

    public char[] convertListtoChar(List<Character> l) {
        char[] ret = new char[l.size()];
        for (int i = 0; i < ret.length; i++) {
            ret[l.size() - 1 - i] = l.get(i).charValue();
        }
        return ret;
    }

    private static double getMemoryInKB() {
        double total = Runtime.getRuntime().totalMemory();
        return (total - Runtime.getRuntime().freeMemory()) / 1e3;
    }

    private static double getTimeInMilliseconds() {
        return System.nanoTime() / 1e6;
    }

    public static void main(String args[]) {
        Efficient s = new Efficient();
        s.readFile(args[0]);
        s.readData();
        String res = s.generateNumber(s1, s.convertListtoInt(num1));
        String res2 = s.generateNumber(s2, s.convertListtoInt(num2));
        double beforeUsedMem = getMemoryInKB();
        double startTime = getTimeInMilliseconds();
        String[] a = s.divideConquerAlignment(res, res2);
        double afterUsedMem = getMemoryInKB();
        double endTime = getTimeInMilliseconds();
        double totalUsage = afterUsedMem - beforeUsedMem;
        double totalTime = endTime - startTime;
        String[] output = new String[5];
        output[0] = a[2];
        output[1] = a[0];
        output[2] = a[1];
        output[3] = totalTime + "";
        output[4] = totalUsage + "";
        s.filewrite(output, args[1]);
    }

}