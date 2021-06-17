import argparse
import resource
import subprocess
import time

from util import validate_path, ensure_dir


def run_git_fact_gen():
    # Create the parser
    runner = argparse.ArgumentParser(description='This script automate the process of generating git facts.')

    # Add the arguments
    runner.add_argument('--repo_path', type=str, help='the path to the program to be analyzed upon')
    runner.add_argument('--gitfacts_path', type=str,
                        help='the path to the tool that generates git facts, '
                             'install here(https://github.com/uxhg/ext-gitfacts)')
    runner.add_argument('--output_path', type=str, help='the path to the analysis output')

    # Get all the input arguments and validate
    args = runner.parse_args()

    repo_path = args.repo_path
    gitfacts_path = args.gitfacts_path
    output_path = args.output_path
    for p in [repo_path, gitfacts_path, output_path]:
        validate_path(p)


def git_fact_gen(repo_path, gitfacts_path, output_path):
    # Execute
    start_time = time.time()
    ensure_dir(output_path / ".facts")
    subprocess.run([gitfacts_path / "target/release/gitfacts", repo_path, "-o", output_path / ".facts/history.facts"])
    end_time = time.time()

    time_usage = end_time - start_time
    mem_usage = resource.getrusage(resource.RUSAGE_CHILDREN).ru_maxrss + resource.getrusage(
        resource.RUSAGE_SELF).ru_maxrss
    print(str(time_usage) + "\t" + str(mem_usage))
