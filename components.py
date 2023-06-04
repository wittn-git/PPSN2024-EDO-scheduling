import numpy as np
from schedule import Schedule
from pymoo.core.sampling import Sampling
from pymoo.core.crossover import Crossover
from pymoo.core.mutation import Mutation
from pymoo.core.duplicate import ElementwiseDuplicateElimination

class RandomScheduleSampling(Sampling):

    def _do(self, problem, n_samples, **kwargs):
        X = []
        for _ in range(n_samples):
            jobs = np.arange(problem.n_jobs)
            x = [[] for _ in range(problem.n_machines)]
            while jobs.any():
                job = np.random.choice(jobs)
                x[np.random.randint(problem.n_machines)].append(job)
                jobs = jobs[jobs != job]
            X.append(Schedule(x))
        return np.array(X)

class ListExchangeCrossover(Crossover):
    def __init__(self):
        super().__init__(2, 2)

    def _do(self, problem, X, **kwargs):
        n_offsprings, n_matings, n_var = X.shape
        Y = np.zeros((n_offsprings, n_matings, 1), dtype=object)
        for k in range(n_matings):
            a, b = X[0, k, 0], X[1, k, 0]
            off_a, off_b = [], []
            for i in range(problem.m):
                if np.random.random() < 0.5:
                    off_a.append(a[i])
                    off_b.append(b[i])
                else:
                    off_a.append(b[i])
                    off_b.append(a[i])
            Y[0, k], Y[1, k] = [Schedule(off_a)], [Schedule(off_b)]
        return Y

class SwapAndTransferMutation(Mutation):
    def __init__(self):
        super().__init__()

    def _do(self, problem, X, **kwargs):
        for i in range(len(X)):
            r = np.random.random()

            if r < 0.1:
                i_1 = np.random.randint(problem.m)
                j_1 = np.random.randint(len(X[i, 0][i_1])-1)
                j_2 = np.random.randint(j_1, len(X[i, 0][i_1]))
                X[i, 0][i_1][j_1], X[i, 0][i_1][j_2] = X[i, 0][i_1][j_2], X[i, 0][i_1][j_1]

            elif r < 0.2:
                i_1 = np.random.randint(problem.m)
                i_2 = np.random.randint(problem.m)
                j_1 = np.random.randint(len(X[i, 0][i_1]))
                j_2 = np.random.randint(len(X[i, 0][i_2]))
                job = X[i, 0][i_1][j_1]
                del X[i, 0][i_1][j_1]
                X[i, 0][i_2].insert(j_2, job)
                
        return X
    
class EqualListElimination(ElementwiseDuplicateElimination):

    def is_equal(self, a, b):
        print(a.X[0])
        A, B = a.X[0].sort(), b.X[0].sort()
        for arr1, arr2 in zip(A, B):
            if not np.array_equal(arr1, arr2):
                return False
        return True