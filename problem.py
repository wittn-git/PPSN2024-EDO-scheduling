import matplotlib.pyplot as plt
import numpy as np
from schedule import Schedule
from pymoo.core.problem import ElementwiseProblem  

class ParallelMachineScheduling(ElementwiseProblem):

    def __init__(self, n_machines, processing_times, release_dates, due_dates):
        """
        Parallel scheduling problem.

        Parameters
        ----------
        n_machines : int
            Integer representing the number of machines.
        processing_times : numpy.array
            Array where processing_time[i] is the processing time for job i.
        release_dates : numpy.array
            Array where release_dates[i] is the release date for job i.
        due_dates : numpy.array 
            Array where due_dates[i] is the due date for job i.
        """
        self.n_jobs = processing_times.shape[0]
        self.n_machines = n_machines
        self.processing_times = processing_times
        self.release_dates = release_dates
        self.due_dates = due_dates
        vars = {
            "S": Schedule
        }
        super().__init__(vars=vars, n_obj=2)

    def _evaluate(self, x, out, *args, **kwargs):
        tardiness = self.get_tardiness(x)
        intime_jobs = self.get_intime_jobs(x, tardiness)
        out["F"] = np.array([ tardiness, - intime_jobs], dtype=float)
    
    def get_starting_times(self, X):
        starting_times = np.zeros(self.n_jobs)
        for x in X:
            starting_times[x[0]] = np.maximum(0, self.release_dates[x[0]])
            for i in range(1, len(x)):
                starting_times[x[i]] = np.maximum(starting_times[x[i-1]] + self.processing_times[x[i-1]], self.release_dates[x[i]])
        return starting_times

    def get_completion_times(self, X, starting_times=None):
        if(starting_times is None):
            starting_times = self.get_starting_times(X)
        return starting_times + self.processing_times

    def get_tardiness(self, X, completion_times=None):
        if(completion_times is None):
            completion_times = self.get_completion_times(X)
        return np.maximum(0, completion_times-self.due_dates)

    def get_intime_jobs(self, X, tardiness=None):
        if(tardiness is None):
            tardiness = self.get_tardiness(X)
        return (tardiness <= 0).astype(int)

def create_random_parallel_problem(n_machines, n_jobs, seed=None):
    if seed is not None: np.random.seed(seed)
    processing_times = np.random.randint(5, 26, size=n_jobs)
    due_dates = np.random.randint(processing_times, 2 * processing_times + 1, size=n_jobs)
    release_dates = np.random.randint(0, due_dates - processing_times + 1, size=n_jobs)
    return ParallelMachineScheduling(n_machines, processing_times, release_dates, due_dates)

def visualize(problem, X, filename=None):

    fig, ax = plt.subplots()

    starting_times = problem.get_starting_times(X)
    completion_times = problem.get_completion_times(X)

    ax.set_xlim([0, np.max(completion_times)])
    ax.set_ylim([0, problem.n_machines+1])

    plt.yticks(np.arange(0, problem.n_machines+1, 1))

    # Set labels and title
    ax.set_xlabel('Time')
    ax.set_ylabel('Machine')
    ax.set_title('Job scheduling')

    # Add jobs to plot
    for j in range(problem.n_machines):
        for v in X[j]:
            ax.add_patch(plt.Rectangle((starting_times[v], j+0.6),problem.processing_times[v], 0.8, color='b', alpha=0.5))
            ax.text(starting_times[v]+problem.processing_times[v]/2, j+0.5, f"Job {v}", ha='center', va='center')
    
    if filename is not None:
        plt.savefig(filename)
    else: plt.show()