BEGIN {
    in_tests = 0
    header_file = "include/__tests.h"
    c_file = "src/__tests.c"
}
$0 == "#ifdef TESTS" { in_tests = 1 }
in_tests == 1 && $0 == "#endif" { in_tests = 0 }
in_tests == 1 && /^void test_/ {
    gsub("void ", "")
    gsub(" {", "")
    tests[NR] = $0
}
END {
    print "#ifndef TACK_TESTS_H\n" \
          "#define TACK_TESTS_H\n\n" \
          "#include \"test.h\"\n" > header_file
    for (i in tests) {
        print "void " tests[i] ";" > header_file
    }
    print "\n#endif /* end of include guard: TACK_TESTS_H */" > header_file

    print "#include \"__tests.h\"\n\n" \
          "void run_all_tests() {" > c_file
    for (i in tests) {
        print "  " tests[i] ";" > c_file
    }
    print "}" > c_file
}
