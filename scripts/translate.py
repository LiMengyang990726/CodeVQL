import argparse
import os
import sys
import subprocess
import resource
import time

def validatePath(path):
    if ((not os.path.isdir(path)) and (not os.path.isfile(path))):
        print('The path or file ' + path + ' specified does not exist')
        sys.exit()

# Create the parser
runner = argparse.ArgumentParser(description='This script automate the process of translating CodeVQL to Souffle.')

# Add the arguments
runner.add_argument('--output_path',
                    type=str,
                    help='the path to the analysis output')   
runner.add_argument('--query_file_path',
                    type=str,
                    help='the path to the input query file')
runner.add_argument('--codevql_path',
                    type=str,
                    help='the path to the codevql from query langauge to internal declarative langauge, install here(https://github.com/LiMengyang990726/codevql/)') 

# Get all the input arguments and validate
args = runner.parse_args()

output_path = args.output_path
validatePath(output_path)

query_file_path = args.query_file_path
validatePath(query_file_path)

codevql_path = args.codevql_path
validatePath(codevql_path)

# Execute
start_time = time.time()
process = subprocess.Popen(
    'cd %s/translator && mkdir -p rules && ./translator %s' % (codevql_path, query_file_path), 
    shell=True
)
process.wait()

process = subprocess.Popen(
    'mv %s/translator/rules %s/rules' % (codevql_path, output_path), 
    shell=True
)
process.wait()
end_time = time.time()

time_usage = end_time - start_time
mem_usage = resource.getrusage(resource.RUSAGE_CHILDREN).ru_maxrss + resource.getrusage(resource.RUSAGE_SELF).ru_maxrss
print(str(time_usage) + "\t" + str(mem_usage))