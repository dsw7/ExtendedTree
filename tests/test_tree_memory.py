from subprocess import run, DEVNULL, PIPE
from .base import TestTree, get_path_to_etree_binary


def run_valgrind(args: list[str]) -> None:
    root = [
        "valgrind",
        "--error-exitcode=2",
        "--leak-check=full",
        get_path_to_etree_binary(),
    ]

    process = run(root + args, stdout=DEVNULL, stderr=PIPE)
    stderr = process.stderr.decode()

    if process.returncode == 2:
        assert False, f"Found a memory leak\n\n{stderr}"


class TestMemory(TestTree):

    def test_help(self) -> None:
        run_valgrind(["-h"])

    def test_missing_dir(self) -> None:
        run_valgrind(["/foobar"])

    def test_traverse_file(self) -> None:
        run_valgrind([self.test_dir / "foo" / "a.txt"])

    def test_relative(self) -> None:
        run_valgrind([self.test_dir])

    def test_absolute(self) -> None:
        run_valgrind([self.test_dir, "-a"])

    def test_json_absolute(self) -> None:
        run_valgrind([self.test_dir, "-a", "-j -1"])

    def test_json_relative(self) -> None:
        run_valgrind([self.test_dir, "-j -1"])

    def test_dirs_only_absolute(self) -> None:
        run_valgrind([self.test_dir, "-da"])

    def test_dirs_only_relative(self) -> None:
        run_valgrind([self.test_dir, "-d"])
