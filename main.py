import numpy as np

from components import *
from pymoo.visualization.scatter import Scatter
from pymoo.algorithms.moo.nsga2 import NSGA2
from pymoo.optimize import minimize

if __name__ == '__main__':

    algorithm = NSGA2(pop_size=30,
                    sampling=RandomScheduleSampling(),
                    crossover=ListExchangeCrossover(),
                    mutation=NumericMutation(),
                    eliminate_duplicates=EqualListElimination())

    res = minimize(ParallelMachineScheduling(3, 3),
                algorithm,
                ('n_gen', 30),
                seed=1,
                verbose=False)

    Scatter().add(res.F).show()
    S = [x[0].schedule for x in res.X]
    print(res.F)
    for s in S:
        print(s)