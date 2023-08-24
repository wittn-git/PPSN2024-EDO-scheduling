mus = [2,10,25,50]
ns = [5,10,25,50,100]
ms = [1,3,5,10]
alphas = [0.2,0.5,1]

runs = 30

times = {
    (2,5): 50,
    (2,10): 50,
    (2,25): 50,
    (2,50): 75,
    (2,100): 175,
    (10,5): 200,
    (10, 10): 200,
    (10, 25): 350,
    (10, 50): 900,
    (10, 100): 3000,
    (25, 25): 2200,
    (25, 50): 6000,
    (25, 100): 17000,
    (50, 50): 30000,
    (50, 100): 70000
}

def iterate_combinations(with_alphas, with_nswap):
    count = 0
    for mu in mus:
        for n in ns:
            for m in ms:
                if(m >= n or (mu > (n*n - n)/(n-m))): continue
                if(with_nswap and m > 1): continue
                current_count = runs * times[(mu,n)] * mu * n * n
                if(with_alphas): current_count *= 3
                count += current_count
    return count

combinations = {
    (False, False): iterate_combinations(False, False) * 8,
    (False, True): iterate_combinations(False, True) * 2,
    (True, False): iterate_combinations(True, False) * 4,
    (True, True): iterate_combinations(True, True) * 1
}

def to_days(nanoseconds):
    return nanoseconds / 1000 / 1000 / 60 / 60 / 7 / 24

total_time = to_days(sum(combinations.values()))
print(f"Total time: {total_time} days")

print("Time per combination:")
for key in combinations.keys():
    time = to_days(iterate_combinations(key[0], key[1]))
    print(f"NSWAP = {key[1]}, alphas = {key[0]}: {time} days")