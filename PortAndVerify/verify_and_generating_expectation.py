# docker python version: 3.4.0
import os
import subprocess

MOUNTED_DIR = "./"
FILE_MAP_TXT = os.path.join(MOUNTED_DIR, "./port-result.txt")
VERIFY_RESULT_TXT = os.path.join(MOUNTED_DIR, "./verify-result.txt")
GPUVERIFY_PATH = "../gpuverify"
EXTRA_PAR = "--no-benign-tolerance"

PASS = "PASS"
RACE = "RACE"
ABORT = "ABORT"


class PortingError(Exception):
    pass


def get_tests():
    test_parameters = {}
    with open(FILE_MAP_TXT, "r") as f:
        for line in f:
            test, _ = line.split()
            test = os.path.join(MOUNTED_DIR, test)
            with open(test, "r") as f:
                test_content = f.read()
            config_line = test_content.split("\n")[1]
            config_list = config_line.strip("//").split(" ")
            test_parameters[test] = config_list

    return test_parameters


def write_verify_result(verify_result):
    original_test_map = {}
    with open(FILE_MAP_TXT, "r") as f:
        for line in f:
            test, file = line.split()
            original_test_map[test] = file

    with open(VERIFY_RESULT_TXT, "w") as f:
        for test in verify_result:
            original_test_name = test[2:]
            f.write(original_test_map[original_test_name] + " " + verify_result[test] + "\n")


def verify_test(test, params):
    command = [GPUVERIFY_PATH, test]
    command.extend(params)
    command.append(EXTRA_PAR)
    try:
        res = subprocess.check_output(command, stderr=subprocess.STDOUT)
        # print(res.decode("utf-8"))
        return PASS
    except subprocess.CalledProcessError as exc:
        error_msg = exc.output.decode("utf-8")
        if "possible" in error_msg and "race on" in error_msg:
            return RACE
        elif "barrier may be reached by" in error_msg:
            # barrier divergence
            return ABORT
        elif "possible null pointer access for" in error_msg:
            # error: possible null pointer access for work item 0 in work group 0
            return PASS
        elif "error: this assertion might not hold" in error_msg:
            # error: this assertion might not hold for work item 1 in work group 0
            # A[get_global_id(0)] = sub_sat(A[get_global_id(0)], B[get_global_id(0)]);
            return ABORT
        elif "this is an implementation limitation" in error_msg:
            # kernel.opt.bc: error: wait_group_events with a variable-sized set of events not supported
            # Please contact the developers; this is an implementation limitation
            return ABORT
        elif "is not a .gbpl file" in error_msg:
            # GPUVerify: error: /checkArrays:A,AB,B is not a .gbpl file
            return ABORT
        elif "global size does not divide by dimension" in error_msg:
            # GPUVerify: COMMAND_LINE_ERROR error (1): Dimension 1 of global size does not divide by dimension 1 of local size
            return ABORT
        elif "Dimensions of local and global size must match" in error_msg:
            # GPUVerify: COMMAND_LINE_ERROR error (1): Dimensions of local and global size must match
            return ABORT
        elif "COMMAND_LINE_ERROR error (1): argument --num_groups=: not allowed with argument --global_size=" in error_msg:
            # GPUVerify: COMMAND_LINE_ERROR error (1): argument --num_groups=: not allowed with argument --global_size=
            return ABORT
        elif "implicit declaration of function 'shuffle' is invalid in C99" in error_msg:
            # error: implicit declaration of function 'shuffle' is invalid in C99
            return ABORT
        elif "Got a SIGABRT while executing native code" in error_msg:
            # error: Got a SIGABRT while executing native code. This usually indicates a fatal error in the mono runtime or one of the native libraries used by your application.
            return ABORT
        elif "llvm::sys::PrintStackTrace" in error_msg or "bugle" in error_msg:
            # 0  bugle 0x00000000006c9cd2 llvm::sys::PrintStackTrace(_IO_FILE*) + 34
            return ABORT
        elif "getting souce loc info failed with: Exception of type 'System.Exception' was thrown." in error_msg:
            # error: getting souce loc info failed with: Exception of type 'System.Exception' was thrown.
            return ABORT
        else:
            print(error_msg)
            return ABORT


def main():
    tests = get_tests()
    verify_result = {}
    total_tests = len(tests)
    current = 0
    for test in tests:
        print("Verifying test: ", current + 1, "/", total_tests)
        current += 1
        params = tests[test]
        result = verify_test(test, params)
        verify_result[test] = result

    pass_count = 0
    race_count = 0
    abort_count = 0
    for test in verify_result:
        result = verify_result[test]
        if result == PASS:
            pass_count += 1
        elif result == RACE:
            race_count += 1
        elif result == ABORT:
            abort_count += 1
        else:
            continue
    print("---------------------------------")
    print("PASS: ", pass_count)
    print("RACE: ", race_count)
    print("ABORT: ", abort_count)
    print("---------------------------------")
    write_verify_result(verify_result)


if __name__ == "__main__":
    main()


# RESULT:
# ---------------------------------
# PASS:  116
# RACE:  53
# ABORT:  17
# ---------------------------------