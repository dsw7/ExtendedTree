.PHONY = format compile clean lint test
.DEFAULT_GOAL = compile

format:
	@clang-format -i --verbose --style=file src/*.cpp src/*.hpp

compile: format
	@cmake -S src -B build
	@make --jobs=12 --directory=build install

clean:
	@rm -rfv build

lint:
	@cppcheck src --enable=all

test: export PATH_BIN = $(CURDIR)/build/etree
test: compile
	@python3 -m unittest -v tests/test*.py
