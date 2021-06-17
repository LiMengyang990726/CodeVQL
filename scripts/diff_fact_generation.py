import argparse
import logging
import os
import resource
import shutil
import subprocess
import time
from pathlib import Path

from util import validate_path, ensure_dir, RevPair, mvn_build, generate_config_file

logger = logging.getLogger(__name__)


def run_diff_fact_gen():
    # Create the parser
    runner = argparse.ArgumentParser(description='This script automate the process of generating git facts.')

    # Add the arguments
    runner.add_argument('--repo_path', type=str, help='the path to the program to be analyzed upon')
    runner.add_argument('--cslicer_path', type=str,
                        help='the path to the tool that generates program facts, '
                             'install here(https://bitbucket.org/liyistc/gitslice/src/facts-dl4ql/)')
    runner.add_argument('--output_path', type=str, help='the path to the analysis output')

    # Get all the input arguments and validate
    args = runner.parse_args()

    repo_path = Path(args.repo_path)
    cslicer_path = Path(args.cslicer_path)
    output_path = Path(args.output_path)
    for p in [repo_path, output_path, cslicer_path]:
        validate_path(p)
    diff_fact_gen(repo_path, output_path, cslicer_path)


def diff_fact_gen(repo_path: Path, output_path: Path, cslicer_path: Path):
    inter_msg_file_name = output_path / "rules/interMsgForPipeline.txt"
    start_time = time.time()
    # Step 1: Read each line inside the inter message file created by the translator
    if os.stat(inter_msg_file_name).st_size != 0:
        file = open(inter_msg_file_name, 'r')
        lines = file.readlines()
        count = 0
        for line in lines:
            results = line.strip().split("\t")
            if len(results) <= 2:
                continue
            else:
                rev_pair = RevPair(results[0], results[1])
                # Step 2: Generate the properties file and Run gitslice

                # os.chdir(repo_path)
                # os.system('mvn compile > /dev/null 2>&1')
                # os.system('mvn test-compile > /dev/null 2>&1')
                if mvn_build(repo_path) is not None:
                    logger.warning(f"Error happened during build for {repo_path} @ {rev_pair[1]}.")
                # os.system('touch diff-%d.properties' % count)
                # os.system('echo "repoPath = %s/.git" >> diff-%d.properties' % (repo_path, count))
                # os.system('echo "startCommit = %s" >> diff-%d.properties' % (start_commit, count))
                # os.system('echo "endCommit = %s" >> diff-%d.properties' % (end_commit, count))
                # os.system('echo "classRoot = %s/target" >> diff-%d.properties' % (repo_path, count))
                cfg_path = repo_path / f"diff-{count}.properties"
                generate_config_file(repo_path, repo_path / "target", rev_pair, cfg_path)

                os.system('java -jar %s -c %s/diff-%d.properties -e dl --ext dep diff > /dev/null 2>&1' % (
                    cslicer_path, repo_path, count))
                # Step 3: Move the needed files to the designated location
                for fact_type in results[2:]:
                    fact_type += ".facts"
                    fact_file = os.path.join(os.path.join(repo_path, ".facts/30-diff"), fact_type)
                    original_file = open(fact_file, 'r')
                    new_combined_file = open(os.path.join(os.path.join(output_path, ".facts"), fact_type), 'a+')
                    new_combined_file.write(original_file.read())
                    new_combined_file.seek(0)
                    original_file.close()
                    new_combined_file.close()
                # Step 4: Remove intermediate files
                os.remove(os.path.join(repo_path, 'diff-%d.properties' % (count)))
                shutil.rmtree(os.path.join(repo_path, ".facts"))
                count += 1

    # Step 5: Count statistics
    end_time = time.time()
    time_usage = end_time - start_time
    mem_usage = resource.getrusage(resource.RUSAGE_CHILDREN).ru_maxrss + resource.getrusage(
        resource.RUSAGE_SELF).ru_maxrss
    print(str(time_usage) + "\t" + str(mem_usage))
