import os
import subprocess

OPENCL_TESTSUITE_FOLDER = "../latest_benchmarks/OpenCL/"
SPV_DISASSEMBLY_FOLDER = "../latest_benchmarks/spv-dis/"
CLSPV_PATH = os.environ["CLSPV_PATH"]
SPIRV_DIS_PATH = os.environ["SPIRVi_DIS_PATH"]
FILE_MAP_TXT = "./port-result.txt"
NO_GPUVERIFY_SPECIFIC_FEATURE = "./no_gpuverify_specific_feature.txt"


class PortingError(Exception):
    pass


def get_opencl_tests(directory):
    tests = {}
    for (dirpath, dirnames, filenames) in os.walk(directory):
        if len(filenames) > 0:
            for file in filenames:
                if file.endswith(".cl"):
                    test_name = dirpath.replace(OPENCL_TESTSUITE_FOLDER, "").split("/")[-1]
                    test_path = os.path.join(dirpath, file)
                    test_dir = dirpath.replace(OPENCL_TESTSUITE_FOLDER, "")
                    if test_name in tests:
                        raise ValueError(f"Duplicate test name: {test_name}")
                    tests[test_path] = (test_name, test_dir)
    return tests


def compile_with_clspv(input_file, output_file):
    command = [CLSPV_PATH, input_file, "--cl-std=CL2.0", "--inline-entry-points", "--spv-version=1.6", "-o",
               output_file]
    result = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if result.returncode != 0:
        return 1
    return 0


def disassemble_with_spirv_dis(input_file, output_file):
    command = [SPIRV_DIS_PATH, input_file, "-o", output_file]
    result = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if result.returncode != 0:
        return 1
    return 0


def get_config_header(config_dict):
    # If the header of a .cl test has --num_groups=1, then use grid 3,1,1
    if "--num_groups" in config_dict:
        num_groups = config_dict["--num_groups"]
        if num_groups == "1":
            return "; @Config: 3, 1, 1\n"
        # If the header of a .cl test has --num_groups>1, then use grid 3,1,3
        return "; @Config: 3, 1, 3\n"
    # If the header of a .cl test has --global--size and its value is equal to --local-size, then use grid 3,1,1
    if "--global_size" in config_dict:
        global_size = config_dict["--global_size"]
        local_size = config_dict["--local_size"]
        if global_size == local_size:
            return "; @Config: 3, 1, 1\n"
        # If the header of a .cl test has --global--size and its value greater than --local-size, then use grid 3,1,3
        return "; @Config: 3, 1, 3\n"
    # If the header doesn't have --local-size or (the header doesn't have --global-size and the header doesn't have --group-size), then something is wrong with the test. A test must have local-size and (group-size or global-size, but not both).
    if "--local_size" not in config_dict or ("--global_size" not in config_dict and "--group_size" not in config_dict):
        raise PortingError("A test must have local-size and (group-size or global-size, but not both)")


def get_input_header(disassemble_lines):
    instructions = {}
    for line in disassemble_lines:
        if "=" not in line:
            continue
        key, value = line.split("=")
        instructions[key.strip()] = value.strip()

    runtime_array_pointer = ""
    for key, value in instructions.items():
        if "OpTypeRuntimeArray" in value:
            runtime_array_pointer = key
            break
    if runtime_array_pointer == "":
        return ""

    struct = ""
    for key, value in instructions.items():
        if runtime_array_pointer in value:
            struct = key
            break
    if struct == "":
        return ""

    struct_pointer = ""
    for key, value in instructions.items():
        if struct in value:
            struct_pointer = key
            break
    if struct_pointer == "":
        return ""

    variables = []
    for key, value in instructions.items():
        if struct_pointer in value:
            variables.append(key)

    res = ""
    for variable in variables:
        res += f"; @Input: {variable} = " + "{{0, 0, 0, 0, 0, 0, 0, 0, 0}}\n"

    return res


def add_header(opencl_file, dis_assembly_file):
    with open(opencl_file, "r") as f:
        opencl_content = f.read()
    with open(dis_assembly_file, "r") as f:
        disassemble_content = f.read()

    # Get config header
    config_line = opencl_content.split("\n")[1]
    config_list = config_line.strip("//").split(" ")
    config_dict = {}
    for conf in config_list:
        if "=" not in conf:
            continue
        key, value = conf.split("=")
        config_dict[key] = value
    config_header = get_config_header(config_dict)

    # Get input header
    disassemble_lines = disassemble_content.split("\n")
    input_header = get_input_header(disassemble_lines)
    disassemble_content = input_header + config_header + disassemble_content
    with open(dis_assembly_file, "w") as f:
        f.write(disassemble_content)


def without_gpuverify_specific_feature(test):
    with open(test, "r") as f:
        content = f.read()
    if "assert" in content:
        return False
    if "invariant" in content:
        return False
    if "requires" in content:
        return False
    if "assume" in content:
        return False
    return True


def check_support(test):
    with open(test, "r") as f:
        content = f.read()
    if "float" in content:
        return False
    return without_gpuverify_specific_feature(test)


def port_test():
    tests = get_opencl_tests(OPENCL_TESTSUITE_FOLDER)
    print(f"Total tests of gpu-verify: {len(tests)}")
    succeed_tests = {}
    general_tests = []
    for test in tests:
        if not check_support(test):
            continue
        if without_gpuverify_specific_feature(test):
            general_tests.append(test)
        test_name, test_dir = tests[test]
        base_dir = os.path.join(SPV_DISASSEMBLY_FOLDER, test_dir)
        os.makedirs(base_dir, exist_ok=True)
        spv_file = os.path.join(base_dir, test_name + ".spv")
        dis_assembly_file = os.path.join(base_dir, test_name + ".spv.dis")
        compile_res = compile_with_clspv(test, spv_file)
        if compile_res != 0:
            continue
        disassemble_res = disassemble_with_spirv_dis(spv_file, dis_assembly_file)
        if disassemble_res != 0:
            continue
        subprocess.run(["rm", spv_file])
        try:
            add_header(test, dis_assembly_file)
        except PortingError as _:
            subprocess.run(["rm", dis_assembly_file])
            continue
        succeed_tests[test] = dis_assembly_file

    return succeed_tests, general_tests


def main():
    ported_tests, general_tests = port_test()
    print(f"Succeed tests: {len(ported_tests)}")
    print(f"General tests: {len(general_tests)}")
    with open(FILE_MAP_TXT, "w") as f:
        for test in ported_tests:
            f.write(f"{test} {ported_tests[test]}\n")
    with open(NO_GPUVERIFY_SPECIFIC_FEATURE, "w") as f:
        for test in general_tests:
            f.write(f"{test}\n")


if __name__ == "__main__":
    main()

# Output:
#Total tests of gpu-verify: 486
# Succeed tests: 186
# General tests: 217
