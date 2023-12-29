import os


def parse_success_line(output_line: str):
    if " -- " not in output_line:
        return None
    if "; " not in output_line:
        return None
    if " A" not in output_line:
        return None

    output_line = output_line.replace(" -- ", " ").replace("; ", " ").replace(" A", "")
    items = output_line.split()

    return (items[0], items[1], float(items[2]))


def compare_success_lines(lhs: str, rhs: str) -> bool:
    EPS = 1e-8

    parsed_lhs = parse_success_line(lhs)
    if parsed_lhs is None:
        return False
    
    parsed_rhs = parse_success_line(rhs)
    if parsed_rhs is None:
        return False
    
    if parsed_lhs[0] != parsed_rhs[0]:
        return False
    
    if parsed_lhs[1] != parsed_rhs[1]:
        return False
    
    if abs(parsed_lhs[2] - parsed_rhs[2]) > EPS:
        return False
    
    return True


def success_test(file_in, file_out):
    file_in_path = f"../resources/intensity/{file_in}"
    file_out_path = f"../resources/intensity/{file_out}"

    with open(file_out_path, 'r') as f:
        answer_lines = [line.strip() for line in f]
    
    result = os.popen(f"../build/intensity < {file_in_path}").read()
    result_lines = result.split('\n')[:-1]

    if (len(answer_lines) != len(result_lines)):
        return False
    
    for result_line, answer_line in zip(result_lines, answer_lines):
        if not compare_success_lines(result_line, answer_line):
            return False
        
    return True


def error_test(file_in, file_out):
    file_in_path = f"../resources/intensity/{file_in}"
    file_out_path = f"../resources/intensity/{file_out}"

    with open(file_out_path, 'r') as f:
        answer_lines = [line.strip() for line in f]
    
    result = os.popen(f"../build/intensity < {file_in_path}").read()
    result_lines = result.split('\n')[:-1]

    if (len(answer_lines) != len(result_lines)):
        return False
    
    if (answer_lines[0] != result_lines[0]):
        return False
    
    if (answer_lines[-1] != result_lines[-1]):
        return False
    
    return True


TESTS, ERROR_TESTS = 11, 3
for i in range(TESTS):
    is_passed = success_test(f"test{i + 1}.in", f"test{i + 1}.out")
    print(f"Test", i + 1, ": ", "SUCCESS" if is_passed else "ERROR!!!!")

print()

for i in range(ERROR_TESTS):
    is_passed = error_test(f"test_error{i + 1}.in", f"test_error{i + 1}.out")
    print(f"TestError", i + 1, ": ", "SUCCESS" if is_passed else "ERROR!!!!")
