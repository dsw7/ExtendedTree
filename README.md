# ExtendedTree
A lightweight `tree` equivalent. This program returns information similar to the `ncdu` command (see
[ncdu(1)](https://linux.die.net/man/1/ncdu) for more information) but in tree-like format.

[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/dsw7/ExtendedTree/master/LICENSE)

## About
This command displays relative disk usage for a directory tree. For example, suppose we run:
```console
tree bar
```
Which returns:
```console
bar/
├── bar/
│   ├── a.txt
│   ├── b.txt
│   └── c.txt
├── baz/
│   ├── a.txt
│   ├── b.txt
│   └── c.txt
└── foo/
    ├── a.txt
    ├── b.txt
    └── c.txt
```
And suppose that `{a,b,c}.txt` all contain 3 bytes. The ExtendedTree equivalent:
```console
etree bar
```
Will return:
```console
└── bar/ [ 52.00 KiB, 100.00%, 9 ]
    ├── baz/ [ 16.00 KiB, 30.77%, 3 ]
    │   ├── a.txt [ 4.00 KiB, 7.69% ]
    │   ├── b.txt [ 4.00 KiB, 7.69% ]
    │   └── c.txt [ 4.00 KiB, 7.69% ]
    ├── foo/ [ 16.00 KiB, 30.77%, 3 ]
    │   ├── a.txt [ 4.00 KiB, 7.69% ]
    │   ├── b.txt [ 4.00 KiB, 7.69% ]
    │   └── c.txt [ 4.00 KiB, 7.69% ]
    └── bar/ [ 16.00 KiB, 30.77%, 3 ]
        ├── a.txt [ 4.00 KiB, 7.69% ]
        ├── b.txt [ 4.00 KiB, 7.69% ]
        └── c.txt [ 4.00 KiB, 7.69% ]

Total size: 52.00 KiB
Number of directories: 3
Number of files: 9
Number of other file-like objects: 0
```
