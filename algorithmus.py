import numpy as np
from pymoo.core.problem import ElementwiseProblem

class MyProblem(ElementwiseProblem):

    def __init__(self, n, m):
        self.n = n
        self.m = m
        vars = {
            "M": Schedule
        }
        super().__init__(n_var=1, n_obj=2)

    def _evaluate(self, x, out, *args, **kwargs):
        out["F"] = np.array([- self.f(x), - self.g(x)], dtype=float)
    
    def f(self, X):
        return sum([np.sum(x) for x in X[0]])

    def g(self, X):
        return sum([np.count_nonzero(x == 1) for x in X[0]])

class Schedule:

    def __init__(self, schedule):
        self.schedule = schedule
    
    def __iter__(self):
        for s in self.schedule:
            yield s
    
    def __getitem__(self, item):
         return self.schedule[item]

from pymoo.core.sampling import Sampling

class MySampling(Sampling):

    def _do(self, problem, n_samples, **kwargs):
        X = [[Schedule([[np.random.randint(0, 5) for _ in range(np.random.randint(1, problem.n))] for _ in range(problem.m)])] for _ in range(n_samples)]
        return np.array(X)

from pymoo.core.crossover import Crossover

class MyCrossover(Crossover):
    def __init__(self):

        # define the crossover: number of parents and number of offsprings
        super().__init__(2, 2)

    def _do(self, problem, X, **kwargs):
        n_offsprings, n_matings, n_var = X.shape

        Y = np.zeros((n_offsprings, n_matings, 1), dtype=object)
        for k in range(n_matings):

            # get the first and the second parent
            a, b = X[0, k, 0], X[1, k, 0]
            # prepare the offsprings
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

from pymoo.core.mutation import Mutation

class MyMutation(Mutation):
    def __init__(self):
        super().__init__()

    def _do(self, problem, X, **kwargs):
        # for each individual
        for i in range(len(X)):

            r = np.random.random()
            if r < 0.1:
                i_1 = np.random.randint(problem.m)
                if len(X[i, 0][i_1]) == 0:
                    continue
                i_2 = np.random.randint(len(X[i, 0][i_1]))
                X[i, 0][i_1][i_2] += min(9, X[i, 0][i_1][i_2]+1)

            elif r < 0.2:
                i_1 = np.random.randint(problem.m)
                if len(X[i, 0][i_1]) == 0:
                    continue
                i_2 = np.random.randint(len(X[i, 0][i_1]))
                X[i, 0][i_1][i_2] = max(0, X[i, 0][i_1][i_2]-1)

            elif r < 0.3:
                i_1 = np.random.randint(problem.m)
                if(len(X[i, 0][i_1]) < problem.n -1):
                   X[i, 0][i_1].append(np.random.randint(10))

            elif r < 0.4:
                i_1 = np.random.randint(problem.m)
                if(len(X[i, 0][i_1]) > 0):
                   del X[i, 0][i_1][0]

        return X
    
from pymoo.core.duplicate import ElementwiseDuplicateElimination

class MyDuplicateElimination(ElementwiseDuplicateElimination):

    def is_equal(self, a, b):
        for arr1, arr2 in zip(a.X[0], b.X[0]):
            if not np.array_equal(arr1, arr2):
                return False
        return False

import numpy as np

from pymoo.algorithms.moo.nsga2 import NSGA2
from pymoo.optimize import minimize


algorithm = NSGA2(pop_size=200,
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
#Scatter().add(res.F).show()
S = [x[0].schedule for x in res.X]
for s in S:
    print(s)