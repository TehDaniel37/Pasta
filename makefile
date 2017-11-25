CC = gcc
TARGET = schedr

# Dependencies
ssct_h = src/include/ssct.h
ssct_url = https://github.com/TehDaniel37/ssct.h/raw/master/ssct.h

# Compiler flags
debug_flags = -g -Wall -pedantic -Werror
release_flags = -O3
test_flags = $(debug_flags) -DTEST --coverage

# Directories
debug_target_dir = bin/debug
release_target_dir = bin/release
install_dir = /usr/local/bin
test_target_dir = bin/debug/test
target_dir = $(debug_target_dir)
include_dir = src/include
src_dir = src/main
test_dir = src/test
object_dir = obj
test_object_dir = obj/test
test_deps_dir = obj/test/deps
test_resource_dir = res/debug/test

# Files
sources := $(shell find $(src_dir) -name '*.c')
objects := $(patsubst $(src_dir)/%.c, $(object_dir)/%.o, $(sources))
headers := $(shell find $(include_dir) -name '*.h')
tests := $(shell find $(test_dir) -name '*.c')
test_objects := $(patsubst $(test_dir)/%.c, $(test_object_dir)/%.o, $(tests))
test_deps := $(patsubst $(src_dir)/%.c, $(test_deps_dir)/%.o, $(sources))
test_deps := $(filter-out $(test_deps_dir)/main.o, $(test_deps))
test_targets := $(patsubst $(test_dir)/%.c, $(test_target_dir)/%, $(tests))
cov_files := $(patsubst $(test_deps_dir)/%.o, $(test_deps_dir)/%.gcno, $(test_deps))

.PHONY: all
all: release 

.PHONY: install
install: release
	@if ! ldconfig -p | grep "libc.so.6" >/dev/null ; then \
		echo "Needed library 'libc.so.6' is not installed. Please install it and try again." ; \
	else \
		sudo cp $(release_target_dir)/$(TARGET) $(install_dir)/$(TARGET) ; \
	fi

.PHONY: uninstall
uninstall:
	@if [[ -f $(install_dir)/$(TARGET) ]] ; then \
		sudo rm $(install_dir)/$(TARGET) ; \
	fi

.PHONY: release
release: CFLAGS=$(release_flags)
release: target_dir=$(release_target_dir)
release: create_dirs
	$(CC) $(CFLAGS) -I $(include_dir) $(sources) -o $(target_dir)/$(TARGET)

# Checks syntax of sources, tests and include files. Also runs static code 
# analysis through cppcheck.
.PHONY: check
check:
	$(CC) -DTEST -fsyntax-only -I $(include_dir) $(sources) $(tests)
	cppcheck -q --enable=all -I src/include --language=c --platform=unix64 --std=c11 --suppress=missingIncludeSystem src

# Compile and link debug version of program
.PHONY: debug
debug: CFLAGS=$(debug_flags)
debug: target_dir=$(debug_target_dir)
debug: build

.PHONY: clean
clean:
	rm -rf obj/* bin/debug/* bin/release/* $(gcov_dir) $(ssct_h)

.PHONY: create_dirs
create_dirs: $(test_deps_dir) $(test_target_dir) $(release_target_dir)

$(test_deps_dir):
	mkdir -p $(test_deps_dir)

$(test_target_dir):
	mkdir -p $(test_target_dir)

$(release_target_dir):
	mkdir -p $(release_target_dir)

.PHONY: build
build: create_dirs $(objects)
	$(CC) $(CFLAGS) $(objects) -o $(target_dir)/$(TARGET)

$(object_dir)/%.o: $(src_dir)/%.c $(headers)
	$(CC) $(CFLAGS) -I $(include_dir) -c $< -o $@

# Calculate code coverage.
.PHONY: cov
cov: CFLAGS=$(test_flags)
cov: clean create_dirs $(ssct_h) $(test_targets)
	for target in $(test_targets) ; do \
		./$$target &>/dev/null ; \
	done
	gcovr -r $(test_deps_dir)

# Run test cases
.PHONY: test
test: CFLAGS=$(test_flags)
test: create_dirs $(ssct_h) $(test_targets)
	for target in $(test_targets) ; do \
		./$$target ; \
	done

.PHONY: memcheck
memcheck: CFLAGS=$(test_flags)
memcheck: create_dirs $(test_targets)
	for target in $(test_targets) ; do \
		valgrind -q --log-fd=2 --track-origins=yes --leak-check=full $$target >/dev/null ; \
	done

# Link and run each test
$(test_target_dir)/%: $(test_object_dir)/%.o $(test_deps)
	$(CC) $(CFLAGS) $< $(test_deps) -o $@ 

# Compile tests
$(test_object_dir)/%.o: $(test_dir)/%.c $(headers)
	$(CC) $(CFLAGS) -I $(include_dir) -c $< -o $@

# Compile test dependencies
$(test_deps_dir)/%.o: $(src_dir)/%.c $(headers)
	$(CC) $(CFLAGS) -I $(include_dir) -c $< -o $@

# Download ssct.h
$(ssct_h):
	wget -q -O $(include_dir)/ssct.h $(ssct_url)

