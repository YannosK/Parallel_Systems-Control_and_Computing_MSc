import argparse
import csv
import os

def csv_to_list(filename):
    '''
    Turn the contents of a csv file into a list
    Excluding the header
    '''
    with open(filename, 'r') as file:
        reader = csv.reader(file)
        next(reader)  # Skip the header row
        return [row for row in reader]
    
parser = argparse.ArgumentParser()
parser.add_argument('method', help='The method for back-substitution must either be \'rows\' or \'columns\'')

if __name__ == "__main__":

    ###################################
    # Argument check
    ###################################

    args = parser.parse_args()

    if args.method not in ['rows', 'columns']:
        raise ValueError("Incorrect argument")
    
    method = args.method

    ###################################
    # templates and .tex files
    ###################################

    if method == 'rows':
        output_filename = os.path.join('..', 'results', 'latex_tables', 'rows_speedup.tex')
    elif method == 'columns':
        output_filename = os.path.join('..', 'results', 'latex_tables', 'columns_speedup.tex')

    start_block = \
    '''
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    \SetCell[r=5]{{m}} {Scheduling}
    '''

    mid_block = \
    '''
    %%%%%%%%%%%%%
    & {Iterations} &
    {Val_1} &
    {Val_2} &
    {Val_3} &
    {Val_4} &
    {Val_8} \\\\
    '''

    end_block = \
    '''
    %%%%%%%%%%%%%
    \hline
    '''

    tex_block = '\t'

    ###################################
    # Iterators
    ###################################

    iterations_start = 2500
    schedules = ['static', 'dynamic', 'guided']
    threadcount = [1, 2, 3, 4, 8]
    chunks = ['default', ['1', 'maxchunk']]

    ###################################
    # Main logic
    ###################################

    method_dir = os.path.join('..', 'results', 'final_results', method)
    base_file = os.path.join(method_dir, 'speedup')     # DOING IT ONLY FOR SPEEDUP

    for schedule in schedules:
        for ch in chunks:

            if ch == 'default':
                chunk = 'default'
            else:
                chunk = ch[0] if schedule == 'static' else ch[1]

            csv_file = base_file + '__' + schedule + '_' + chunk + '.csv'
            if not os.path.exists(csv_file):
                raise FileNotFoundError(f"csv file: {csv_file} not found")

            if chunk == 'default':
                schedule_str = schedule
            elif chunk == '1':
                schedule_str = schedule + ', 1'
            elif chunk == 'maxchunk':
                schedule_str = schedule + ', maxchunk'
            
            tex_block += start_block.format(
                Scheduling=schedule_str
            ).strip()

            tex_block += '\n\t'
            
            valuelist = csv_to_list(csv_file)

            for value_row in valuelist:
                tex_block += mid_block.format(
                    Iterations=value_row[0],
                    Val_1=value_row[1],
                    Val_2=value_row[2],
                    Val_3=value_row[3],
                    Val_4=value_row[4],
                    Val_8=value_row[5]
                ).strip()
                tex_block += '\n\t'

            tex_block += end_block.strip()
            tex_block += '\n\t'
    
        with open(output_filename, 'w') as tex_file:
            tex_file.write(tex_block)