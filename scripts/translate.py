import argparse
import logging
import resource
import shutil
import subprocess
import sys
import time
from pathlib import Path
from typing import Optional

from util import validate_path, ensure_dir, ErrorCode, CFG_PATHS, init_logging, write_logs, \
    get_cur_time_str

logger = logging.getLogger(__name__)


def run_translator():
    # Create the parser
    runner = argparse.ArgumentParser(
        description='This script automate the process of translating EvoMe to Souffle.')
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
    logger.info("Start running translator")
    run_translate = subprocess.run(["./translator", query_file_path], cwd=evome_path / "translator",
                                   check=True, capture_output=True)
    write_logs(run_translate.stdout.decode(), "Translator.out")
    write_logs(run_translate.stderr.decode(), "Translator.err", True)

    shutil.move(evome_path / "translator/rules", output_path / "rules")
    end_time = time.time()

    time_usage = end_time - start_time
    mem_usage = resource.getrusage(resource.RUSAGE_CHILDREN).ru_maxrss + resource.getrusage(
        resource.RUSAGE_SELF).ru_maxrss
    logger.info(f"Time usage:{time_usage}\tMem usage:{mem_usage}\n")


if __name__ == "__main__":
    init_logging()
    run_translator()
