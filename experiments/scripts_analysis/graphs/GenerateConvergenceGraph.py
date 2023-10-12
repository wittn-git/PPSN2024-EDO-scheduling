import numpy as np
import matplotlib.pyplot as plt
from matplotlib.patches import FancyArrowPatch, Arc
from matplotlib.lines import Line2D
import sys

def f(x):
    return (-11/150) * x**4 + (149/105)* x**3 - (8917/1050) * x ** 2 + (1717/105) * x

def make_plot(outputfile):

    plt.rcParams.update({'font.size': 14})
    fig, axes = plt.subplots(nrows=1, ncols=2, figsize=(12, 5))
    fig.subplots_adjust(wspace=0.5, right=0.9, left= 0.1)

    

    for i, axis in enumerate(axes):
        x = np.linspace(0, 10, 100)
        y = f(x)
        axis.margins(x=0, y = 0)
        axis.plot(x, y, label='Objective Space', color='#B0E0E6')
        axis.set_ylim([0, 15])
        axis.set_xlabel('Feature Value')
        

        if(i == 0):
            x_range = np.linspace(1.2, 1.7, 100)
            y_range = f(x_range)
            axis.plot(x_range, y_range, label='Current Population', color='#7B68EE', linewidth=4)
            axis.arrow(x=1.8+.115, y=f(1.8)+0.155, dx=0.3, dy=f(2.1)-f(1.8), width=.04, color='#BA55D3', label='Selection Pressure', linewidth=1) 
            axis.arrow(x=1.2 - 0.2, y=f(1.2), dx=-0.3, dy=f(0.9)-f(1.2), width=.04, color='#BA55D3', label='Selection Pressure', linewidth=1) 
            axis.set_ylabel('Fitness Value')
        elif(i == 1):
            x_range = np.linspace(0.85, 2.1 , 100)
            y_range = f(x_range)
            axis.plot(x_range, y_range, label='Current Population', color='#7B68EE', linewidth=4)
            axis.arrow(x=1.8+.2+0.3, y=f(1.8+.06+0.3)+0.155, dx = -0.32, dy = 1, width=.04, color='#BA55D3', label='Selection Pressure', linewidth=1) 
            axis.arrow(x=1.2-0.55, y=f(0.9)-0.2, dx=0.27, dy=f(1.2)-f(0.9), width=.04, color='#BA55D3', label='Selection Pressure', linewidth=1)   
            axis.set_yticks([])
            axis_2 = axis.twinx()
            axis_2.set_ylim([0, 15])
            axis_2.set_ylabel('Fitness Value')        

        legend_entry = Line2D([0, 0], [0, 0], linestyle='-', marker='>', color='#BA55D3', markersize=10)
        handles, labels = axis.get_legend_handles_labels()
        handles = handles[:-2]
        handles.append(legend_entry)
        axis.legend(handles=handles, labels=labels, loc="upper left") 


    arc_1 = Arc(
        (580, 300),         
        120, 40,    
        theta1=0,    
        theta2=140,   
        angle=20,
        linewidth=2,  
        color='black',
        alpha=0.6
    )

    arrow_1 = FancyArrowPatch(
        (645, 313),    
        (650, 308),   
        arrowstyle='->',
        mutation_scale=20,  
        color="black",
        linewidth=2,
        alpha=0.6
    )

    arc_2 = Arc(
        (620, 270),         
        120, 40,    
        theta1=0,    
        theta2=140,   
        angle=200,
        linewidth=2,  
        color='black',
        alpha=0.6
    )

    arrow_2 = FancyArrowPatch(
        (555, 256),    
        (550, 261),   
        arrowstyle='->',
        mutation_scale=20,  
        color="black",
        linewidth=2,
        alpha=0.6
    )

    fig.patches.append(arrow_1)
    fig.patches.append(arrow_2)
    fig.patches.append(arc_1)
    fig.patches.append(arc_2)

    #plt.tight_layout()
    plt.savefig(outputfile)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python3 GenerateConvergenceGraph.py <output_file>")
        exit(1)
    
    output_file = sys.argv[1]
    make_plot(output_file)