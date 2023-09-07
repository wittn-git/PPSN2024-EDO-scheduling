import numpy as np
import random
import matplotlib.pyplot as plt
import sys

def generate_artifical_vector(mu, count, balance_level, max_val):
    vec = []
    entries_n = int(0.5 * (mu**2 - mu))
    if count == 0: return 1
    if balance_level == 0: 
        vec = []
        acc = 0
        while(acc < count):
            k = min(count-acc, max_val)
            vec.append(k)
            acc += k

    if balance_level == 1: 
        fill_value_1 = int((count * 0.8) / (entries_n*0.2))
        if fill_value_1 == 0:
            while(sum(vec) < count * 0.8):
                vec.append(1)
        else:
            if fill_value_1 > max_val: fill_value_1 = max_val
            while(sum(vec) + fill_value_1 < count * 0.8):
                vec.append(fill_value_1)
            vec.append(count * 0.8 - sum(vec))
        fill_value_2 = int((count * 0.2) / (entries_n-len(vec)))
        if fill_value_2 == 0:
            while(sum(vec) < count):
                vec.append(1)
        else:
            if fill_value_2 > max_val: fill_value_2 = max_val
            while(sum(vec) + fill_value_2 < count):
                vec.append(fill_value_2)
            vec.append(count - sum(vec))
        while(len(vec) < entries_n):
            vec.append(0)

    if balance_level == 2:
        acc = 0
        for i in range(entries_n - 1):
            k = min(random.randint(max(0, int(count / entries_n - 3)), int(count / entries_n + 3)), max_val)
            if acc + k > count: 
                vec.append(count - acc)
                acc = count
                break
            acc += k
            vec.append(k)
        while(len(vec) < entries_n):
            vec.append(0)
        index = 0
        while(acc < count):
            if vec[index] < max_val:
                vec[index] += 1
                acc += 1
            index = (index + 1) % entries_n      

    if balance_level == 3:
        main_entry = int(count // entries_n)
        remaining_count = int(count % entries_n)
        vec = [main_entry + 1] * remaining_count + [main_entry] * (entries_n - remaining_count)

    return 1 - np.linalg.norm(vec) / np.linalg.norm([max_val] * entries_n)

def make_plot(mu, n, output_folder):
    maximum_DFs = (mu**2-mu)/2 * ( n - 1)
    X = np.arange(0, maximum_DFs+1)
    plt.figure()
    
    cmap = plt.get_cmap("tab10")
    colors = [
        cmap.colors[4],
        cmap.colors[1],
        cmap.colors[2],
        cmap.colors[3],
    ]

    for i, color_ in enumerate(colors):
        Y = [generate_artifical_vector(mu, x, i, n-1) for x in X]
        print(Y[100])
        plt.plot(X, Y, label='Balance-Level {}'.format(i), color=color_)

    plt.xlabel('Number of Duplicate Directly-Follows Relations')
    plt.ylabel('Population Diversity ')
    plt.legend()

    plt.savefig(output_folder + '/DiversityDeterminationGraph_mu{}_n{}.png'.format(mu, n))

if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Usage: python3 GenerateDeterminationGraph.py <output_folder> <mu> <n>")
        exit(1)
    
    output_folder = sys.argv[1]
    mu = int(sys.argv[2])
    n = int(sys.argv[3])

    make_plot(mu, n, output_folder)