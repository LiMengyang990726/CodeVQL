import argparse
import logging
import resource
import shlex
import shutil
import subprocess
import time
from pathlib import Path
from typing import Optional

from util import validate_path, ensure_dir, write_logs

logger = logging.getLogger(__name__)


def run_souffle_analysis():
    # Create the parser
    runner = argparse.ArgumentParser(
        description='This script automate the process of analyzing the final query.')

    # Add the arguments
    runner.add_argument('--output_path',
                        type=str,
                        help='the path to the analysis output')

    # Get all the input arguments and validate
    args = runner.parse_args()

    output_path = args.output_path
    souffle_path: Optional[Path] = Path(args.souffle_path) if args.souffle_path else None
    validate_path(output_path)
    souffle_analysis(output_path, souffle_path)


def souffle_analysis(out_path, souffle_path: Optional[Path] = None):
    # Execute
    start_time = time.time()
    # os.system('mkdir %s/output && souffle -F %s/.facts/ -D %s/output %s/rules/main.dl'
    # % (output_path, output_path, output_path, output_path))
    ensure_dir(out_path / "output")
    souffle_bin = souffle_path if souffle_path else "souffle"
    souffle_cmd = f"{souffle_bin} -F {out_path}/.facts -D {out_path}/output {out_path}/rules/main.dl"
    logger.info(f"Running souffle: {souffle_cmd}")
    run_souffle = subprocess.run(shlex.split(souffle_cmd), check=True, capture_output=True)
    write_logs(run_souffle.stderr.decode(), "Analysis.err", True)

    end_time = time.time()
    time_usage = end_time - start_time
    mem_usage = resource.getrusage(resource.RUSAGE_CHILDREN).ru_maxrss + resource.getrusage(
        resource.RUSAGE_SELF).ru_maxrss
    logger.info(f"Time usage:{time_usage}\tMem usage:{mem_usage}\n")
