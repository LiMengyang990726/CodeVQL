import logging
import os
import shlex
import shutil
import subprocess
import sys
from collections import namedtuple
from enum import IntEnum, unique
from pathlib import Path
from typing import Optional, Dict

logger = logging.getLogger(__name__)

RevPair = namedtuple("RevPair", ["rev_old", "rev_new"])


@unique
class ErrorCode(IntEnum):
    OTHERS = 1
    INVALID_CMDLINE = 2
    INVALID_CONF = 3
    USER_TERM = 4
    PATH_ERROR = 5
    BAD_REVISION = 6
    GIT_FAILURE = 7
    EXT_TOOL_NOT_AVAIL = 8
    MAVEN_BUILD_ERROR = 9
    NOT_IMPLEMENTED = 100


def print_cli_banner():
    # print name of the APP
    if not os.environ.get("NO_CLI_BANNER"):
        from pyfiglet import Figlet
        title = Figlet(font='slant')
        subtitle = Figlet(font='bubble')
        print(title.renderText('EvoMe'))
        print(subtitle.renderText('Version-Aware Security Analysis'))


def validate_path(path: Path):
    if not path.exists():
        logger.error(f'{path}: path specified does not exist')
        sys.exit(ErrorCode.PATH_ERROR)


def ensure_dir(path: Path) -> Optional[ErrorCode]:
    if not path.exists():
        if path.is_file():
            logger.error(f"{path} is a file, cannot mkdir.")
            return ErrorCode.PATH_ERROR
        else:
            logger.info(f"{path} does not exist, create dir now.")
    else:
        logger.info(f'Directory "{path}" already exists.')


def init_logging():
    rootlogger = logging.getLogger()
    log_level = os.environ.get("LOG_LEVEL")
    if log_level is None:
        log_level = "warning"
    numeric_level = getattr(logging, log_level.upper(), None)
    if not isinstance(numeric_level, int):
        raise ValueError('Invalid log level: %s' % log_level)
    rootlogger.setLevel(numeric_level)

    handler = logging.StreamHandler(sys.stderr)
    handler.setFormatter(ColorFormatter())
    rootlogger.addHandler(handler)


CFG_PATHS: Dict[str, Path] = {
    "logging": Path("/tmp/evome/logs")
}


class ColorFormatter(logging.Formatter):
    COLORS = {
        logging.DEBUG: "\033[96m",
        logging.INFO: "\033[92m",
        logging.WARNING: "\033[93m",
        logging.ERROR: "\033[91m",
        logging.CRITICAL: "\033[01;91m\033[47m",  # bold red on white background
        'RESET': "\033[0m"
    }

    def format(self, record):
        color = self.COLORS[record.levelno]
        color_reset = self.COLORS["RESET"]
        self.datefmt = "%m-%d %H:%M:%S"
        # self._style._fmt = color + '[%(asctime)s] [%(levelname)8s] [%(name)s > %(funcName)s()] ' + color_reset + '%(message)s'
        self._style._fmt = color + '[%(asctime)s] [%(levelname)8s] ' + color_reset + '%(message)s'
        return super().format(record)


def checkout_commit(repo_path: Path, commit: str, alt_dir=None) -> Optional[ErrorCode]:
    if " " in commit or "\t" in commit:
        return ErrorCode.BAD_REVISION
    if alt_dir:
        logger.info(f"Checkout {commit} @ {alt_dir}")
        worktree_add_cmd = f'git worktree add {alt_dir} {commit}'
        worktree_add = subprocess.run(shlex.split(worktree_add_cmd), cwd=repo_path, stderr=subprocess.PIPE)
        if worktree_add.stderr:
            logger.error(f"Error when: {worktree_add_cmd}")
            return ErrorCode.GIT_FAILURE
    else:
        logger.info(f"Checkout {commit} @ {repo_path}")
        checkout_cmd = f'git checkout {commit}'
        checkout = subprocess.run(shlex.split(checkout_cmd), cwd=repo_path)
        if checkout.stderr:
            logger.error(f"Error when: {checkout_cmd}")
            return ErrorCode.GIT_FAILURE


def mvn_build(build_root: Path) -> Optional[ErrorCode]:
    if not build_root.is_dir():
        logger.error(f"{build_root} is not a directory")
        return ErrorCode.PATH_ERROR
    if not shutil.which("mvn"):
        logger.error(f"Maven is not installed or not callable from this script")
        return ErrorCode.EXT_TOOL_NOT_AVAIL
    try:
        subprocess.run(["mvn" "test-compile"], cwd=build_root, check=True,
                       stdout=open(os.devnull), stderr=open(os.devnull))
    except subprocess.CalledProcessError:
        logger.error("Errors in mvn test-compile")
        return ErrorCode.MAVEN_BUILD_ERROR


def generate_config_file(repo_path: Path, build_path: Optional[Path], rev_pair: Optional[RevPair], out_file: Path):
    if not build_path:
        build_path = repo_path
    lines = [f"repoPath = {repo_path / '.git'}\n",
             f"classRoot = {build_path / 'target/classes'}\n"]
    if rev_pair:
        if rev_pair.rev_old:
            lines.append(f"startCommit = {rev_pair.rev_old}\n")
        if rev_pair.rev_new:
            lines.append(f"endCommit = {rev_pair.rev_new}\n")
        # lines.insert([f"startCommit = {rev_pair.rev_old}\n", f"endCommit = {rev_pair.rev_new}\n"])

    with out_file.open("w") as f_out:
        f_out.writelines(lines)
