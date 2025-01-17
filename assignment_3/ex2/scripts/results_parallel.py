import argparse
import subprocess
import re
import os
import csv
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
           └── parallel
               └── <filename>
   ```
   '''

   result_dir = "../results"
   if not os.path.exists(result_dir):
      os.makedirs(result_dir)

   result_par = os.path.join(result_dir, 'parallel')
   if not os.path.exists(result_par):
      os.makedirs(result_par)

   file = filename

   file_par = os.path.join(result_par, file)
   if not os.path.exists(file_par):
       with open(file_par, 'w') as newfile:
           newfile.close()
   else:
       if cleanup == True:
           with open(file_par, 'w') as cleanfile:
               cleanfile.close()
   
   return file_par

def get_results(
        problemsize : int,
        processcount: int
) -> TupleToUse[float,float]:
   '''
   Calls the python `runner_parallel.py` script that builds and runs the program.
   It returns a tuple with the execution time  and the intercommunications time
   meausred in seconds.

   Args:
   - problemsize (`int`): The number of rows or columns of the square matrix 
   - processcount (`int`): The number of MPI processes to run the program

   Returns:
   - a Tuple of two `float` members. First member is execution time
   and second member is intercommunications time
   - a tuple of `None` if one of the two is not printed 
   '''

   command = [
      'python3',
      'runner_parallel.py',
      str(processcount),
      str(problemsize),
      '-m',
      '-c'
   ]

   result = subprocess.run(command, capture_output=True, text=True)
   output = result.stdout

   match = re.search(r"Parallel matrix-vector multiplication execution time:\s*([\d.]+)", output)
   if match:
       exec_time = float(match.group(1))
   else:
       return [None,None]
   
   match = re.search(r"Parallel matrix-vector multiplication data sharing time:\s*([\d.]+)", output)
   if match:
       share_time = float(match.group(1))
   else:
       return [None,None]
   
   return [exec_time, share_time]
    
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
   # Iterators
   #****************#

   sizes = [8192, 16384, 32768] # problem size
   # processescounts = [1, 4, 16, 64, 128] # MPI process count
   processescounts = [1, 4, 8] # MPI process count

   #****************#
   # Constants
   #****************#
   
   executions = 10 # number of executions to compute average

   exectimes_file_name = 'parallel_execution_timings.csv' # name of file to store results of execution timings
   sharetimes_file_name = 'parallel_datashare_timings.csv' # name of file to store results of data share timings

   file_header = ['Size'] # header row of file to store results
   for processes in processescounts:
      file_header.append(f'{processes}')

   #****************#
   # Logic
   #****************#

   exectimes_results_file = create_result_directories(exectimes_file_name, args.clean)
   sharetimes_results_file = create_result_directories(sharetimes_file_name, args.clean)

   for size in sizes:

      exec_file_row = [size]
      share_file_row = [size]
      
      for processes in processescounts:

         exec_timings = []
         share_timings = []

         for i in tqdm(range(executions), desc=f'Parallel for {size} size and {processes} processes: '):
            timing_tuple = get_results(size, processes)

            exec_timing = timing_tuple[0]
            share_timing = timing_tuple[1]

            exec_timings.append(exec_timing)
            share_timings.append(share_timing)

         exec_timing_avg = sum(exec_timings) / len(exec_timings)
         share_timing_avg = sum(share_timings) / len(share_timings)

         exec_file_row.append(exec_timing_avg)
         share_file_row.append(share_timing_avg)

      with open(exectimes_results_file, 'a', newline='') as csvfile:
         writer = csv.writer(csvfile)
         if not os.path.getsize(exectimes_results_file) > 0:
            writer.writerow(file_header)
         writer.writerow(exec_file_row)
         csvfile.close()

      with open(sharetimes_results_file, 'a', newline='') as csvfile:
         writer = csv.writer(csvfile)
         if not os.path.getsize(sharetimes_results_file) > 0:
            writer.writerow(file_header)
         writer.writerow(share_file_row)
         csvfile.close()
