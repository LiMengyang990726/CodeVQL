import argparse
import os
import sys
import subprocess
import resource
import time
from subprocess import check_output
from pyfiglet import Figlet

def validatePath(path):
    if ((not os.path.isdir(path)) and (not os.path.isfile(path))):
        print('The path or file ' + path + ' specified does not exist')
        sys.exit()

# Print name of the APP
title = Figlet(font='slant')
subtitle = Figlet(font='bubble')
print(title.renderText('CodeVQL'))
print(subtitle.renderText('Version-Aware Security Analysis'))

# Create the parser
runner = argparse.ArgumentParser(description='\
            This is the Command Line Interface(CLI) of the CodeVQL language. \
            CodeVQL can perform the basic security analysis in a version-aware way. \
            For more information, please check at this link [TODO]. \
        ')

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
runner.add_argument('--query_file_path',
                    type=str,
                    help='the path to the input query file')
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

gitfacts_path = args.gitfacts_path
validatePath(gitfacts_path)

output_path = args.output_path
subprocess.run(['mkdir', '-p', output_path])
subprocess.run(['chmod', '-R', '777', output_path])
validatePath(output_path)

query_file_path = args.query_file_path
validatePath(query_file_path)

codeqltosouffle_path = args.codeqltosouffle_path
validatePath(codeqltosouffle_path)

cslicer_path = args.cslicer_path
validatePath(cslicer_path)

# Step 1: Generate Git Facts
os.system(
    'echo Step 1: Start generating git facts && \
    python3.7 %s/automationtools_py/gitfact-generation.py --repo_path %s --gitfacts_path %s --output_path %s && \
    echo Step 1 is done!' % (codeqltosouffle_path, repo_path, gitfacts_path, output_path)
)

# Step 2: Translate CodeVQL to Souffle
os.system(
    'echo Step 2: Start translating CodeVQL to Souffle && \
    python3.7 %s/automationtools_py/translate.py --query_file_path %s --codeqltosouffle_path %s --output_path %s && \
    echo Step 2 is done!' % (codeqltosouffle_path, query_file_path, codeqltosouffle_path, output_path)
)

# Step 3: Get multiversion facts
os.system(
    'echo Step 3: Start Generating program facts in selected version && \
    python3.7 %s/automationtools_py/multiversion-fact-generation.py --repo_path %s --cslicer_path %s --codeqltosouffle_path %s --output_path %s && \
    echo Step 3 is done!' % (codeqltosouffle_path, repo_path, cslicer_path, codeqltosouffle_path, output_path)
)

# Step 4: Analyze to get the result
os.system(
    'echo Step 4: Start analyzing the query && \
    python3.7 %s/automationtools_py/query-analysis.py --output_path %s && \
    echo Step 4 is done and program finished!' % (codeqltosouffle_path, output_path))
