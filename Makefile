.PHONY = help format compile clean lint test
.DEFAULT_GOAL = help

define HELP_LIST_TARGETS
To format code:
    $$ make format
To compile binary:
    $$ make compile
To remove build directory:
    $$ make clean
To run cppcheck linter:
    $$ make lint
To run unit tests:
    $$ make test
endef

export HELP_LIST_TARGETS

help:
	@echo "$$HELP_LIST_TARGETS"

format:
	@clang-format -i --verbose --style=file ExtendedTree/*.cpp ExtendedTree/*.hpp

compile: format
	@cmake -S ExtendedTree -B build
	@make --jobs=12 --directory=build install

clean:
	@rm -rfv build

lint:
	@cppcheck ExtendedTree --enable=all

test: export PATH_BIN = $(CURDIR)/build/etree
test: compile
	@python3 -m unittest -v tests/test*.py
