import argparse
import subprocess
import sys
import os
import csv

if sys.version_info < (3, 9):
    from typing import Tuple
    TupleToUse = Tuple
else:
    TupleToUse = tuple

################################################
# Function definitions
################################################

def get_fresh_results() -> str:
   '''
   Runs the 'results_sequential.py` and `results_parallel.py` again
   to produce new results .csv files

   Returns:
   - A `str` with an error message or None if all run correctly
   '''

   command_seq = [
      'python3',
      'results_sequential.py',
      '-c'
   ]

   command_par = [
      'python3',
      'results_parallel.py',
      '-c'
   ]

   try:
      with subprocess.Popen(command_seq, stdout=sys.stdout, stderr=sys.stderr, text=True) as result_seq, \
           subprocess.Popen(command_par, stdout=sys.stdout, stderr=sys.stderr, text=True) as result_par:
         result_seq.wait()
         result_par.wait()
   except KeyboardInterrupt:
      result_seq.terminate()
      result_par.terminate()
      result_seq.wait(timeout=5)
      result_par.wait(timeout=5)
      return "Process terminated by user"

   return None

def check_file_existence() -> TupleToUse[str, str]:
   '''
   Checks if the necessary files to compute efficiency and speedup exist

   Returns:
   - `None` if one of a file does not exist or a tuple with the two paths as `str`.
   The first element is the path to the sequential timings, while the second is the
   path to the parallel timings
   '''

   file_seq = os.path.join(
      '..',
      'results',
      'sequential',
      'sequential_timings.csv'
   )

   file_par = os.path.join(
      '..',
      'results',
      'parallel',
      'parallel_execution_timings.csv'
   )

   ret_seq = file_seq if os.path.exists(file_seq) else None
   ret_par = file_par if os.path.exists(file_par) else None

   return [ret_seq, ret_par]

def create_result_files(
      speedup_filename,
      efficiency_filename,
      cleanup=False
   ) -> str:
   '''
   A function to create the files of the results.
   If the files already exist it does not create them again.

   Args:
   - speedup_filename (`string`): name of the file to store the speedup
   - efficiency_filename (`string`): name of the file to store the efficiency
   - cleanup (`bool`): if True it cleans the files
   
   Returns:
   - a Tuple of `str` with the first element being the speedup file name
   and the second being the efficiency filename
   - None if the initial directories are not found

   Structure:

   ```
   ../results
           └── parallel
               ├── <speedup_filename>
               └── <efficiency_filename>
   ```
   '''

   result_par = os.path.join('..', 'results', 'parallel')
   if not os.path.exists(result_par):
      return None

   file_sp = os.path.join(result_par, speedup_filename)
   if not os.path.exists(file_sp):
       with open(file_sp, 'w') as newfile:
           newfile.close()
   else:
       if cleanup == True:
           with open(file_sp, 'w') as cleanfile:
               cleanfile.close()
   
   file_ef = os.path.join(result_par, efficiency_filename)
   if not os.path.exists(file_sp):
       with open(file_ef, 'w') as newfile:
           newfile.close()
   else:
       if cleanup == True:
           with open(file_ef, 'w') as cleanfile:
               cleanfile.close()
   
   return [file_sp, file_ef]

def csv_to_list(filename, skipheader=True):
   '''
   Turn the contents of a csv file into a list
   Excluding the header

   Args:
   - filename (`str`); the path to the csv file
   - skipheader (`bool`): if set to False it includes the header in the result
   '''
   with open(filename, 'r') as file:
      reader = csv.reader(file)
      if skipheader:
         next(reader)
      return [row for row in reader]

################################################
# Parser
################################################

parser = argparse.ArgumentParser()

parser.add_argument('-f', '--fresh',
                    action='store_true',
                    help='With this flag it creates runs both sequential and parallel\
                     execution and computes timings anew')
parser.add_argument('-c', '--clean',
                    action='store_true',
                    help='Cleans up result files')

################################################
# Main
################################################

if __name__ == "__main__":

   args = parser.parse_args()

   if args.fresh:
      fresh_results_out = get_fresh_results()
      if fresh_results_out is not None:
         raise RuntimeError(fresh_results_out)
      
   file_dirs = check_file_existence()

   if file_dirs[0] is None:
      raise FileNotFoundError("No file of sequential timings found")
   else:
      file_seq = file_dirs[0]

   if file_dirs[1] is None:
      raise FileNotFoundError("No file of parallel timings found")
   else:
      file_par = file_dirs[1]

   seq_content_list = csv_to_list(file_seq)
   seq_size_list = [element[0] for element in seq_content_list]
   seq_timing_list = [element[1] for element in seq_content_list]

   par_content_list = csv_to_list(file_par, False)
   par_size_list = [element[0] for element in par_content_list]
   par_size_list.pop(0)
   par_timing_list = [element[1:] for element in par_content_list]
   processcount = par_timing_list.pop(0)

   result_csv = create_result_files('speedups.csv', 'efficiencies.csv', args.clean)
   if result_csv is None:
      raise FileNotFoundError("No proper storage directory")
   else:
      sp_csv = result_csv[0]
      ef_csv = result_csv[1]

   header = ['Size']
   for processes in processcount:
      header.append(f'{processes}_procs')

   speedup_list = []
   efficiency_list = []

   for index_seq, size in enumerate(seq_size_list):
      if size in par_size_list:
         index_par = par_size_list.index(size)

         sp_row = [size]
         ef_row = [size]

         for index_proc, processes in enumerate(processcount):
            sp = float(seq_timing_list[index_seq]) / float(par_timing_list[index_par][index_proc])
            ef = sp / float(processes)

            sp_row.append(sp)
            ef_row.append(ef)

         with open(sp_csv, 'a', newline='') as csvfile:
            writer = csv.writer(csvfile)
            if not os.path.getsize(sp_csv) > 0:
               writer.writerow(header)
            writer.writerow(sp_row)
            csvfile.close()

         with open(ef_csv, 'a', newline='') as csvfile:
            writer = csv.writer(csvfile)
            if not os.path.getsize(ef_csv) > 0:
               writer.writerow(header)
            writer.writerow(ef_row)
            csvfile.close()



