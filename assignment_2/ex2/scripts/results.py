import argparse
import os
import csv
import subprocess
import re
from tqdm import tqdm # type: ignore
import sys

if sys.version_info < (3, 9):
    from typing import Tuple
    TupleToUse = Tuple
else:
    TupleToUse = tuple


################################################
# Function definitions
################################################

def create_result_directories(cleanup=False):
    '''
    A function to create the directory structure of the results.
    If the files or the directories exist it does not create them again.

    Args:
    - cleanup : if True it cleans the files

    Structure:

    ```
    ../results
            ├── columns
            │   ├── efficiency__<schedule>.csv
            │   ├── ...
            │   └── speedup__<schedule>.csv
            └── rows
                ├── efficiency__<schedule>.csv
                ├── ...
                └── speedup__<schedule>.csv
    ```
    '''

    result_dir = "../results"
    if not os.path.exists(result_dir):
        os.makedirs(result_dir)

    result_rows = os.path.join(result_dir, 'rows')
    if not os.path.exists(result_rows):
        os.makedirs(result_rows)

    result_columns = os.path.join(result_dir, 'columns')
    if not os.path.exists(result_columns):
        os.makedirs(result_columns)

    schedules = [
        'static_default', 'static_1', 'static_maxchunk',
        'dynamic_default', 'dynamic_1', 'dynamic_maxchunk',
        'guided_default', 'guided_1', 'guided_maxchunk'
        ]
    
    metrics = ['speedup', 'efficiency']

    for schedule in schedules:
        for metric in metrics:
            file = metric + '__' + schedule + '.csv'

            rowfile = os.path.join(result_rows, file)
            if not os.path.exists(rowfile):
                with open(rowfile, 'w') as newfile:
                    newfile.close()
            else:
                if cleanup == True:
                    with open(rowfile, 'w') as cleanfile:
                        cleanfile.close()

            columnfile = os.path.join(result_columns, file)
            if not os.path.exists(columnfile):
                with open(columnfile, 'w') as newfile:
                    newfile.close()
            else:
                if cleanup == True:
                    with open(columnfile, 'w') as cleanfile:
                        cleanfile.close()


def get_results(
        threadcount : int,
        iterations : int,
        method : str,
        schedule : str
) -> TupleToUse[float, float]:
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

    command = [
        'python3',
        'runner.py',
        str(threadcount),
        str(iterations),
        method, 
        'parallel',
        '-s',
        schedule
    ]

    result = subprocess.run(command, capture_output=True, text=True)
    output = result.stdout

    matchreler = re.search(r"Relative error to sequential method:\s*([\d.]+)", output)
    if matchreler:
        if float(matchreler.group(1)) == 0:
            match = re.search(r"Speedup:\s*([\d.]+)", output)
            if match:
                speedup = float(match.group(1))
                efficiency = speedup / threadcount
                return [speedup, efficiency]
            else:
                return [None, None]
        else:
            return [None, None]
    else:
        return [None, None]
    

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
parser.add_argument('-c', '--clean',
                    action='store_true',
                    help='Cleans up result files')

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

    iterations_start = 2500
    threadcount = [1, 2, 3, 4, 8]
    chunks = ['default', '1', 'maxchunk']

    #****************#
    # Logic
    #****************#

    create_result_directories(args.clean)

    for method in methods:
        method_dir = os.path.join('..', 'results', method)
        ef_file = os.path.join(method_dir, 'efficiency')
        sp_file = os.path.join(method_dir, 'speedup')

        for schedule in schedules:
            for chunk in chunks:

                ef_csv = ef_file + '__' + schedule + '_' + chunk + '.csv'
                if not os.path.exists(ef_csv):
                    raise FileNotFoundError(f"csv file: {ef_csv} not found")
                sp_csv = sp_file + '__' + schedule + '_' + chunk + '.csv'
                if not os.path.exists(sp_csv):
                    raise FileNotFoundError(f"csv file: {sp_csv} not found")

                ef_header = ['Iterations']
                sp_header = ['Iterations']
                for threads in threadcount:
                    ef_header.append(f'{threads}_threads')
                    sp_header.append(f'{threads}_threads')

                iterations = iterations_start
                for i in range(5):

                    ef_result = [iterations]
                    sp_result = [iterations]

                    # for threads in threadcount:
                    for threads in tqdm(threadcount, desc=f'Method: {method}, File: {ef_csv}, Iterations: {iterations}'):
                        if chunk == 'default':
                            [speedup, efficiency] = get_results(threads, iterations, method, f'\"{schedule}\"')
                        elif chunk == '1':
                            [speedup, efficiency] = get_results(threads, iterations, method, f'\"{schedule},1\"')
                        elif chunk == 'maxchunk':
                            [speedup, efficiency] = get_results(threads, iterations, method, f'\"{schedule},{iterations // threads}\"')
                        else:
                            raise ValueError('Wrong chunk value')
                        
                        sp_result.append(speedup)
                        ef_result.append(efficiency)
                        
                    with open(ef_csv, 'a', newline='') as csvfile:
                        writer = csv.writer(csvfile)
                        if not os.path.getsize(ef_csv) > 0:
                            writer.writerow(ef_header)
                        writer.writerow(ef_result)
                        csvfile.close()
                
                    with open(sp_csv, 'a', newline='') as csvfile:
                        writer = csv.writer(csvfile)
                        if not os.path.getsize(sp_csv) > 0:
                            writer.writerow(sp_header)
                        writer.writerow(sp_result)
                        csvfile.close()

                    iterations *= 2
