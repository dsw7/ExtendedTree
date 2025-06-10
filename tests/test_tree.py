from dataclasses import dataclass
from json import loads
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


class TestValidReporting(TestTree):

    def test_json(self) -> None:
        process = run_subprocess([self.test_dir, "-j -1"])
        self.assertEqual(process.exit_code, 0)

        json = loads(process.stdout)
        self.assertEqual(json["dirname"], "/tmp/etree_test")
        self.assertEqual(json["num_children"], 9)
        self.assertEqual(json["filesize"], 27)
        self.assertEqual(json["usage"], 100)

        self.assertEqual(len(json["children"]), 3)

        child = json["children"][0]
        self.assertIn(child["dirname"], {"foo", "bar", "baz"})
        self.assertEqual(child["num_children"], 3)
        self.assertEqual(child["filesize"], 9)
        self.assertAlmostEqual(child["usage"], 33.3333, places=4)

        self.assertEqual(len(child["children"]), 3)

        subchild = child["children"][0]
        self.assertIn(subchild["filename"], {"a.txt", "b.txt", "c.txt"})
        self.assertNotIn("num_children", subchild)
        self.assertEqual(subchild["filesize"], 3)
        self.assertAlmostEqual(subchild["usage"], 11.1111, places=4)

    def test_json_with_excludes(self) -> None:
        process = run_subprocess([self.test_dir, "-j -1", "-Ifoo", "-Ibar"])
        self.assertEqual(process.exit_code, 0)

        json = loads(process.stdout)
        self.assertEqual(json["dirname"], "/tmp/etree_test")
        self.assertEqual(json["num_children"], 9)
        self.assertEqual(json["filesize"], 27)
        self.assertEqual(json["usage"], 100)

        self.assertEqual(len(json["children"]), 1)

        child = json["children"][0]
        self.assertEqual(child["dirname"], "baz")
        self.assertEqual(child["num_children"], 3)
        self.assertEqual(child["filesize"], 9)
        self.assertAlmostEqual(child["usage"], 33.3333, places=4)

        self.assertEqual(len(child["children"]), 3)

        subchild = child["children"][0]
        self.assertIn(subchild["filename"], {"a.txt", "b.txt", "c.txt"})
        self.assertNotIn("num_children", subchild)
        self.assertEqual(subchild["filesize"], 3)
        self.assertAlmostEqual(subchild["usage"], 11.1111, places=4)

    def test_json_with_level(self) -> None:
        process = run_subprocess([self.test_dir, "-j -1", "-L1"])
        self.assertEqual(process.exit_code, 0)

        json = loads(process.stdout)
        self.assertEqual(json["dirname"], "/tmp/etree_test")
        self.assertEqual(json["num_children"], 9)
        self.assertEqual(json["filesize"], 27)
        self.assertEqual(json["usage"], 100)

        self.assertEqual(len(json["children"]), 3)

        child = json["children"][0]
        self.assertIn(child["dirname"], {"foo", "bar", "baz"})
        self.assertEqual(child["num_children"], 3)
        self.assertEqual(child["filesize"], 9)
        self.assertAlmostEqual(child["usage"], 33.3333, places=4)

        self.assertEqual(len(child["children"]), 0)
