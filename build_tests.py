import glob
from os import path


INCLUDE_PATHS = ['include', 'test']
TEST_RUNNER = 'testrunner'
HEADER_TEMPLATE = """#ifndef TACK_TESTS_H
#define TACK_TESTS_H

#include "test.h"

{declarations}

#endif /* end of include guard: TACK_TESTS_H */
"""
C_FILE_TEMPLATE = """#include "__tests.h"

void run_all_tests() {{
{tests}
}}
"""


def make_header_file(test_functions):
    with open('test/__tests.h', 'w') as header:
        header.write(HEADER_TEMPLATE.format(declarations='\n'.join(
            f'void {f}();' for f in test_functions)))


def make_c_file(test_functions):
    with open('test/__tests.c', 'w') as cfile:
        cfile.write(C_FILE_TEMPLATE.format(
            tests='\n'.join(f'  {f}();' for f in test_functions)))


def create_compile_command(test_files):
    cfiles = [f.replace('test/test-', '') for f in test_files]
    files = ' '.join(list(test_files) + cfiles +
                     ['test/__tests.c', 'test/test.c'])
    includes = ' '.join(f'-I{i}/' for i in INCLUDE_PATHS)
    return f'cc {includes} {files} -o {TEST_RUNNER}'


def main():
    files_to_include = set()
    test_functions = []

    for test_file in glob.glob('test/*.c'):
        with open(test_file) as f:
            for line in f.readlines():
                if line.startswith('void test_'):
                    func = line.partition('()')[0]
                    test_functions.append(func.replace('void ', ''))
                    files_to_include.add(test_file)

    make_header_file(test_functions)
    make_c_file(test_functions)
    print(create_compile_command(files_to_include))


if __name__ == '__main__':
    main()
