import argparse
import datetime
import logging
import os
import resource
import shutil
import subprocess
import sys
import time
from pathlib import Path
from typing import Optional

from util import validate_path, ensure_dir, ErrorCode, CFG_PATHS, init_logging, write_logs

logger = logging.getLogger(__name__)


def run_translator():
    # Create the parser
    runner = argparse.ArgumentParser(description='This script automate the process of translating EvoMe to Souffle.')
    # Add the arguments
    runner.add_argument('--output_path', type=str, help='the path to the analysis output')
    runner.add_argument('--query_file_path', type=str, help='the path to the input query file')
    runner.add_argument('--evome_path', type=str,
                        help='the path to the EvoMe from query langauge to internal declarative langauge, '
                             'install here(https://github.com/LiMengyang990726/EvoMe/)')
    # Get all the input arguments and validate
    args = runner.parse_args()
    output_path = Path(args.output_path)
    query_file_path = Path(args.query_file_path)
    evome_path = Path(args.evome_path)
    for p in [output_path, query_file_path, evome_path]:
        validate_path(p)
    translate(output_path, query_file_path, evome_path)


def translate(output_path: Path, query_file_path: Path, evome_path: Path):
    # Execute
    start_time = time.time()
    create_dir_err: Optional[ErrorCode] = ensure_dir(evome_path / "translator/rules")
    if create_dir_err is not None:
        sys.exit(create_dir_err)
    stdout_log: Path = CFG_PATHS["logging"] / f"translate-{get_cur_time_str()}.log"
    stderr_log: Path = CFG_PATHS["logging"] / f"translate-{get_cur_time_str()}.err"
    logger.info(f"Start running translator, logs will be written to {stdout_log} and {stderr_log}")
    run_translate = subprocess.run(["./translator", query_file_path], cwd=evome_path / "translator", check=True)
    write_logs(run_translate.stdout.decode(), stdout_log)
    write_logs(run_translate.stderr.decode(), stderr_log)

    shutil.move(evome_path / "translator/rules", output_path / "rules")
    end_time = time.time()

    time_usage = end_time - start_time
    mem_usage = resource.getrusage(resource.RUSAGE_CHILDREN).ru_maxrss + resource.getrusage(
        resource.RUSAGE_SELF).ru_maxrss
    logger.info(f"Time usage:{time_usage}\nMem usage:{mem_usage}\n")


def get_cur_time_str() -> str:
    return str(datetime.datetime.now().isoformat()).replace(':', '-')


if __name__ == "__main__":
    init_logging()
    run_translator()
