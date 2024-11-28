import subprocess
import re
import csv
import os
from tqdm import tqdm

def get_excution_times(threads, iterations):
    '''
    Runs the runner.py script to capture the execution time and check if expected values are the same with the calculated values

    Args:
        threads : number of threads
        iterations : number of for loop iterations isnide the thread function

    Returns:
        The execution time or None if build failed or the expected values did not match the actual values
    '''

    # running runner.py and catching the output
    result = subprocess.run(['python3', 'runner.py', str(threads), str(iterations)], capture_output=True, text=True)
    output = result.stdout

    # comparing actual with expected values
    expected_val = re.search(r"Expected value of common variable:\s*([\d.]+)", output)
    actual_val = re.search(r"Actual value of common variable:\s*([\d.]+)", output)
    if int(actual_val.group(1)) != int(expected_val.group(1)):
        return None

    # returning the execution time
    match = re.search(r"Elapsed time:\s*([\d.]+)", output)
    if match:
        return float(match.group(1))
    else:
        return None

if __name__ == "__main__":

    print('************************************************************************')
    
    print("Running results generation script for Exercise 1.2 - Atomic Operations")

    ###############################################
    # Parameters
    ###############################################

    runs = 10   # number of runs for each execution
    threads = [1, 2, 4, 8, 16, 32] # number of threads
    max_iterations = 6 # maximum exponent of iterations of for loops of C code threads (it is decimal exponent)

    ###############################################
    # Collect results
    ###############################################

    timing_results = []
    efficiency_results = []

    for i in range(1, max_iterations + 1, 1):
        iterations = 10 ** i

        thread_times = []
        thread_efficiencies = []
        for thread in threads:

            run_times = []
            for j in tqdm(range(runs), desc=f'{thread} threads, {iterations} iterations'):
                run_times.append(get_excution_times(thread, iterations))
            run_time = sum(run_times) / len(run_times)

            thread_times.append(run_time)
            thread_efficiencies.append(thread_times[0] / (thread * run_time))
        
        timing_results.append([iterations] + thread_times)
        efficiency_results.append([iterations] + thread_efficiencies)

    ###############################################
    # Update csv
    ###############################################

    # checking that result directory and .csv exist - else we create them
    result_dir = "results"
    if not os.path.exists(result_dir):
        os.makedirs(result_dir)
    timings_csv = os.path.join(result_dir, "timings.csv")
    efficiencies_csv = os.path.join(result_dir, "efficiencies.csv")

    # filling in the execution times in timings.csv
    with open(timings_csv, "w", newline="") as csvfile:
        csvwriter = csv.writer(csvfile)

        thread_header = []
        for thread in threads:
            thread_header.append(f'{thread} Threads')
        csvwriter.writerow(['Iterations'] + thread_header)

        for timing_result in timing_results:
            csvwriter.writerow(timing_result)

    # filling in the efficiencies in efficiencies.csv
    with open(efficiencies_csv, "w", newline="") as csvfile:
        csvwriter = csv.writer(csvfile)

        thread_header = []
        for thread in threads:
            thread_header.append(f'{thread} Threads')
        csvwriter.writerow(['Iterations'] + thread_header)

        for efficiency_result in efficiency_results:
            csvwriter.writerow(efficiency_result)

    print('************************************************************************')