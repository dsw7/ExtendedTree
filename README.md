# ExtendedTree
A lightweight `tree` equivalent

[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/dsw7/ExtendedTree/master/LICENSE)

## About
This command displays relative disk usage for a directory tree. For example, suppose we run:
```console
tree bar
```
Which returns:
```
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
```
bar/ [ 100.00% ]
    baz/ [ 33.33% ]
        a.txt [ 11.11% ]
        b.txt [ 11.11% ]
        c.txt [ 11.11% ]
    foo/ [ 33.33% ]
        a.txt [ 11.11% ]
        b.txt [ 11.11% ]
        c.txt [ 11.11% ]
    bar/ [ 33.33% ]
        a.txt [ 11.11% ]
        b.txt [ 11.11% ]
        c.txt [ 11.11% ]
```
Note that this calculation is based on the file size itself and not on block size.
