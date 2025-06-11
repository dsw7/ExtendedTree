from dataclasses import dataclass
from json import loads
from pathlib import Path
from subprocess import run, PIPE
from .base import TestTree, get_path_to_etree_binary


@dataclass
class Process:
    exit_code: int
    stdout: str
    stderr: str


def run_subprocess(args: list[str]) -> Process:
    command = [get_path_to_etree_binary()] + args
    process = run(command, stdout=PIPE, stderr=PIPE)

    return Process(
        exit_code=process.returncode,
        stdout=process.stdout.decode(),
        stderr=process.stderr.decode(),
    )


class TestCommandLine(TestTree):

    def test_help(self) -> None:
        process = run_subprocess(["-h"])
        self.assertEqual(process.exit_code, 0)
        self.assertIn("Usage:", process.stdout)

    def test_missing_dir(self) -> None:
        process = run_subprocess(["/foobar"])
        self.assertEqual(process.exit_code, 1)
        self.assertIn("Error: Directory does not exist", process.stderr)

    def test_traverse_file(self) -> None:
        process = run_subprocess([self.test_dir / "foo" / "a.txt"])
        self.assertEqual(process.exit_code, 1)
        self.assertIn("Error: Not a valid directory", process.stderr)

    def test_default(self) -> None:
        process = run_subprocess([self.test_dir])
        self.assertEqual(process.exit_code, 0)
        self.assertRegex(process.stdout, r"foo/.*\[ 16.00 KiB, 30\.77%, 3 \]")
        self.assertRegex(process.stdout, r"a.txt.*\[ 4.00 KiB, 7\.69% \]")

    def test_print_bytes(self) -> None:
        process = run_subprocess([self.test_dir, "-b"])
        self.assertEqual(process.exit_code, 0)
        self.assertRegex(process.stdout, r"foo/.*\[ 16384 B, 30\.77%, 3 \]")
        self.assertRegex(process.stdout, r"bar/.*\[ 16384 B, 30\.77%, 3 \]")
        self.assertRegex(process.stdout, r"baz/.*\[ 16384 B, 30\.77%, 3 \]")
        self.assertRegex(process.stdout, r"(a\.txt|b\.txt|c\.txt)")

    def test_exclude(self) -> None:
        process = run_subprocess([self.test_dir, "-Ifoo"])
        self.assertEqual(process.exit_code, 0)
        self.assertNotRegex(process.stdout, "foo")
        self.assertRegex(process.stdout, r"bar/.*\[ 16.00 KiB, 30\.77%, 3 \]")
        self.assertRegex(process.stdout, r"baz/.*\[ 16.00 KiB, 30\.77%, 3 \]")

    def test_dirs_only(self) -> None:
        process = run_subprocess([self.test_dir, "-d"])
        self.assertEqual(process.exit_code, 0)
        self.assertRegex(process.stdout, r"foo/.*\[ 16.00 KiB, 30\.77%, 3 \]")
        self.assertRegex(process.stdout, r"bar/.*\[ 16.00 KiB, 30\.77%, 3 \]")
        self.assertRegex(process.stdout, r"baz/.*\[ 16.00 KiB, 30\.77%, 3 \]")
        self.assertNotRegex(process.stdout, r"(a\.txt|b\.txt|c\.txt)")

    def test_dirs_only_exclude(self) -> None:
        process = run_subprocess([self.test_dir, "-d", "-Ifoo"])
        self.assertEqual(process.exit_code, 0)
        self.assertNotRegex(process.stdout, "foo")
        self.assertRegex(process.stdout, r"bar/.*\[ 16.00 KiB, 30\.77%, 3 \]")
        self.assertRegex(process.stdout, r"baz/.*\[ 16.00 KiB, 30\.77%, 3 \]")
        self.assertNotRegex(process.stdout, r"(a\.txt|b\.txt|c\.txt)")

    def test_dirs_only_print_bytes(self) -> None:
        process = run_subprocess([self.test_dir, "-db"])
        self.assertEqual(process.exit_code, 0)
        self.assertRegex(process.stdout, r"foo/.*\[ 16384 B, 30\.77%, 3 \]")
        self.assertRegex(process.stdout, r"bar/.*\[ 16384 B, 30\.77%, 3 \]")
        self.assertRegex(process.stdout, r"baz/.*\[ 16384 B, 30\.77%, 3 \]")
        self.assertNotRegex(process.stdout, r"(a\.txt|b\.txt|c\.txt)")

    def test_level(self) -> None:
        process = run_subprocess([self.test_dir, "-L1"])
        self.assertEqual(process.exit_code, 0)
        self.assertRegex(process.stdout, r"foo/.*\[ 16.00 KiB, 30\.77%, 3 \]")
        self.assertRegex(process.stdout, r"bar/.*\[ 16.00 KiB, 30\.77%, 3 \]")
        self.assertRegex(process.stdout, r"baz/.*\[ 16.00 KiB, 30\.77%, 3 \]")
        self.assertNotRegex(process.stdout, r"(a\.txt|b\.txt|c\.txt)")

    def test_level_print_bytes(self) -> None:
        process = run_subprocess([self.test_dir, "-L1", "-b"])
        self.assertEqual(process.exit_code, 0)
        self.assertRegex(process.stdout, r"foo/.*\[ 16384 B, 30\.77%, 3 \]")
        self.assertRegex(process.stdout, r"bar/.*\[ 16384 B, 30\.77%, 3 \]")
        self.assertRegex(process.stdout, r"baz/.*\[ 16384 B, 30\.77%, 3 \]")
        self.assertNotRegex(process.stdout, r"(a\.txt|b\.txt|c\.txt)")

    def test_level_print_bytes_exclude(self) -> None:
        process = run_subprocess([self.test_dir, "-L1", "-b", "-Ifoo"])
        self.assertEqual(process.exit_code, 0)
        self.assertNotRegex(process.stdout, "foo")
        self.assertNotRegex(process.stdout, r"(a\.txt|b\.txt|c\.txt)")
        self.assertRegex(process.stdout, r"bar/.*\[ 16384 B, 30\.77%, 3 \]")
        self.assertRegex(process.stdout, r"baz/.*\[ 16384 B, 30\.77%, 3 \]")

    def test_exclude_multiple(self) -> None:
        process = run_subprocess([self.test_dir, "-Ifoo", "-Ibar", "-Ibaz"])
        self.assertEqual(process.exit_code, 0)
        self.assertNotRegex(process.stdout, r"(foo|bar|baz)")


class TestJsonOutput(TestTree):

    def test_json(self) -> None:
        process = run_subprocess([self.test_dir, "-j -1"])
        self.assertEqual(process.exit_code, 0)

        json = loads(process.stdout)
        test_file = Path(__file__).parent / "data" / "test_json.json"
        json_expected = loads(test_file.read_text())
        self.assertDictEqual(json, json_expected)

    def test_json_with_excludes(self) -> None:
        process = run_subprocess([self.test_dir, "-j -1", "-Ifoo", "-Ibar"])
        self.assertEqual(process.exit_code, 0)

        json = loads(process.stdout)
        test_file = Path(__file__).parent / "data" / "test_json_with_excludes.json"
        json_expected = loads(test_file.read_text())
        self.assertDictEqual(json, json_expected)

    def test_json_with_level(self) -> None:
        process = run_subprocess([self.test_dir, "-j -1", "-L1"])
        self.assertEqual(process.exit_code, 0)

        json = loads(process.stdout)
        test_file = Path(__file__).parent / "data" / "test_json_with_level.json"
        json_expected = loads(test_file.read_text())
        self.assertDictEqual(json, json_expected)
