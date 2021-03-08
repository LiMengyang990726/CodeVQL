import argparse
import os
import resource
import sys
import subprocess

def validatePath(path):
    if ((not os.path.isdir(path)) and (not os.path.isfile(path))):
        print('The path or file ' + path + ' specified does not exist')
        sys.exit()

# Create the parser
runner = argparse.ArgumentParser(description='This script automate the process of generating git facts.')

# Add the arguments
runner.add_argument('--repo_path',
                    type=str,
                    help='the path to the program to be analyzed upon')
runner.add_argument('--gitfacts_path',
                    type=str,
                    help='the path to the tool that generates git facts, install here(https://github.com/uxhg/ext-gitfacts)')
runner.add_argument('--output_path',
                    type=str,
                    help='the path to the analysis output')   

# Get all the input arguments and validate
args = runner.parse_args()

repo_path = args.repo_path
validatePath(repo_path)

gitfacts_path = args.gitfacts_path
validatePath(gitfacts_path)

output_path = args.output_path
validatePath(output_path)

# Execute
process = subprocess.Popen(
    'mkdir -p %s/.facts/20-deps && touch %s/.facts/20-deps/history.facts' % (output_path, output_path), 
    shell=True
)
process.wait()

process = subprocess.Popen(
    '%s/target/release/gitfacts %s -o %s/.facts/20-deps/history.facts' % (gitfacts_path, repo_path, output_path), 
    shell=True
)
process.wait()

usage = resource.getrusage(resource.RUSAGE_CHILDREN).ru_maxrss + resource.getrusage(resource.RUSAGE_SELF).ru_maxrss
print(usage)

