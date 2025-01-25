from os import environ
from pathlib import Path
from shutil import rmtree
from subprocess import call
from tempfile import gettempdir
from unittest import TestCase


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


class TestSimpleMath(TestCase):

    def setUp(self) -> None:
        self.test_dir: Path = build_test_dir()
        self.bin: str = environ["PATH_BIN"]

    def tearDown(self) -> None:
        if self.test_dir.exists():
            rmtree(self.test_dir)

    def test_basic(self) -> None:
        exit_code = call([self.bin, self.test_dir, "-j -1"])
        self.assertEqual(exit_code, 0)
