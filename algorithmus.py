import numpy as np
from pymoo.core.problem import ElementwiseProblem

class MyProblem(ElementwiseProblem):

    def __init__(self, n, m):
        self.n = n
        self.m = m
        super().__init__(n_var=m, n_obj=2, n_ieq_constr=0)

    def _evaluate(self, x, out, *args, **kwargs):
        out["F"] = np.array([- self.f(x), - self.g(x)], dtype=float)
    
    def f(self, X):
        return sum([np.sum(x) for x in X])

    def g(self, X):
        return sum([np.count_nonzero(x == 1) for x in X])

from pymoo.core.sampling import Sampling

class MySampling(Sampling):

    def _do(self, problem, n_samples, **kwargs):
        X = [[[np.random.randint(10) for _ in range(np.random.randint(1,5))] for _ in range(problem.m)] for _ in range(n_samples)]
        return X

from pymoo.core.crossover import Crossover

class MyCrossover(Crossover):
    def __init__(self):

        # define the crossover: number of parents and number of offsprings
        super().__init__(2, 2)

    def _do(self, problem, X, **kwargs):
        offspring_n, n_matings, n_var = X.shape
        Y = np.zeros((offspring_n, n_matings, n_var))
        print(X)

        for k in range(n_matings):

            # get the first and the second parent
            a, b = X[0, k], X[1, k]
            # prepare the offsprings
            off_a, off_b = [], []
            for i in range(problem.m):
                if np.random.random() < 0.5:
                    off_a.append(a[i])
                    off_b.append(b[i])
                else:
                    off_a.append(b[i])
                    off_b.append(a[i])
            print(off_a)
            Y[0, k], Y[1, k] = off_a, off_b
        print(Y.shape)
        return Y

from pymoo.core.mutation import Mutation

class MyMutation(Mutation):
    def __init__(self):
        super().__init__()

    def _do(self, problem, X, **kwargs):

        # for each individual
        for i in range(len(X)):

            r = np.random.random()

            if r < 0.1:
                i_1 = np.random.randint(0, problem.m)
                i_2 = np.random.randint(0, len(X[i, i_1]))
                X[i, i_1][i_2] += min(9, X[i, i_1][i_2]+1)

            elif r < 0.2:
                i_1 = np.random.randint(0, problem.m)
                i_2 = np.random.randint(0, len(X[i, i_1]))
                X[i, i_1][i_2] = max(0, X[i, i_1][i_2]-1)
            elif r < 0.3:
                i_1 = np.random.randint(0, problem.m)
                if(len(X[i, i_1]) < problem.n -1):
                   X[i, i_1].append(np.random.randint(10))
            elif r < 0.4:
                i_1 = np.random.randint(0, problem.m)
                if(len(X[i, i_1]) > 0):
                   X[i, i_1].remove(0)

        return X
    
from pymoo.core.duplicate import ElementwiseDuplicateElimination

class MyDuplicateElimination(ElementwiseDuplicateElimination):

    def is_equal(self, a, b):
        for arr1, arr2 in zip(a.X, b.X):
            if not np.array_equal(arr1, arr2):
                return False
        return True

import numpy as np

from pymoo.algorithms.moo.nsga2 import NSGA2
from pymoo.optimize import minimize


algorithm = NSGA2(pop_size=11,
                  sampling=MySampling(),
                  crossover=MyCrossover(),
                  mutation=MyMutation(),
                  eliminate_duplicates=MyDuplicateElimination())

res = minimize(MyProblem(5, 2),
               algorithm,
               ('n_gen', 100),
               seed=1,
               verbose=False)

from pymoo.visualization.scatter import Scatter
Scatter().add(res.F).show()
results = res.X[np.argsort(res.F[:, 0])]
count = [np.sum([e == "a" for e in r]) for r in results[:, 0]]
print(np.column_stack([results, count]))