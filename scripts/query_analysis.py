import argparse
import resource
import time

from util import validate_path, ensure_dir


def run_souffle_analysis():
    # Create the parser
    runner = argparse.ArgumentParser(description='This script automate the process of analyzing the final query.')

    # Add the arguments
    runner.add_argument('--output_path',
                        type=str,
                        help='the path to the analysis output')

    # Get all the input arguments and validate
    args = runner.parse_args()

    output_path = args.output_path
    validate_path(output_path)


def souffle_analysis(out_path):
    # Execute
    start_time = time.time()
    # os.system('mkdir %s/output && souffle -F %s/.facts/ -D %s/output %s/rules/main.dl'
    # % (output_path, output_path, output_path, output_path))
    ensure_dir(out_path / "output")
    souffle_cmd = f"souffle -F {out_path}/.facts -D {out_path}/output {out_path}/rules/main.dl"
    end_time = time.time()
    time_usage = end_time - start_time
    mem_usage = resource.getrusage(resource.RUSAGE_CHILDREN).ru_maxrss + resource.getrusage(
        resource.RUSAGE_SELF).ru_maxrss
    print(str(time_usage) + "\t" + str(mem_usage))
