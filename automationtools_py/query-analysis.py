import argparse
import os
import sys
import resource
import time

def validatePath(path):
    if ((not os.path.isdir(path)) and (not os.path.isfile(path))):
        print('The path or file ' + path + ' specified does not exist')
        sys.exit()

# Create the parser
runner = argparse.ArgumentParser(description='This script automate the process of analyzing the final query.')

# Add the arguments
runner.add_argument('--output_path',
                    type=str,
                    help='the path to the analysis output')   

# Get all the input arguments and validate
args = runner.parse_args()

output_path = args.output_path
validatePath(output_path)

# Execute
start_time = time.time()
os.system('mkdir %s/output && souffle -F %s/.facts/20-deps/ -D %s/output %s/rules/main.dl' % (output_path, output_path, output_path, output_path))
end_time = time.time()
time_usage = end_time - start_time
mem_usage = resource.getrusage(resource.RUSAGE_CHILDREN).ru_maxrss + resource.getrusage(resource.RUSAGE_SELF).ru_maxrss
print(str(time_usage) + "\t" + str(mem_usage))
