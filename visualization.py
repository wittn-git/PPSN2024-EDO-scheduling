from components import RandomScheduleSampling
import problem as prob

def directly_follows_measure(X, Y):
    score = 0
    for x in X:
        for y in Y:
            for i in range(len(x)-1):
                for j in range(len(y)-1):
                    if x[i] == y[j] and x[i+1] == y[j+1]:
                        score += 1
    return score

if __name__ == '__main__':

    problem = prob.create_random_parallel_problem(3, 10, 0)
    sampler = RandomScheduleSampling()
    samples = sampler.do(problem, 10)

    low_score, low_items = None, None
    high_score, high_items = None, None
    for i, s1 in enumerate(samples):
        for s2 in samples[i+1:]:
            score = directly_follows_measure(s1.X, s2.X)
            if low_score is None or score < low_score:
                low_score = score
                low_items = (s1, s2)
            if high_score is None or score > high_score:
                high_score = score
                high_items = (s1, s2)
    
    print('Lowest score:', low_score)
    print('Lowest items:', low_items[0].X, low_items[1].X)
    print('Highest score:', high_score)
    print('Highest items:', high_items[0].X, high_items[1].X)

    prob.visualize(problem, low_items[0].X, "low1.png")
    prob.visualize(problem, low_items[1].X, "low2.png")
    prob.visualize(problem, high_items[0].X, "high1.png")
    prob.visualize(problem, high_items[1].X, "high2.png")