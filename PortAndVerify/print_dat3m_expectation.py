import os
import subprocess

SPV_DISASSEMBLY_FOLDER = "../testsuite/spv-dis/"
FILE_MAP_TXT = "./port-result.txt"
VERIFY_RESULT_TXT = "./verify-result.txt"
OUTPUT_TXT = "./output.txt"


PASS = "PASS"
FAIL = "FAIL"
RACE = "RACE"
ABORT = "ABORT"


UN_SUPPORTED_TOKEN = ["|"]


def check_support(test):
    with open(test, "r") as f:
        for line in f:
            if any(token in line for token in UN_SUPPORTED_TOKEN):
                return False
    return True


def read_verify_result():
    verify_result = {}
    with open(VERIFY_RESULT_TXT, "r") as f:
        for line in f:
            test, result = line.split()
            verify_result[test] = result
    return verify_result


def dat3m_print(test_path, result, support):
    test_path = test_path.replace(SPV_DISASSEMBLY_FOLDER, "")
    if support:
        return f"{{\"{test_path}\", 1, {result}}},\n"
    else:
        return f"// {{\"{test_path}\", 1, {result}}},\n"


def print_safety_result(verify_result):
    output = ""
    for test in verify_result:
        support = check_support(test)
        if verify_result[test] == PASS:
            output += dat3m_print(test, PASS, support)
    return output


def print_race_result(verify_result):
    output = ""
    for test in verify_result:
        support = check_support(test)
        if verify_result[test] == PASS:
            output += dat3m_print(test, PASS, support)
        if verify_result[test] == RACE:
            output += dat3m_print(test, FAIL, support)
    return output

def main():
    verify_result = read_verify_result()
    safety_output = print_safety_result(verify_result)
    race_output = print_race_result(verify_result)
    with open(OUTPUT_TXT, "w") as f:
        f.write(race_output)


if __name__ == "__main__":
    main()
