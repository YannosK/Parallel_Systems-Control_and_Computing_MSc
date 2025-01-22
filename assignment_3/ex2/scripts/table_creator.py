import argparse
import csv
import os

def csv_to_list(filename):
   with open(filename, 'r') as file:
      reader = csv.reader(file)
      next(reader)  # Skip the header row
      return [row for row in reader]
    
def string_to_power_of_ten(value: str) -> str:
   if not value.isdigit() or not value.startswith("1") or any(c != '0' for c in value[1:]):
      raise ValueError("Input must be a string like '10', '100', '1000', etc.")
   exponent = len(value) - 1
   return f"$10^{exponent}$"

def create_tex_file(
      maindir : str,
      filename : str
   ) -> str:
   '''
   Creates the tex file of a given name in the given main directory.
   It opens it, cleans it, closes it and returns the path to it
   '''

   if not os.path.exists(maindir):
      raise FileNotFoundError(f'{maindir} does not exist')

   file = os.path.join(maindir, filename)
   with open(file, 'w') as newfile:
       newfile.close()
   
   return file


parser = argparse.ArgumentParser()
parser.add_argument('-t', '--type',
                    choices= [
                        'speedup',
                        'efficiency',
                        'exectimes',
                        'sharetimes','all'
                     ],
                     default='all',
                     help='what type of table to create'
                     )

if __name__ == "__main__":
    
   args = parser.parse_args()

   ###################################
   # Constants
   ###################################

   main_dir = os.path.join('..', 'results', 'latex_tables')
   if not os.path.exists(main_dir):
      os.makedirs(main_dir)

   source_dir = os.path.join('..', 'results', 'final_results', 'parallel')
   if not os.path.exists(source_dir):
      raise FileNotFoundError(f"No direcotry {source_dir}")

   ###################################
   # templates and files
   ###################################

   output_files = []
   source_files = []

   if args.type == 'all' or 'speedup':
      
      sp_file = create_tex_file(main_dir, 'speedup.tex')
      output_files.append(sp_file)

      sp_source = os.path.join(source_dir, 'speedups.csv')
      if not os.path.exists(sp_source):
         raise FileNotFoundError(f'File {sp_source} not found')
      source_files.append(sp_source)

   if args.type == 'all' or 'efficiency':

      ef_file = create_tex_file(main_dir, 'efficiency.tex')
      output_files.append(ef_file)

      ef_source = os.path.join(source_dir, 'efficiencies.csv')
      if not os.path.exists(ef_source):
         raise FileNotFoundError(f'File {ef_source} not found')
      source_files.append(ef_source)

   if args.type == 'all' or 'exectimes':
      et_file = create_tex_file(main_dir, 'execution_times.tex')
      output_files.append(et_file)

      et_source = os.path.join(source_dir, 'parallel_execution_timings.csv')
      if not os.path.exists(et_source):
         raise FileNotFoundError(f'File {et_source} not found')
      source_files.append(et_source)

   if args.type == 'all' or 'sharetimes':

      st_file = create_tex_file(main_dir, 'share_times.tex')
      output_files.append(st_file)

      st_source = os.path.join(source_dir, 'parallel_datashare_timings.csv')
      if not os.path.exists(st_source):
         raise FileNotFoundError(f'File {st_source} not found')
      source_files.append(st_source)
   
   scaffold = '''
	 %%%%%%%%%%%%%%%%%%
    {Size}
    % 1 Process
    & {Val_1}
    % 4 Processes
    & {Val_2}
    % 16 Processes
    & {Val_3}
    % 64 
    & {Val_4}
    % 128 Processes
    & {Val_5}
    \\\\
'''

   ###################################
   # Main logic
   ###################################

   for output_file, source_file in zip(output_files, source_files):
      valuelist = csv_to_list(source_file)
      with open(output_file, 'w') as tex_file:
         for value_row in valuelist:
            tex_block = scaffold.format(
               Size=value_row[0],
               Val_1=value_row[1],
               Val_2=value_row[2],
               Val_3=value_row[3],
               Val_4=value_row[4],
               Val_5=value_row[5],
            ).strip()
            tex_file.write("\t" + tex_block + "\n")
