import argparse
from tqdm import tqdm
import subprocess
import os


################################################
# Function definitions
################################################

def get_results(
        threadcount : int,
        iterations : int,
        method : str,
        schedule : str
) -> tuple[float, float]:
    '''
    Calls the python runner.py script that builds and runs the program.
    It returns a tuple with the speedup and the efficiency.

    Args:
    - threadcount (int): The number of threads
    - iterations (int): test test
    - method (str): test test
    - schedule (str): test test

    Returns:
    - tuple with first element the speedup and second the efficiency as floats
    '''

################################################
# Parser
################################################

parser = argparse.ArgumentParser()
parser.add_argument('-s', '--schedule',
                    choices=['static', 'dynamic', 'guided', 'all'], default='all', 
                    help='Choose OpenMP scheduling. Default is to produce results for all schedules')
parser.add_argument('-m', '--method',
                    choices=['rows', 'columns', 'all'], default='all', 
                    help='Choose method of parallel program. Default is to run for all methods')

################################################
# Main
################################################

if __name__ == "__main__":


    args = parser.parse_args()

    #****************#
    # Iterators
    #****************#

    if args.schedule == 'all':
        schedules = ['static', 'dynamic', 'guided']
    else:
        schedules = [args.schedule]

        
    if args.method == 'all':
        methods = ['rows', 'columns']
    else:
        methods = [args.method]


    iterations = 2500
    threadcount = [1, 2, 3, 4, 8]

    #****************#
    # Logic
    #****************#

    for method in methods:
        for threads in threadcount:
            for i in range(5):
                maxchunk = iterations // threadcount # not sure if correct for prime cases (3 especially)
                for schedule in schedules:
                    print()
                iterations *= 2



