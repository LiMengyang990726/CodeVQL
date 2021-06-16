import argparse
import os
import sys
import subprocess
import re
import resource
import time

def validatePath(path):
    if ((not os.path.isdir(path)) and (not os.path.isfile(path))):
        print('The path or file ' + path + ' specified does not exist')
        sys.exit()

# Create the parser
runner = argparse.ArgumentParser(description='This script automate the process of translating EvoMe to Souffle.')

# Add the arguments
runner.add_argument('--repo_path',
                    type=str,
                    help='the path to the program to be analyzed upon')
runner.add_argument('--output_path',
                    type=str,
                    help='the path to the analysis output')   
runner.add_argument('--evome_path',
                    type=str,
                    help='the path to the EvoMe from query langauge to internal declarative langauge, install here(https://github.com/LiMengyang990726/EvoMe/)')
runner.add_argument('--cslicer_path',
                    type=str,
                    help='the path to the tool that generates program facts, install here(https://bitbucket.org/liyistc/gitslice/src/facts-dl4ql/)')            
runner.add_argument('--program_fact_path',
                    type=str,
                    help='the path to the pre-generated fact path')

# Get all the input arguments and validate
args = runner.parse_args()

repo_path = args.repo_path
validatePath(repo_path)

output_path = args.output_path
validatePath(output_path)

evome_path = args.evome_path
validatePath(evome_path)

cslicer_path = args.cslicer_path
validatePath(cslicer_path)

program_fact_path = args.program_fact_path
validatePath(program_fact_path)

# Step 1: Execute version related souffle files to generate facts in the required sequence
start_time = time.time()
result_sequence = []
regex_from = re.compile('SelectedVersion[\w]+from\.dl')
for f in os.listdir(os.path.join(output_path, "rules")):
    file_name = os.path.join(os.path.join(output_path, "rules"), f)
    if os.path.isfile(file_name) and regex_from.match(f):
            result_sequence.append(f)

regex_base = re.compile('SelectedVersion[\w]+base\.dl')
for f in os.listdir(os.path.join(output_path, "rules")):
    file_name = os.path.join(os.path.join(output_path, "rules"), f)
    if os.path.isfile(file_name) and regex_base.match(f):
            result_sequence.append(f)

regex_plain = re.compile('SelectedVersion[\w]+\.dl')
for f in os.listdir(os.path.join(output_path, "rules")):
    file_name = os.path.join(os.path.join(output_path, "rules"), f)
    if os.path.isfile(file_name) and regex_plain.match(f):
            result_sequence.append(f)

result_sequence.append("Version.dl")
os.chdir(os.path.join(output_path, "rules"))
for file_name in result_sequence:
    command = "souffle -F " + os.path.join(output_path, ".facts") + " -D " + os.path.join(output_path, ".facts") + " " + file_name
    os.system(command)
for file_name in result_sequence:
    f = os.path.join(os.path.join(output_path, "rules"),file_name)
    if os.path.isfile(f):
        os.remove(f)

# Step 2: Copying over pre-generated program fact files
os.system('cp -a %s/. %s/.facts' % (program_fact_path, output_path))

end_time = time.time()
time_usage = end_time - start_time
mem_usage = resource.getrusage(resource.RUSAGE_CHILDREN).ru_maxrss + resource.getrusage(resource.RUSAGE_SELF).ru_maxrss
print(str(time_usage) + "\t" + str(mem_usage))
