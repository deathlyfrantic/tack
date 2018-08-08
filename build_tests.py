import glob
from os import path


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
    with open('include/__tests.h', 'w') as header:
        header.write(HEADER_TEMPLATE.format(declarations='\n'.join(
            f'void {f}();' for f in test_functions)))


def make_c_file(test_functions):
    with open('src/__tests.c', 'w') as cfile:
        cfile.write(C_FILE_TEMPLATE.format(
            tests='\n'.join(f'  {f}();' for f in test_functions)))


def main():
    files = glob.glob('src/*.c')
    test_functions = []

    for test_file in glob.glob('src/*.c'):
        in_tests = False
        with open(test_file) as f:
            for line in f.readlines():
                if line.startswith('#ifdef TESTS'):
                    in_tests = True
                if in_tests and line.startswith('#endif'):
                    in_tests = False
                if in_tests and line.startswith('void test_'):
                    func = line.partition('()')[0]
                    test_functions.append(func.replace('void ', ''))

    make_header_file(test_functions)
    make_c_file(test_functions)


if __name__ == '__main__':
    main()
