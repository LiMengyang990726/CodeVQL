import argparse
import os
import shlex
import shutil
import sys
import subprocess
import re
import resource
import time
from util import validate_path


def main():
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
                        help='the path to the EvoMe from query langauge to internal declarative langauge, '
                             'install here(https://github.com/LiMengyang990726/EvoMe/)')
    runner.add_argument('--cslicer_path',
                        type=str,
                        help='the path to the tool that generates program facts, '
                             'install here(https://bitbucket.org/liyistc/gitslice/src/facts-dl4ql/)')
    runner.add_argument('--program_fact_path',
                        type=str,
                        help='the path to the pre-generated fact path')

    # Get all the input arguments and validate
    args = runner.parse_args()

    repo_path = args.repo_path
    output_path = args.output_path
    evome_path = args.evome_path
    cslicer_path = args.cslicer_path
    program_fact_path = args.program_fact_path

    for p in [repo_path, output_path, evome_path, cslicer_path, program_fact_path]:
        validate_path(p)


def sample_repo_fact_gen(output_path, program_fact_path):
    # Step 1: Execute version related souffle files to generate facts in the required sequence
    start_time = time.time()
    result_sequence = []
    regex_from = re.compile(r'SelectedVersion[\w]+from\.dl')
    for f in os.listdir(os.path.join(output_path, "rules")):
        file_name = os.path.join(os.path.join(output_path, "rules"), f)
        if os.path.isfile(file_name) and regex_from.match(f):
            result_sequence.append(f)

    regex_base = re.compile(r'SelectedVersion[\w]+base\.dl')
    for f in os.listdir(os.path.join(output_path, "rules")):
        file_name = os.path.join(os.path.join(output_path, "rules"), f)
        if os.path.isfile(file_name) and regex_base.match(f):
            result_sequence.append(f)

    regex_plain = re.compile(r'SelectedVersion[\w]+\.dl')
    for f in os.listdir(os.path.join(output_path, "rules")):
        file_name = os.path.join(os.path.join(output_path, "rules"), f)
        if os.path.isfile(file_name) and regex_plain.match(f):
            result_sequence.append(f)

    result_sequence.append("Version.dl")
    os.chdir(os.path.join(output_path, "rules"))
    for file_name in result_sequence:
        # command = "souffle -F " + os.path.join(output_path, ".facts") + " -D " + os.path.join(output_path, ".facts") + " " + file_name
        # os.system(command)
        command = f"souffle -F {output_path}/.facts -D {output_path}/.facts {file_name}"
        subprocess.run(shlex.split(command), check=True)
    for file_name in result_sequence:
        f = os.path.join(os.path.join(output_path, "rules"), file_name)
        if os.path.isfile(f):
            os.remove(f)

    # Step 2: Copying over pre-generated program fact files
    os.system('cp -a %s/. %s/.facts' % (program_fact_path, output_path))
    # shutil.copytree(program_fact_path, output_path/".facts")

    end_time = time.time()
    time_usage = end_time - start_time
    mem_usage = resource.getrusage(resource.RUSAGE_CHILDREN).ru_maxrss + resource.getrusage(resource.RUSAGE_SELF).ru_maxrss
    print(str(time_usage) + "\t" + str(mem_usage))
