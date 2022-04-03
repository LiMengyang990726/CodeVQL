import argparse
import os
import re
import resource
import subprocess
import time
from pathlib import Path
import shlex
from typing import Optional

from util import validate_path, init_logging, checkout_commit
from fact_generation import gen_fact


def run_multi_ver_fact_gen():
    # Create the parser
    runner = argparse.ArgumentParser(description='This script automate the process of translating EvoMe to Souffle.')
    # Add the arguments
    runner.add_argument('--repo_path', type=str, help='the path to the program to be analyzed upon')
    runner.add_argument('--output_path', type=str, help='the path to the analysis output')
    runner.add_argument('--evome_path', type=str,
                        help='the path to the EvoMe from query langauge to internal declarative langauge, '
                             'install here(https://github.com/LiMengyang990726/EvoMe/)')
    runner.add_argument('--cslicer_path', type=str,
                        help='the path to the tool that generates program facts, '
                             'install here(https://bitbucket.org/liyistc/gitslice/src/facts-dl4ql/)')
    runner.add_argument('--souffle_path', type=str, required=False,
                        help='the path to Soufflé binary')

    # Get all the input arguments and validate
    args = runner.parse_args()
    repo_path = Path(args.repo_path)
    output_path = Path(args.output_path)
    evome_path = Path(args.evome_path)
    cslicer_path = Path(args.cslicer_path)
    souffle_path: Optional[Path] = Path(args.souffle_path) if args.souffle_path else None
    for p in [repo_path, output_path, evome_path, cslicer_path]:
        validate_path(p)
    multi_ver_fact_gen(repo_path, output_path, cslicer_path, souffle_path)


def multi_ver_fact_gen(repo_path, output_path, cslicer_path, souffle_path: Optional[Path] = None):
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
    souffle_bin = souffle_path if souffle_path else "souffle"
    for file_name in result_sequence:
        run_souffle = f'{souffle_bin} -F {output_path}/".facts" -D {output_path}/".facts" {file_name}'
        subprocess.run(shlex.split(run_souffle), cwd=output_path/"rules")
        # command = "souffle -F " + os.path.join(output_path, ".facts") + " -D "
        # + os.path.join(output_path, ".facts") + " " + file_name
    for file_name in result_sequence:
        f = output_path / "rules" / file_name
        if os.path.isfile(f):
            os.remove(f)

    # Step 2: Check out each of the unique commit
    original_commit = subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=repo_path).strip().decode("utf-8")

    commits = []
    with open(output_path / ".facts/VersionComb.facts") as fp:
        for i, line in enumerate(fp):
            commits.extend(line.strip().split("\t"))
    commits = set(commits)
    end_time = time.time()

    subprocesses_usage = []
    for commit in commits:
        usage = gen_fact(repo_path, output_path, cslicer_path, commit)
        # command = "python3.7 " + os.path.join(evome_path, "scripts/fact-generation.py") + \
        #           " --repo_path " + repo_path + \
        #           " --cslicer_path " + cslicer_path + \
        #           " --output_path " + output_path + \
        #           " --commit " + commit
        # usage = subprocess.check_output(command, shell=True).strip().decode("utf-8")
        subprocesses_usage.append(usage)

    # command = "git checkout " + original_commit + "> /dev/null 2>&1"
    # os.system(command)
    checkout_commit(repo_path, original_commit)

    time_usage = end_time - start_time
    for subprocess_usage in subprocesses_usage:
        time_usage += float(subprocess_usage.split("\t")[0])
    mem_usage = resource.getrusage(resource.RUSAGE_CHILDREN).ru_maxrss + resource.getrusage(
        resource.RUSAGE_SELF).ru_maxrss
    print(str(time_usage) + "\t" + str(mem_usage))


if __name__ == "__main__":
    init_logging()
    run_multi_ver_fact_gen()
