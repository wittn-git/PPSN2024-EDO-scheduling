from problem import *
from components import *
from pymoo.visualization.scatter import Scatter
from pymoo.algorithms.moo.nsga2 import NSGA2
from pymoo.optimize import minimize

if __name__ == '__main__':

    algorithm = NSGA2(
                    pop_size=25,
                    sampling=RandomScheduleSampling(),
                    crossover=ListExchangeCrossover(),
                    mutation=SwapAndTransferMutation(),
                    eliminate_duplicates=EqualListElimination()
    )

    problem = create_random_parallel_problem(3, 12, 1)

    res = minimize(
                problem,
                algorithm,
                ('n_gen', 100),
                seed=1,
                verbose=False
    )
    
    Scatter().add(res.F).show()
    print(res.F)
    for x in res.opt.get("X"):
        print(x[0])