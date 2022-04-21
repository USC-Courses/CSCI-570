import os
import re
import sys
import numpy as np
import matplotlib.pyplot as plt

data_dir_path = 'data/datapoints/out'
img_dir_path = 'plot/img'


def get_repo_dir():
    real_path = os.path.realpath(sys.argv[0])
    repo_dir = os.path.join(real_path, "../../")
    repo_dir = os.path.realpath(repo_dir)
    return repo_dir


def get_data_dir():
    repo_dir = get_repo_dir()
    data_dir = os.path.join(repo_dir, data_dir_path)
    data_dir = os.path.realpath(data_dir)
    if not os.path.exists(data_dir):
        raise IOError("path not found: " + data_dir)
    return data_dir


def get_img_dir():
    repo_dir = get_repo_dir()
    img_dir = os.path.join(repo_dir, img_dir_path)
    img_dir = os.path.realpath(img_dir)
    if not os.path.exists(img_dir):
        os.makedirs(img_dir)
    return img_dir


def extract_info(filenames):
    data_dir = get_data_dir()
    datapoints = []
    for filename in filenames:
        with open(data_dir + "/" + filename, 'r') as f:
            lines = f.read().splitlines()
            alignment1 = lines[1]
            alignment2 = lines[2]
            n = len(alignment1.replace("_", ""))
            m = len(alignment2.replace("_", ""))
            problem_size = n + m
            time_elapsed = float(lines[3])
            memory_used = float(lines[4])
            datapoints.append([problem_size, time_elapsed, memory_used])
    datapoints.sort(key=lambda item: item[0])
    return datapoints


def get_plot_data():
    data_dir = get_data_dir()
    plot_data = {}
    languages = ["cpp", "java", "python"]
    solutions = ["basic", "efficient"]
    for language in languages:
        plot_data[language] = {}
        for solution in solutions:
            prefix = language + "_" + solution
            pattern = prefix + r"\d+\.out"
            filenames = list(filter(re.compile(pattern).match, os.listdir(data_dir)))
            filenames.sort()
            datapoints = extract_info(filenames)
            plot_data[language][solution] = datapoints
    return plot_data


def draw_line(name, x, y, label):
    plt.figure(name)
    plt.plot(x, y, label=label, lw=1.5, marker="x", ms=1, mew=5)


def save_figure(name, title, x_label, y_label, y_scale):
    plt.figure(name)
    y_min, y_max = plt.ylim()
    plt.title(title, fontdict={'size': 14, 'weight': 'bold'})
    plt.xlabel(x_label, fontdict={'size': 12})
    plt.ylabel(y_label, fontdict={'size': 12})
    plt.grid(True, linestyle='--', alpha=0.5)
    plt.ylim(y_min, y_max * y_scale)
    plt.legend(loc='upper right', fontsize=10)
    plt.subplots_adjust(left=0.15, right=0.95)
    plt.savefig(get_img_dir() + "/" + name + ".png")


def plot_and_save(plot_data):
    time_figure = "compare_time"
    basic_time_figure = "compare_basic_time"
    efficient_time_figure = "compare_efficient_time"
    mem_figure = "compare_memory"
    basic_mem_figure = "compare_basic_memory"
    efficient_mem_figure = "compare_efficient_memory"
    time_title = "CPU Time vs problem size(%s)"
    mem_title = "Memory usage vs problem size(%s)"
    x_label = "Problem Size(m+n)"
    time_y_label = "Time Elapsed(ms)"
    mem_y_label = "Memory Used(kb)"
    y_scale = [1.2, 1.3, 1.7]
    for language, sub_dict in plot_data.items():
        language_time_figure = language + "_time"
        plt.figure(language_time_figure)
        # plt.axes().set_prop_cycle('color', colors)
        language_mem_figure = language + "_mem"
        plt.figure(language_mem_figure)
        # plt.axes().set_prop_cycle('color', colors)
        for solution, datapoints in sub_dict.items():
            label = language + " " + solution
            problem_size = [item[0] for item in datapoints]
            time_elapsed = [item[1] for item in datapoints]
            memory_used = [item[2] for item in datapoints]
            draw_line(language_time_figure, problem_size, time_elapsed, solution)
            draw_line(language_mem_figure, problem_size, memory_used, solution)
            draw_line(time_figure, problem_size, time_elapsed, label)
            draw_line("compare_%s_time" % solution, problem_size, time_elapsed, label)
            draw_line(mem_figure, problem_size, memory_used, label)
            draw_line("compare_%s_memory" % solution, problem_size, memory_used, label)
        save_figure(language_time_figure, time_title % language, x_label, time_y_label, y_scale[0])
        save_figure(language_mem_figure, mem_title % language, x_label, mem_y_label, y_scale[0])
    save_figure(time_figure, time_title % "all", x_label, time_y_label, y_scale[2])
    save_figure(basic_time_figure, time_title % "basic", x_label, time_y_label, y_scale[1])
    save_figure(efficient_time_figure, time_title % "efficient", x_label, time_y_label, y_scale[1])
    save_figure(mem_figure, mem_title % "all", x_label, mem_y_label, y_scale[2])
    save_figure(basic_mem_figure, mem_title % "basic", x_label, mem_y_label, y_scale[1])
    save_figure(efficient_mem_figure, mem_title % "efficient", x_label, mem_y_label, y_scale[1])
    plt.show()


if __name__ == '__main__':
    plot_data = get_plot_data()
    plot_and_save(plot_data)
