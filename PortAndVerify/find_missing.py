import os
import subprocess

SPV_DISASSEMBLY_FOLDER = "../testsuite/spv-dis/"
FILE_MAP_TXT = "./port-result.txt"
VERIFY_RESULT_TXT = "./verify-result.txt"
OUTPUT_TXT = "./output.txt"
EXPECTATION_TXT = "./expectation.txt"


PASS = "PASS"
FAIL = "FAIL"
RACE = "RACE"
ABORT = "ABORT"


def read_verify_result():
    verify_result = {}
    with open(VERIFY_RESULT_TXT, "r") as f:
        for line in f:
            test, result = line.split()
            verify_result[test] = result
    return verify_result

def read_expectation():
    expectation = {}
    with open(EXPECTATION_TXT, "r") as f:
        for line in f:
            test,_, result = line.split(",")
            expectation[test] = result
    return expectation


def main():
    verify_result = read_verify_result()
    expectation = read_expectation()

    for test in verify_result:
        key = test.replace("../latest_benchmarks/spv-dis/", "")
        value = verify_result[test]
        if value == RACE:
            value = FAIL
        if value == ABORT:
            continue
        if key not in expectation:
            print(f"--------------This test is not in the expectation: {key} with result {value}")
            continue
        if value != expectation[key].strip():
            print(f"++++Expectation failed: {key} current is {expectation[key]} but the new expectation is: {value}")

if __name__ == "__main__":
    main()
