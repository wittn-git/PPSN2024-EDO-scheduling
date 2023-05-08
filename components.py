import numpy as np
from schedule import Schedule
from pymoo.core.problem import ElementwiseProblem
from pymoo.core.sampling import Sampling
from pymoo.core.crossover import Crossover
from pymoo.core.mutation import Mutation
from pymoo.core.duplicate import ElementwiseDuplicateElimination

class ParallelMachineScheduling(ElementwiseProblem):

    def __init__(self, n, m):
        self.n = n
        self.m = m
        vars = {
            "M": Schedule
        }
        super().__init__(vars=vars, n_obj=2)

    def _evaluate(self, x, out, *args, **kwargs):
        out["F"] = np.array([- self.f(x), - self.g(x)], dtype=float)
    
    def f(self, X):
        return sum([np.sum(x) for x in X[0]])

    def g(self, X):
        return sum([x.count(1) for x in X[0]])

class RandomScheduleSampling(Sampling):

    def _do(self, problem, n_samples, **kwargs):
        X = [[Schedule([[np.random.randint(0, 5) for _ in range(problem.n)] for _ in range(problem.m)])] for _ in range(n_samples)]
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

class NumericMutation(Mutation):
    def __init__(self):
        super().__init__()

    def _do(self, problem, X, **kwargs):
        for i in range(len(X)):
            r = np.random.random()

            if r < 0.1:
                i_1 = np.random.randint(problem.m)
                i_2 = np.random.randint(len(X[i, 0][i_1]))
                X[i, 0][i_1][i_2] = min(9, X[i, 0][i_1][i_2]+1)

            elif r < 0.2:
                i_1 = np.random.randint(problem.m)
                i_2 = np.random.randint(len(X[i, 0][i_1]))
                X[i, 0][i_1][i_2] = max(0, X[i, 0][i_1][i_2]-1)


        return X
    
class EqualListElimination(ElementwiseDuplicateElimination):

    def is_equal(self, a, b):
        A, B = a.X[0].sort(), b.X[0].sort()
        for arr1, arr2 in zip(A, B):
            if not np.array_equal(arr1, arr2):
                return False
        return True