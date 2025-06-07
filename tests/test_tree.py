from json import loads
from typing import TypedDict, Iterable
from subprocess import run, PIPE, CompletedProcess
from .base import TestTree, get_path_to_etree_binary


class Layers(TypedDict):
    filesize: dict[int, list[int]]
    usage: dict[int, list[float]]


class Tree(TypedDict):
    children: Iterable["Tree"]
    filename: str
    filesize: int
    usage: float


def run_subprocess(args: list[str]) -> CompletedProcess[bytes]:
    command = [get_path_to_etree_binary()] + args
    return run(command, stdout=PIPE, stderr=PIPE)


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
        self.assertEqual(process.returncode, 0)
        self.assertIn("Usage:", process.stdout.decode())

    def test_missing_dir(self) -> None:
        process = run_subprocess(["/foobar"])
        self.assertEqual(process.returncode, 1)
        self.assertIn("Error: Directory does not exist", process.stderr.decode())

    def test_traverse_file(self) -> None:
        process = run_subprocess([self.test_dir / "foo" / "a.txt"])
        self.assertEqual(process.returncode, 1)
        self.assertIn("Error: Not a valid directory", process.stderr.decode())

    def test_default(self) -> None:
        process = run_subprocess([self.test_dir])
        self.assertEqual(process.returncode, 0)

        stdout = process.stdout.decode()
        self.assertRegex(stdout, "foo/.*[ 9 bytes, 33.33% ]")
        self.assertRegex(stdout, "a.txt.*[ 3 bytes, 11.11% ]")

    def test_human_readable(self) -> None:
        process = run_subprocess([self.test_dir, "-b"])
        self.assertEqual(process.returncode, 0)

        stdout = process.stdout.decode()
        self.assertRegex(stdout, "foo/.*[ 9, 33.33% ]")
        self.assertRegex(stdout, "bar/.*[ 9, 33.33% ]")
        self.assertRegex(stdout, "baz/.*[ 9, 33.33% ]")

    def test_exclude(self) -> None:
        process = run_subprocess([self.test_dir, "-Ifoo"])
        self.assertEqual(process.returncode, 0)

        stdout = process.stdout.decode()
        self.assertNotRegex(stdout, "foo/.*[ 9, 33.33% ]")
        self.assertRegex(stdout, "bar/.*[ 9, 33.33% ]")
        self.assertRegex(stdout, "baz/.*[ 9, 33.33% ]")

    def test_dirs_only(self) -> None:
        process = run_subprocess([self.test_dir, "-d"])
        self.assertEqual(process.returncode, 0)

        stdout = process.stdout.decode()
        self.assertRegex(stdout, "foo/.*[ 9, 33.33% ]")
        self.assertRegex(stdout, "bar/.*[ 9, 33.33% ]")
        self.assertRegex(stdout, "baz/.*[ 9, 33.33% ]")
        self.assertNotRegex(stdout, "a.txt.*[ 3, 11.11% ]")
        self.assertNotRegex(stdout, "b.txt.*[ 3, 11.11% ]")
        self.assertNotRegex(stdout, "c.txt.*[ 3, 11.11% ]")

    def test_dirs_only_exclude(self) -> None:
        process = run_subprocess([self.test_dir, "-d", "-Ifoo"])
        self.assertEqual(process.returncode, 0)

        stdout = process.stdout.decode()
        self.assertNotRegex(stdout, "foo/.*[ 9, 33.33% ]")
        self.assertRegex(stdout, "bar/.*[ 9, 33.33% ]")
        self.assertRegex(stdout, "baz/.*[ 9, 33.33% ]")
        self.assertNotRegex(stdout, "a.txt.*[ 3, 11.11% ]")
        self.assertNotRegex(stdout, "b.txt.*[ 3, 11.11% ]")
        self.assertNotRegex(stdout, "c.txt.*[ 3, 11.11% ]")

    def test_dirs_only_human_readable(self) -> None:
        process = run_subprocess([self.test_dir, "-db"])
        self.assertEqual(process.returncode, 0)


class TestValidReporting(TestTree):

    def test_filesizes(self) -> None:
        process = run_subprocess([self.test_dir, "-j -1"])
        self.assertEqual(process.returncode, 0)

        stdout: Tree = loads(process.stdout.decode())
        layers = Layers(filesize={}, usage={})
        traverse_level_order(stdout, layers)

        filesizes = layers["filesize"]
        self.assertListEqual(filesizes[0], [27])
        self.assertListEqual(filesizes[1], [9, 9, 9])
        self.assertListEqual(filesizes[2], [3, 3, 3, 3, 3, 3, 3, 3, 3])

    def test_usages(self) -> None:
        process = run_subprocess([self.test_dir, "-j -1"])
        self.assertEqual(process.returncode, 0)

        stdout: Tree = loads(process.stdout.decode())
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

    def test_filesizes_with_exclude(self) -> None:
        process = run_subprocess([self.test_dir, "-j -1", "-Ifoo"])
        self.assertEqual(process.returncode, 0)

        stdout: Tree = loads(process.stdout.decode())
        layers = Layers(filesize={}, usage={})
        traverse_level_order(stdout, layers)

        filesizes = layers["filesize"]
        self.assertListEqual(filesizes[0], [18])
        self.assertListEqual(filesizes[1], [9, 9])
        self.assertListEqual(filesizes[2], [3, 3, 3, 3, 3, 3])

    def test_usages_with_exclude(self) -> None:
        process = run_subprocess([self.test_dir, "-j -1", "-Ifoo"])
        self.assertEqual(process.returncode, 0)

        stdout: Tree = loads(process.stdout.decode())
        layers = Layers(filesize={}, usage={})
        traverse_level_order(stdout, layers)

        usages = layers["usage"]
        self.assertListEqual(usages[0], [100.0])
        self.assertEqual(len(usages[1]), 2)
        self.assertEqual(len(usages[2]), 6)

        for i in range(2):
            self.assertAlmostEqual(usages[1][i], 50.0, places=4)

        for i in range(6):
            self.assertAlmostEqual(usages[2][i], 16.6667, places=4)

    def test_filesizes_with_exclude_2(self) -> None:
        process = run_subprocess([self.test_dir, "-j -1", "-Ifoo", "-Ibar"])
        self.assertEqual(process.returncode, 0)

        stdout: Tree = loads(process.stdout.decode())
        layers = Layers(filesize={}, usage={})
        traverse_level_order(stdout, layers)

        filesizes = layers["filesize"]
        self.assertListEqual(filesizes[0], [9])
        self.assertListEqual(filesizes[1], [9])
        self.assertListEqual(filesizes[2], [3, 3, 3])

    def test_usages_with_exclude_2(self) -> None:
        process = run_subprocess([self.test_dir, "-j -1", "-Ifoo", "-Ibar"])
        self.assertEqual(process.returncode, 0)

        stdout: Tree = loads(process.stdout.decode())
        layers = Layers(filesize={}, usage={})
        traverse_level_order(stdout, layers)

        usages = layers["usage"]
        self.assertListEqual(usages[0], [100.0])
        self.assertEqual(len(usages[1]), 1)
        self.assertEqual(len(usages[2]), 3)

        for i in range(1):
            self.assertAlmostEqual(usages[1][i], 100.0, places=4)

        for i in range(3):
            self.assertAlmostEqual(usages[2][i], 33.3333, places=4)
