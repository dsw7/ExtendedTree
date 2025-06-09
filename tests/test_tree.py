from dataclasses import dataclass
from json import loads
from typing import TypedDict, Iterable
from subprocess import run, PIPE
from .base import TestTree, get_path_to_etree_binary


class Layers(TypedDict):
    filesize: dict[int, list[int]]
    usage: dict[int, list[float]]


class Tree(TypedDict):
    children: Iterable["Tree"]
    filename: str
    filesize: int
    usage: float


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


def traverse_level_order(stdout: Tree, layers: Layers, depth: int = 0) -> None:
    if depth not in layers["filesize"]:
        layers["filesize"][depth] = []

    if depth not in layers["usage"]:
        layers["usage"][depth] = []

    layers["filesize"][depth].append(stdout["filesize"])
    layers["usage"][depth].append(stdout["usage"])

    depth += 1

    for child in stdout["children"]:
        traverse_level_order(child, layers, depth)


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
        self.assertRegex(process.stdout, r"foo/.*\[ 9 bytes, 33\.33%, 3 \]")
        self.assertRegex(process.stdout, r"a.txt.*\[ 3 bytes, 11\.11% \]")

    def test_human_readable(self) -> None:
        process = run_subprocess([self.test_dir, "-b"])
        self.assertEqual(process.exit_code, 0)
        self.assertRegex(process.stdout, r"foo/.*\[ 9, 33\.33%, 3 \]")
        self.assertRegex(process.stdout, r"bar/.*\[ 9, 33\.33%, 3 \]")
        self.assertRegex(process.stdout, r"baz/.*\[ 9, 33\.33%, 3 \]")
        self.assertRegex(process.stdout, r"(a\.txt|b\.txt|c\.txt)")

    def test_exclude(self) -> None:
        process = run_subprocess([self.test_dir, "-Ifoo"])
        self.assertEqual(process.exit_code, 0)
        self.assertNotRegex(process.stdout, "foo")
        self.assertRegex(process.stdout, r"bar/.*\[ 9 bytes, 33\.33%, 3 \]")
        self.assertRegex(process.stdout, r"baz/.*\[ 9 bytes, 33\.33%, 3 \]")

    def test_dirs_only(self) -> None:
        process = run_subprocess([self.test_dir, "-d"])
        self.assertEqual(process.exit_code, 0)
        self.assertRegex(process.stdout, r"foo/.*\[ 9 bytes, 33\.33%, 3 \]")
        self.assertRegex(process.stdout, r"bar/.*\[ 9 bytes, 33\.33%, 3 \]")
        self.assertRegex(process.stdout, r"baz/.*\[ 9 bytes, 33\.33%, 3 \]")
        self.assertNotRegex(process.stdout, r"(a\.txt|b\.txt|c\.txt)")

    def test_dirs_only_exclude(self) -> None:
        process = run_subprocess([self.test_dir, "-d", "-Ifoo"])
        self.assertEqual(process.exit_code, 0)
        self.assertNotRegex(process.stdout, "foo")
        self.assertRegex(process.stdout, r"bar/.*\[ 9 bytes, 33\.33%, 3 \]")
        self.assertRegex(process.stdout, r"baz/.*\[ 9 bytes, 33\.33%, 3 \]")
        self.assertNotRegex(process.stdout, r"(a\.txt|b\.txt|c\.txt)")

    def test_dirs_only_human_readable(self) -> None:
        process = run_subprocess([self.test_dir, "-db"])
        self.assertEqual(process.exit_code, 0)
        self.assertRegex(process.stdout, r"foo/.*\[ 9, 33\.33%, 3 \]")
        self.assertRegex(process.stdout, r"bar/.*\[ 9, 33\.33%, 3 \]")
        self.assertRegex(process.stdout, r"baz/.*\[ 9, 33\.33%, 3 \]")
        self.assertNotRegex(process.stdout, r"(a\.txt|b\.txt|c\.txt)")

    def test_level(self) -> None:
        process = run_subprocess([self.test_dir, "-L1"])
        self.assertEqual(process.exit_code, 0)
        self.assertRegex(process.stdout, r"foo/.*\[ 9 bytes, 33\.33%, 3 \]")
        self.assertRegex(process.stdout, r"bar/.*\[ 9 bytes, 33\.33%, 3 \]")
        self.assertRegex(process.stdout, r"baz/.*\[ 9 bytes, 33\.33%, 3 \]")
        self.assertNotRegex(process.stdout, r"(a\.txt|b\.txt|c\.txt)")

    def test_level_human_readable(self) -> None:
        process = run_subprocess([self.test_dir, "-L1", "-b"])
        self.assertEqual(process.exit_code, 0)
        self.assertRegex(process.stdout, r"foo/.*\[ 9, 33\.33%, 3 \]")
        self.assertRegex(process.stdout, r"bar/.*\[ 9, 33\.33%, 3 \]")
        self.assertRegex(process.stdout, r"baz/.*\[ 9, 33\.33%, 3 \]")
        self.assertNotRegex(process.stdout, r"(a\.txt|b\.txt|c\.txt)")

    def test_level_human_readable_exclude(self) -> None:
        process = run_subprocess([self.test_dir, "-L1", "-b", "-Ifoo"])
        self.assertEqual(process.exit_code, 0)
        self.assertNotRegex(process.stdout, "foo")
        self.assertNotRegex(process.stdout, r"(a\.txt|b\.txt|c\.txt)")
        self.assertRegex(process.stdout, r"bar/.*\[ 9, 33\.33%, 3 \]")
        self.assertRegex(process.stdout, r"baz/.*\[ 9, 33\.33%, 3 \]")

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
        self.assertEqual(json["filecount"], 9)
        self.assertEqual(json["filesize"], 27)
        self.assertEqual(json["usage"], 100)

        self.assertEqual(len(json["children"]), 3)

        child = json["children"][0]
        self.assertIn(child["dirname"], {"foo", "bar", "baz"})
        self.assertEqual(child["filecount"], 3)
        self.assertEqual(child["filesize"], 9)
        self.assertAlmostEqual(child["usage"], 33.3333, places=4)

        self.assertEqual(len(child["children"]), 3)

        subchild = child["children"][0]
        self.assertIn(subchild["filename"], {"a.txt", "b.txt", "c.txt"})
        self.assertNotIn("filecount", subchild)
        self.assertEqual(subchild["filesize"], 3)
        self.assertAlmostEqual(subchild["usage"], 11.1111, places=4)

    def test_filesizes_with_exclude(self) -> None:
        process = run_subprocess([self.test_dir, "-j -1", "-Ifoo"])
        self.assertEqual(process.exit_code, 0)

        stdout: Tree = loads(process.stdout)
        layers = Layers(filesize={}, usage={})
        traverse_level_order(stdout, layers)

        self.assertDictEqual(
            layers["filesize"], {0: [27], 1: [9, 9], 2: [3, 3, 3, 3, 3, 3]}
        )

    def test_filesizes_with_exclude_2(self) -> None:
        process = run_subprocess([self.test_dir, "-j -1", "-Ifoo", "-Ibar"])
        self.assertEqual(process.exit_code, 0)

        stdout: Tree = loads(process.stdout)
        layers = Layers(filesize={}, usage={})
        traverse_level_order(stdout, layers)

        self.assertDictEqual(layers["filesize"], {0: [27], 1: [9], 2: [3, 3, 3]})

    def test_level(self) -> None:
        process = run_subprocess([self.test_dir, "-j -1", "-L1"])
        self.assertEqual(process.exit_code, 0)

        stdout: Tree = loads(process.stdout)
        layers = Layers(filesize={}, usage={})
        traverse_level_order(stdout, layers)

        self.assertDictEqual(layers["filesize"], {0: [27], 1: [9, 9, 9]})

    def test_usages(self) -> None:
        process = run_subprocess([self.test_dir, "-j -1"])
        self.assertEqual(process.exit_code, 0)

        stdout: Tree = loads(process.stdout)
        layers = Layers(filesize={}, usage={})
        traverse_level_order(stdout, layers)

        usages = layers["usage"]
        self.assertListEqual(usages[0], [100.0])
        self.assertEqual(len(usages[1]), 3)
        self.assertEqual(len(usages[2]), 9)

        for i in range(3):
            self.assertAlmostEqual(usages[1][i], 33.3333, places=4)

        for i in range(9):
            self.assertAlmostEqual(usages[2][i], 11.1111, places=4)

    def test_usages_with_exclude(self) -> None:
        process = run_subprocess([self.test_dir, "-j -1", "-Ifoo"])
        self.assertEqual(process.exit_code, 0)

        stdout: Tree = loads(process.stdout)
        layers = Layers(filesize={}, usage={})
        traverse_level_order(stdout, layers)

        usages = layers["usage"]
        self.assertListEqual(usages[0], [100.0])
        self.assertEqual(len(usages[1]), 2)
        self.assertEqual(len(usages[2]), 6)

        for i in range(2):
            self.assertAlmostEqual(usages[1][i], 33.3333, places=4)

        for i in range(6):
            self.assertAlmostEqual(usages[2][i], 11.1111, places=4)

    def test_usages_with_exclude_2(self) -> None:
        process = run_subprocess([self.test_dir, "-j -1", "-Ifoo", "-Ibar"])
        self.assertEqual(process.exit_code, 0)

        stdout: Tree = loads(process.stdout)
        layers = Layers(filesize={}, usage={})
        traverse_level_order(stdout, layers)

        usages = layers["usage"]
        self.assertListEqual(usages[0], [100.0])
        self.assertEqual(len(usages[1]), 1)
        self.assertEqual(len(usages[2]), 3)

        self.assertAlmostEqual(usages[1][0], 33.3333, places=4)

        for i in range(3):
            self.assertAlmostEqual(usages[2][i], 11.1111, places=4)
