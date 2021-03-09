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
runner = argparse.ArgumentParser(description='This script automate the process of translating CodeVQL to Souffle.')

# Add the arguments
runner.add_argument('--repo_path',
                    type=str,
                    help='the path to the program to be analyzed upon')
runner.add_argument('--output_path',
                    type=str,
                    help='the path to the analysis output')   
runner.add_argument('--codeqltosouffle_path',
                    type=str,
                    help='the path to the translator from query langauge to internal declarative langauge, install here(https://github.com/LiMengyang990726/codeqltosouffle/)')
runner.add_argument('--cslicer_path',
                    type=str,
                    help='the path to the tool that generates program facts, install here(https://bitbucket.org/liyistc/gitslice/src/facts-dl4ql/)')            

# Get all the input arguments and validate
args = runner.parse_args()

repo_path = args.repo_path
validatePath(repo_path)

output_path = args.output_path
validatePath(output_path)

codeqltosouffle_path = args.codeqltosouffle_path
validatePath(codeqltosouffle_path)

cslicer_path = args.cslicer_path
validatePath(cslicer_path)

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
    command = "souffle -F " + os.path.join(os.path.join(output_path, ".facts"), "20-deps") + " " + file_name
    os.system(command)
for file_name in result_sequence:
    f = os.path.join(os.path.join(output_path, "rules"),file_name)
    if os.path.isfile(f):
        os.remove(f)

# Step 2: Check out each of the unique commit
os.chdir(repo_path)
original_commit = subprocess.check_output(['git', 'rev-parse', 'HEAD']).strip().decode("utf-8")

commits = []
with open(os.path.join(output_path, ".facts/20-deps/VersionComb.facts")) as fp:
    for i, line in enumerate(fp):
        commits.append(line.strip().split("\t")[0])
commits = set(commits)
end_time = time.time()

subprocesses_usage = []
for commit in commits:
    command = "python3.7 " + os.path.join(codeqltosouffle_path, "automationtools/fact-generation.py") + \
        " --repo_path " + repo_path + \
        " --cslicer_path " + cslicer_path + \
        " --output_path " + output_path + \
        " --commit " + commit
    usage = subprocess.check_output(command, shell=True).strip().decode("utf-8")
    subprocesses_usage.append(usage)

command = "git checkout " + original_commit + "> /dev/null 2>&1"
os.system(command)

time_usage = end_time - start_time
for subprocess_usage in subprocesses_usage:
    time_usage += int(subprocess_usage.split("\t")[1])
mem_usage = resource.getrusage(resource.RUSAGE_CHILDREN).ru_maxrss + resource.getrusage(resource.RUSAGE_SELF).ru_maxrss
print(str(time_usage) + "\t" + str(mem_usage))
