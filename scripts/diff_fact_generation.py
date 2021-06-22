import argparse
import logging
import os
import resource
import shlex
import shutil
import subprocess as sub
import time
from pathlib import Path

from util import validate_path, RevPair, generate_config_file, init_logging

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
                logger.info(f"Generating facts for commits {rev_pair.rev_old} - {rev_pair.rev_new}")
                # Step 2: Generate the properties file and Run gitslice

                # os.chdir(repo_path)
                # os.system('mvn compile > /dev/null 2>&1')
                # os.system('mvn test-compile > /dev/null 2>&1')
                # if mvn_build(repo_path) is not None:
                #    logger.warning(f"Error happened during build for {repo_path} @ {rev_pair[1]}.")
                # os.system('touch diff-%d.properties' % count)
                # os.system('echo "repoPath = %s/.git" >> diff-%d.properties' % (repo_path, count))
                # os.system('echo "startCommit = %s" >> diff-%d.properties' % (start_commit, count))
                # os.system('echo "endCommit = %s" >> diff-%d.properties' % (end_commit, count))
                # os.system('echo "classRoot = %s/target" >> diff-%d.properties' % (repo_path, count))
                cfg_path = repo_path / f"diff-{count}.properties"
                generate_config_file(repo_path, None, rev_pair, cfg_path)

                # os.system('java -jar %s -c %s/diff-%d.properties -e dl --ext dep diff > /dev/null 2>&1' % (
                #     cslicer_path, repo_path, count))
                cslicer_cmd = f"java -jar {cslicer_path} -c {cfg_path} -e dl -ext diff"
                run_cslicer = sub.run(shlex.split(cslicer_cmd), check=True, stdout=open(os.devnull), stderr=sub.PIPE)
                if len(run_cslicer.stderr) > 0:
                    logger.warning(f"Errors occurred when running CSlicer:\n\t{cslicer_cmd}")
                    logger.warning(f"------------------")
                    logger.warning(run_cslicer.stderr.decode("utf-8"))
                    logger.warning(f"------------------")
                # Step 3: Move the needed files to the designated location
                diff_facts_dir = repo_path / ".facts" / "30-diff"
                for fact_type in results[2:]:
                    fact_file = f"{fact_type}.facts"
                    fact_file_path = diff_facts_dir / fact_file
                    # original_file = open(fact_file, 'r')
                    combined_file_path = output_path / ".facts" / fact_file
                    with open(fact_file_path, 'r') as orig_file, open(combined_file_path, 'a+') as comb_file:
                        logger.info(f"Move generated differential facts: "
                                    f"{fact_file_path} => {combined_file_path}")
                        comb_file.write(orig_file.read())
                        comb_file.seek(0)
                # Step 4: Remove intermediate files
                # os.remove(os.path.join(repo_path, 'diff-%d.properties' % (count)))
                logger.info(f"Remove CSlicer configuration file @ {cfg_path}")
                os.remove(cfg_path)
                logger.info(f"Remove facts dir @ {diff_facts_dir}")
                shutil.rmtree(diff_facts_dir)
                count += 1

    # Step 5: Count statistics
    end_time = time.time()
    time_usage = end_time - start_time
    mem_usage = resource.getrusage(resource.RUSAGE_CHILDREN).ru_maxrss + resource.getrusage(
        resource.RUSAGE_SELF).ru_maxrss
    print(str(time_usage) + "\t" + str(mem_usage))


if __name__ == "__main__":
    init_logging()
    run_diff_fact_gen()
