import argparse
import logging
from pathlib import Path
from typing import Optional

from diff_fact_generation import diff_fact_gen
from gitfact_generation import git_fact_gen
from multiversion_fact_generation import multi_ver_fact_gen
from query_analysis import souffle_analysis
from translate import translate
from util import print_cli_banner, ensure_dir, validate_path, init_logging

logger = logging.getLogger(__name__)


def main():
    print_cli_banner()

    # Create the parser
    runner = argparse.ArgumentParser(description='\
                This is the Command Line Interface(CLI) of the EvoMe language. \
                EvoMe can perform the basic security analysis in a version-aware way. \
                For more information, please check at this link [TODO]. \
            ')

    # Add the arguments
    runner.add_argument('--repo_path',
                        type=str,
                        help='the path to the program to be analyzed upon')
    runner.add_argument('--gitfacts_path',
                        type=str,
                        help='the path to the tool that generates git facts, install here(https://github.com/uxhg/ext-gitfacts)')
    runner.add_argument('--output_path',
                        type=str,
                        help='the path to the analysis output')
    runner.add_argument('--query_file_path',
                        type=str,
                        help='the path to the input query file')
    runner.add_argument('--evome_path',
                        type=str,
                        help='the path to the EvoMe from query langauge to internal declarative langauge, install here(https://github.com/LiMengyang990726/EvoMe/)')
    runner.add_argument('--cslicer_path',
                        type=str,
                        help='the path to the tool that generates program facts, install here(https://bitbucket.org/liyistc/gitslice/src/facts-dl4ql/)')
    runner.add_argument('--souffle_path',
                        type=str, required=False,
                        help='the path to Soufflé binary')

    # Get all the input arguments and validate
    args = runner.parse_args()
    repo_path = Path(args.repo_path)
    gitfacts_path = Path(args.gitfacts_path)
    output_path = Path(args.output_path)
    ensure_dir(output_path)
    query_file_path = Path(args.query_file_path)
    evome_path = Path(args.evome_path)
    cslicer_path = Path(args.cslicer_path)
    souffle_path: Optional[Path] = Path(args.souffle_path) if args.souffle_path else None
    for p in [repo_path, gitfacts_path, output_path, query_file_path, evome_path, cslicer_path]:
        validate_path(p)

    # Step 1: Generate Git Facts
    logger.info("Step 1: start generating git facts")
    git_fact_gen(repo_path, gitfacts_path, output_path)
    # "python3.7 %s/scripts/gitfact-generation.py --repo_path %s --gitfacts_path %s --output_path %s && \
    # %(evome_path, repo_path, gitfacts_path, output_path)
    logger.info("Step 1 is done.")

    # Step 2: Translate EvoMe to Souffle
    logger.info("Step 2: Start translating EvoMe to Souffle")
    translate(output_path, query_file_path, evome_path)
    # python3.7 %s/scripts/translate.py --query_file_path %s --evome_path %s --output_path %s && \
    # % (evome_path, query_file_path, evome_path, output_path)
    logger.info("Step 2 is done.")

    # Step 3: Get multiversion facts
    logger.info("Step 3: Start Generating program facts in selected version")
    multi_ver_fact_gen(repo_path, output_path, cslicer_path, souffle_path)
    # python3.7 %s/scripts/multiversion-fact-generation.py --repo_path %s --cslicer_path %s --evome_path %s --output_path %s && \
    # Step 3 is done!' % (evome_path, repo_path, cslicer_path, evome_path, output_path)
    logger.info("Step 3 is done.")

    # Step 4: Get diff facts
    logger.info("Step 4: Start Generating diff facts")
    diff_fact_gen(cslicer_path, output_path, repo_path)
    logger.info("Step 4 is done.")
    # python3.7 %s/scripts/diff-fact-generation.py --repo_path %s --cslicer_path %s --output_path %s && \
    # echo Step 4 is done!' % (evome_path, repo_path, cslicer_path, output_path)

    # Step 5: Analyze to get the result
    logger.info("Step 5: Start analyzing the query")
    souffle_analysis(output_path, souffle_path)
    logger.info("Step 5 is done.")
    # python3.7 %s/scripts/query-analysis.py --output_path %s && \
    # echo Step 5 is done and program finished!' % (evome_path, output_path))


if __name__ == "__main__":
    init_logging()
    main()
