from functools import cache
from os import environ, statvfs
from pathlib import Path
from shutil import rmtree
from tempfile import gettempdir
from unittest import TestCase


@cache
def get_path_to_etree_binary() -> str:
    return environ["PATH_BIN"]


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


class TestTree(TestCase):

    @classmethod
    def setUpClass(cls) -> None:
        cls.test_dir: Path = build_test_dir()

    @classmethod
    def tearDownClass(cls) -> None:
        if cls.test_dir.exists():
            rmtree(cls.test_dir)

    def get_block_size(self) -> int:
        statvfs_info = statvfs(self.test_dir)
        return statvfs_info.f_frsize
