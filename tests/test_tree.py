from json import loads
from typing import TypedDict, TypeAlias, Iterable
from os import environ
from pathlib import Path
from shutil import rmtree
from subprocess import run, PIPE
from tempfile import gettempdir
from unittest import TestCase

Layers: TypeAlias = dict[int, list[int]]


class Tree(TypedDict):
    filename: str
    filesize: int
    children: Iterable["Tree"]


def build_test_dir() -> Path:
    test_dir = Path(gettempdir()) / "etree_test"
    test_dir.mkdir(exist_ok=True)

    dir_a = test_dir / "foo"
    dir_b = test_dir / "bar"
    dir_c = test_dir / "baz"

    dir_a.mkdir(exist_ok=True)
    dir_b.mkdir(exist_ok=True)
    dir_c.mkdir(exist_ok=True)

    (dir_a / "a.txt").write_text("abc")
    (dir_a / "b.txt").write_text("abc")
    (dir_a / "c.txt").write_text("abc")
    (dir_b / "a.txt").write_text("abc")
    (dir_b / "b.txt").write_text("abc")
    (dir_b / "c.txt").write_text("abc")
    (dir_c / "a.txt").write_text("abc")
    (dir_c / "b.txt").write_text("abc")
    (dir_c / "c.txt").write_text("abc")

    return test_dir


def traverse_level_order(stdout: Tree, layers: Layers, depth: int = 0) -> None:
    if depth not in layers:
        layers[depth] = []

    layers[depth].append(stdout["filesize"])

    depth += 1

    for child in stdout["children"]:
        traverse_level_order(child, layers, depth)


class TestAbsolute(TestCase):

    def setUp(self) -> None:
        self.test_dir: Path = build_test_dir()
        self.bin: str = environ["PATH_BIN"]

    def tearDown(self) -> None:
        if self.test_dir.exists():
            rmtree(self.test_dir)

    def test_basic(self) -> None:
        process = run([self.bin, self.test_dir, "-j -1"], stdout=PIPE)
        self.assertEqual(process.returncode, 0)

        stdout: Tree = loads(process.stdout.decode())
        layers: Layers = {}
        traverse_level_order(stdout, layers)

        self.assertListEqual(layers[0], [27])
        self.assertListEqual(layers[1], [9, 9, 9])
        self.assertListEqual(layers[2], [3, 3, 3, 3, 3, 3, 3, 3, 3])
