from json import loads
from typing import TypedDict, TypeAlias, Iterable
from subprocess import run, PIPE, DEVNULL
from .base import TestTree

Layers: TypeAlias = dict[int, list[int]]


class Tree(TypedDict):
    filename: str
    filesize: int
    children: Iterable["Tree"]


def traverse_level_order(stdout: Tree, layers: Layers, depth: int = 0) -> None:
    if depth not in layers:
        layers[depth] = []

    layers[depth].append(stdout["filesize"])

    depth += 1

    for child in stdout["children"]:
        traverse_level_order(child, layers, depth)


class TestCommandLine(TestTree):

    def test_help(self) -> None:
        process = run([self.bin, "-h"], stdout=PIPE)
        self.assertEqual(process.returncode, 0)
        self.assertIn("Usage:", process.stdout.decode())

    def test_missing_dir(self) -> None:
        process = run([self.bin, "/foobar"], stderr=PIPE)
        self.assertEqual(process.returncode, 1)
        self.assertIn("Error: Directory does not exist", process.stderr.decode())

    def test_traverse_file(self) -> None:
        process = run([self.bin, self.test_dir / "foo" / "a.txt"], stderr=PIPE)
        self.assertEqual(process.returncode, 1)
        self.assertIn("Error: Not a valid directory", process.stderr.decode())

    def test_relative(self) -> None:
        process = run([self.bin, self.test_dir], stdout=DEVNULL)
        self.assertEqual(process.returncode, 0)

    def test_absolute(self) -> None:
        process = run([self.bin, self.test_dir, "-a"], stdout=DEVNULL)
        self.assertEqual(process.returncode, 0)

    def test_json_absolute(self) -> None:
        process = run([self.bin, self.test_dir, "-a", "-j -1"], stdout=DEVNULL)
        self.assertEqual(process.returncode, 0)

    def test_json_relative(self) -> None:
        process = run([self.bin, self.test_dir, "-j -1"], stdout=DEVNULL)
        self.assertEqual(process.returncode, 0)

    def test_dirs_only_absolute(self) -> None:
        process = run([self.bin, self.test_dir, "-da"], stdout=DEVNULL)
        self.assertEqual(process.returncode, 0)

    def test_dirs_only_relative(self) -> None:
        process = run([self.bin, self.test_dir, "-d"], stdout=DEVNULL)
        self.assertEqual(process.returncode, 0)


class TestValidReporting(TestTree):

    def test_absolute(self) -> None:
        process = run([self.bin, self.test_dir, "-a", "-j -1"], stdout=PIPE)
        stdout: Tree = loads(process.stdout.decode())

        layers: Layers = {}
        traverse_level_order(stdout, layers)

        self.assertListEqual(layers[0], [27])
        self.assertListEqual(layers[1], [9, 9, 9])
        self.assertListEqual(layers[2], [3, 3, 3, 3, 3, 3, 3, 3, 3])

    def test_relative(self) -> None:
        process = run([self.bin, self.test_dir, "-j -1"], stdout=PIPE)
        stdout: Tree = loads(process.stdout.decode())

        layers: Layers = {}
        traverse_level_order(stdout, layers)

        self.assertListEqual(layers[0], [100.0])
        self.assertEqual(len(layers[1]), 3)
        self.assertEqual(len(layers[2]), 9)

        for i in range(3):
            self.assertAlmostEqual(layers[1][i], 33.3333, places=4)

        for i in range(9):
            self.assertAlmostEqual(layers[2][i], 11.1111, places=4)
