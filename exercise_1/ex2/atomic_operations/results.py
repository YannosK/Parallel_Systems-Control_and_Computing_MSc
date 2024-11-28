import subprocess
import re
import csv
from tqdm import tqdm

def get_excution_times(threads, iterations):

    result = subprocess.run(['python3', 'runner.py', str(threads), str(iterations)], capture_output=True, text=True)
    output = result.stdout

    match = re.search(r"Elapsed time:\s*([\d.]+)", output)
    if match:
        return float(match.group(1))
    else:
        return None

if __name__ == "__main__":

    print('************************************************************************')
    
    print("Running reuslts generation script for Exercise 1.2 / Atomic Lock")

    ###############################################
    # Parameters
    ###############################################

    runs = 10   # number of runs for each execution
    threads = [1, 2, 4, 8, 16, 32] # number of threads
    max_iterations = 6 # maximum exponent of iterations of for loops of C code threads (it is decimal exponent)

    ###############################################
    # Collect results
    ###############################################

    results = []

    for i in range(1, max_iterations + 1, 1):
        iterations = 10 ** i

        thread_times = []
        for thread in threads:

            run_times = []
            for j in tqdm(range(runs), desc=f'{thread} threads, {iterations} iterations'):
                run_times.append(get_excution_times(thread, iterations))
            run_time = sum(run_times) / len(run_times)

            thread_times.append(run_time)
        
        results.append([iterations] + thread_times)

    ###############################################
    # Update csv
    ###############################################

    with open("execution_times.csv", "w", newline="") as csvfile:
        csvwriter = csv.writer(csvfile)

        thread_header = []
        for thread in threads:
            thread_header.append(f'{thread} Threads')
        csvwriter.writerow(['Iterations'] + thread_header)

        for result in results:
            csvwriter.writerow(result)


    print('************************************************************************')