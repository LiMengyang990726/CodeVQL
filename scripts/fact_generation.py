import argparse
import os
import resource
import shutil
import subprocess
import time
from pathlib import Path

from util import validate_path, checkout_commit, mvn_build, generate_config_file, CFG_PATHS, ensure_dir, RevPair, \
    init_logging


def run_fact_gen():
    # Create the parser
    runner = argparse.ArgumentParser(
        description='This script automate the process of generating program fact at specified version.')

    # Add the arguments
    runner.add_argument('--repo_path',
                        type=str,
                        help='the path to the program to be analyzed upon')
    runner.add_argument('--output_path',
                        type=str,
                        help='the path to the analysis output')
    runner.add_argument('--cslicer_path',
                        type=str,
                        help='the path to the tool that generates program facts, install here(https://bitbucket.org/liyistc/gitslice/src/facts-dl4ql/)')
    runner.add_argument('--commit',
                        type=str,
                        help='the commit id to generate fact upon')

    # Get all the input arguments and validate
    args = runner.parse_args()

    repo_path = Path(args.repo_path)
    output_path = Path(args.output_path)
    cslicer_path = Path(args.cslicer_path)
    commit = args.commit
    for p in [repo_path, output_path, cslicer_path]:
        validate_path(p)
    gen_fact(repo_path, output_path, cslicer_path, commit)


def gen_fact(repo_path: Path, output_path: Path, cslicer_path: Path, commit: str) -> str:
    # Step 1: Compile maven project
    # os.chdir(repo_path)
    # os.system("git checkout " + commit + "> /dev/null 2>&1")
    checkout_commit(repo_path, commit)
    # os.system('mvn compile > /dev/null 2>&1')
    # os.system('mvn test-compile > /dev/null 2>&1')
    # only applied to single-mod projects
    mvn_build(repo_path)

    # Step 2: Create cslicer properties file
    # os.system('touch cslicer.properties')
    # os.system('echo "repoPath = %s/.git" >> cslicer.properties' % (repo_path))
    # os.system('echo "classRoot = %s/target" >> cslicer.properties' % (repo_path))
    cslicer_cfg: Path = repo_path / "cslicer.properties"
    generate_config_file(repo_path, None, RevPair(None, commit), cslicer_cfg)

    # Step 3: Run CSlicer to get fact files
    # os.system('java -jar %s -c %s/cslicer.properties -e dl --ext dep > /dev/null 2>&1' % (cslicer_path, repo_path))

    start_time = time.time()
    with open(CFG_PATHS["logging"] / "cslicer-dep.log", 'w') as collect_log:
        # check the implementation of -ver, to make sure it writes the correct version
        cslicer_cmd = ["java", "-jar", cslicer_path, "-c", cslicer_cfg, "-e", "dl", "-ver" "-ext", "dep"]
        subprocess.run(args=cslicer_cmd, stdout=collect_log, stderr=subprocess.STDOUT, check=True)
        ensure_dir(output_path / ".facts")
        # for f in os.listdir(repo_path/".facts/20-deps"):
        #     shutil.move(repo_path/".facts/20-deps"/f, output_path/".facts")
    end_time = time.time()

    # Step 4: Append version to get versionised fact file (Note: only for those inside 20-deps)
    for path, _, files in os.walk(os.path.join(repo_path, ".facts/20-deps")):
        for name in files:
            file_name = os.path.join(path, name)
            with open(file_name, 'r') as fr:
                file_lines = [''.join([x.strip(), '\t' + commit, '\n']) for x in fr.readlines()]
            with open(file_name, 'w') as fw:
                fw.writelines(file_lines)
            original_file = open(file_name, 'r')
            new_combined_file = open(os.path.join(os.path.join(output_path, ".facts"), name), 'a+')
            new_combined_file.write(original_file.read())
            new_combined_file.seek(0)
            original_file.close()
            new_combined_file.close()

    # Step 5: Remove intermediate files
    os.remove(os.path.join(repo_path, "cslicer.properties"))
    shutil.rmtree(os.path.join(repo_path, ".facts"))

    time_usage = end_time - start_time
    mem_usage = resource.getrusage(resource.RUSAGE_CHILDREN).ru_maxrss + resource.getrusage(
        resource.RUSAGE_SELF).ru_maxrss
    usage = f"{time_usage}\t{mem_usage}"
    print(usage)
    return usage


if __name__ == "__main__":
    init_logging()
    run_fact_gen()
