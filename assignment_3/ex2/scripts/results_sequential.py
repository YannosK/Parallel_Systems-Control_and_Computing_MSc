import argparse
import subprocess
import re
import os
import csv
from tqdm import tqdm # type: ignore

################################################
# Function definitions
################################################

def create_result_directories(
      filename,
      cleanup=False
   ) -> str:
   '''
   A function to create the directory structure of the results.
   If the files or the directories exist it does not create them again.

   Args:
   - filename (`string`): name of the file to store the timings
   - cleanup (`bool`): if True it cleans the files
   
   Returns:
   - a string of the directory of the results file

   Structure:

   ```
   ../results
           └── sequential
               └── <filename>
   ```
   '''

   result_dir = "../results"
   if not os.path.exists(result_dir):
      os.makedirs(result_dir)

   result_seq = os.path.join(result_dir, 'sequential')
   if not os.path.exists(result_seq):
      os.makedirs(result_seq)

   file = filename

   file_seq = os.path.join(result_seq, file)
   if not os.path.exists(file_seq):
       with open(file_seq, 'w') as newfile:
           newfile.close()
   else:
       if cleanup == True:
           with open(file_seq, 'w') as cleanfile:
               cleanfile.close()
   
   return file_seq

def get_results(
        problemsize : int,
) -> float:
   '''
   Calls the python `runner_sequential.py` script that builds and runs the program.
   It returns a tuple with the execution time in seconds.

   Args:
   - problemsize (`int`): The number of rows or columns of the square matrix 

   Returns:
   - execution time as a `float`
   - `None` if timing was not printed
   '''

   command = [
       'python3',
       'runner_sequential.py',
       str(problemsize),
   ]

   result = subprocess.run(command, capture_output=True, text=True)
   output = result.stdout

   match = re.search(r"Sequential matrix-vector multiplication time:\s*([\d.]+)", output)
   if match:
       exec_time = float(match.group(1))
       return exec_time
   else:
       return None
    
################################################
# Parser
################################################

parser = argparse.ArgumentParser()
parser.add_argument('-c', '--clean',
                    action='store_true',
                    help='Cleans up result files')

################################################
# Main
################################################

if __name__ == "__main__":

   args = parser.parse_args()
    
   #****************#
   # Constants
   #****************#
   
   executions = 10 # number of executions to compute average
   file_name = 'sequential_timings.csv' # name of file to store results
   file_header = ['Size', 'Time'] # header row of file to store results 

   #****************#
   # Iterators
   #****************#

   sizes = [128, 2048, 32768] # problem size

   #****************#
   # Logic
   #****************#

   results_file = create_result_directories(file_name, args.clean)

   for size in sizes:

      timings = []
      for i in tqdm(range(executions), desc=f'Sequential for size {size}: '):
         timing = get_results(size)
         timings.append(timing)

      timing_avg = sum(timings) / len(timings)
      file_row = [size, timing_avg]

      with open(results_file, 'a', newline='') as csvfile:
         writer = csv.writer(csvfile)
         if not os.path.getsize(results_file) > 0:
            writer.writerow(file_header)
         writer.writerow(file_row)
         csvfile.close()



           

