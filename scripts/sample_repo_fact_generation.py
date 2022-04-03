import argparse
import logging
import os
import re
import resource
import shlex
import subprocess
import time
from pathlib import Path
from typing import Optional

from util import validate_path, write_logs

logger = logging.getLogger(__name__)


def main():
    # Create the parser
    runner = argparse.ArgumentParser(
        description='This script automate the process of translating EvoMe to Souffle.')

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
    runner.add_argument('--souffle_path',
                        type=str, required=False,
                        help='the path to Soufflé binary')

    # Get all the input arguments and validate
    args = runner.parse_args()

    repo_path = args.repo_path
    output_path = args.output_path
    evome_path = args.evome_path
    cslicer_path = args.cslicer_path
    program_fact_path = args.program_fact_path
    souffle_path: Optional[Path] = Path(args.souffle_path) if args.souffle_path else None

    for p in [repo_path, output_path, evome_path, cslicer_path, program_fact_path]:
        validate_path(p)


def sample_repo_fact_gen(output_path, program_fact_path, souffle_path: Optional[Path] = None):
    # Step 1: Execute version related souffle files to generate facts in the required sequence
    start_time = time.time()
    result_sequence = []
    regex_from = re.compile(r'SelectedVersion[\w]+from\.dl')
    for f in os.listdir(output_path / "rules"):
        file_name = output_path / "rules" / f
        if os.path.isfile(file_name) and regex_from.match(f):
            result_sequence.append(f)

    regex_base = re.compile(r'SelectedVersion[\w]+base\.dl')
    for f in os.listdir(output_path / "rules"):
        file_name = output_path / "rules" / f
        if os.path.isfile(file_name) and regex_base.match(f):
            result_sequence.append(f)

    regex_plain = re.compile(r'SelectedVersion[\w]+\.dl')
    for f in os.listdir(output_path / "rules"):
        file_name = output_path / "rules" / f
        if os.path.isfile(file_name) and regex_plain.match(f):
            result_sequence.append(f)

    result_sequence.append("Version.dl")
    logger.info(f"Queries for listing versions selected: {result_sequence}")
    # os.chdir(os.path.join(output_path, "rules"))

    souffle_bin = souffle_path if souffle_path else "souffle"
    for file_name in result_sequence:
        # command = "souffle -F " + os.path.join(output_path, ".facts") + " -D " + os.path.join(output_path, ".facts") + " " + file_name
        # os.system(command)
        rule_full_path = output_path / "rules" / file_name
        command = f"{souffle_bin} -F {output_path}/.facts -D {output_path}/.facts {rule_full_path}"
        run_souffle = subprocess.run(shlex.split(command), check=True, cwd=output_path / "rules",
                                     capture_output=True)
        write_logs(run_souffle.stderr.decode(), "Souffle.getVersions.err")
    for file_name in result_sequence:
        f = output_path / "rules" / file_name
        if os.path.isfile(f):
            os.remove(f)

    # Step 2: Copying over pre-generated program fact files
    os.system('cp -a %s/. %s/.facts' % (program_fact_path, output_path))
    # shutil.copytree(program_fact_path, output_path/".facts")

    end_time = time.time()
    time_usage = end_time - start_time
    mem_usage = resource.getrusage(resource.RUSAGE_CHILDREN).ru_maxrss + resource.getrusage(
        resource.RUSAGE_SELF).ru_maxrss
    logger.info(f"Time usage:{time_usage}\tMem usage:{mem_usage}\n")
