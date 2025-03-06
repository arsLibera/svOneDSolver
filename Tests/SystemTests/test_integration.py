import shutil
import csv
import glob
import os
import re
import subprocess
import numpy as np
import pytest
from collections import defaultdict

@pytest.fixture(scope="session")
def exePath(pytestconfig):
    relativePath = pytestconfig.getoption("relativeExePath")

    assert relativePath, "--relativeExePath is required. Please " \
                         "provide the (relative) path to the executable."

    return os.path.abspath(relativePath)


def run_simulation_and_assert_results(testCaseName, tmpdir, exePath, resultData, run_func):
    # Run the test case using the provided function (either legacy or converted JSON)
    results = run_test_case_by_name(testCaseName, tmpdir, exePath, run_func)

    # Loop over each result check and assert
    for field, seg, node, time, fun, expectedValue, rtol in resultData:
        resultValue = get_result_value(results, field, seg, node, time, fun)

        # Assertion with details
        assert resultValue == pytest.approx(expectedValue, rel=rtol, abs=1e-8), \
            f"\n\nTest failed for {testCaseName} - Field: {field}, Seg: {seg}, Node: {node}, Time: {time}, Fun: {fun}. " \
            f"\nExpected: {expectedValue}, Got: {resultValue}\n"


def run_test_case_by_name(name, testDir, exePath, run_func):
    # test file absolute path
    inputFilePath = os.path.join(os.path.dirname(__file__), 'cases', name + '.in')

    # run 1D simulation
    run_func(inputFilePath, testDir, exePath)

    # extract results
    return read_results_1d(testDir, 'results_' + name + '_seg*')


def run_oned(inputFilePath, testDir, exePath):
    # Run simulation in the testDir to keep the current directory clean. 
    # Because we are in a temporary directory, we have to pass the
    # locations as absolute paths.
    try:
        subprocess.check_output([exePath, inputFilePath], cwd=testDir)
    except subprocess.CalledProcessError as err:
        raise RuntimeError('Test failed. svOneDSolver returned error:\n' + err.output.decode("utf-8"))


def run_converted_json_input(inputFilePath, testDir, exePath):
    # Run the simulations again, but this time use the JSON input file
    try:
        tempJsonFilePath = "tempInput.json"

        conversionCommandList = [exePath, "-legacyToJson", f'"{inputFilePath}"', f'"{tempJsonFilePath}"']
        runCommandList = [exePath, "-jsonInput", f'"{tempJsonFilePath}"']

        # First, convert to legacy ".in" file to a JSON input file
        subprocess.check_output(conversionCommandList, cwd=testDir)

        # Now, run the actual simulation with the JSON input file
        subprocess.check_output(runCommandList, cwd=testDir)
    except subprocess.CalledProcessError as err:
        raise RuntimeError('Test failed. svOneDSolver returned error:\n' + err.output.decode("utf-8"))


def read_results_1d(res_dir, name):
    """
    Read results from oneDSolver and store in dictionary
    Args:
        res_dir: directory containing 1D results
        name: pattern to search for in result file names
    Returns:
        dictionary res[result field][segment id][node, time step]
    """
    # read from files, store in dict, and remove files
    res = defaultdict(lambda: defaultdict(list))
    for field in ['flow', 'pressure', 'area', 'wss', 'Re']:
        for f_res in glob.glob(os.path.join(res_dir, name + '_' + field + '.dat')):
            with open(f_res) as f:
                for line in csv.reader(f, delimiter=' '):
                    res[field][int(re.findall(r'\d+', f_res)[-1])] += [[float(m) for m in line if m][1:]]
            os.remove(f_res)

    # convert results to numpy array
    for f in res.keys():
        for s in res[f].keys():
            res[f][s] = np.array(res[f][s])

    return res


def get_result_value(results, field, seg, node, time, fun):
    """
    Read results and select function
    """
    # extract result
    res = results[field][seg][node, time]

    # select result type
    if fun == 'point':
        return res
    elif fun == 'mean':
        return np.mean(res)
    elif fun == 'max':
        return np.max(res)
    elif fun == 'min':
        return np.min(res)
    else:
        raise ValueError('Unknown result type ' + fun)

# Each element in the test cases list is a tuple structured as follows:
# 
# (<test_case_name>, <result_data>)
#     - <test_case_name>: A string representing the name of the test case
#     - <result_data>: A list of tuples, each containing:
#         (<field>, <segment>, <node>, <time>, <function>, <expected_value>, <rtol>)
#             - <field>: A string representing the result field to be checked (e.g., 'pressure', 'flow', 'area', etc.).
#             - <segment>: The segment index (integer or `-1` for the last or global segment).
#             - <node>: The node index within the segment (integer or `-1` for the last or global node).
#             - <time>: The time step index (integer or `-1` for the last time step or a range like `np.arange(start, stop)`).
#             - <function>: A string indicating the result aggregation method ('point', 'mean', 'max', or 'min').
#                 - 'point': Direct comparison of the result at the specified segment, node, and time.
#                 - 'mean': The average value of the results over the specified time steps.
#                 - 'max': The maximum value of the results over the specified time steps.
#                 - 'min': The minimum value of the results over the specified time steps.
#             - <expected_value>: The expected value to be compared against the result.
#             - <rtol>: The relative tolerance for the check
#
# Note: need to verify the particular meanings of each field. It would be nicer if they
# were less complicated (like not using a value of "-1", but instead using something more
# obvious, like a stand-in helper for the first or last)

test_cases = [
    ('tube_pressure', [
        ('pressure', 0, 0, -1, 'point', 11005.30965, 1e-7),
        ('pressure', 0, -1, -1, 'point', 10000.0, 1e-8),
        ('flow', 0, -1, -1, 'point', 100.0, 1e-16),
        ('area', 0, -1, -1, 'point', 1.0, 1e-5),
    ]),
    ('tube_pressure_wave', [
        ('pressure', 0, 0, -1, 'point', 10000.0, 1e-8),
        ('pressure', 0, -1, -1, 'point', 9086.52306835, 1e-4),
        ('flow', 0, -1, -1, 'point', 90.8652306835, 1e-4),
        ('area', 0, -1, -1, 'point', 1.0, 1e-5),
    ]),
    ('tube_rcr', [
        ('pressure', 0, 0, -1, 'point', 11005.30965, 1e-7),
        ('pressure', 0, -1, -1, 'point', 10000.0, 1e-8),
        ('flow', 0, -1, -1, 'point', 100.0, 1e-16),
        ('area', 0, -1, -1, 'point', 1.0, 1e-5),
    ]),
    ('tube_rcr_Pd', [
        ('pressure', 0, 0, -1, 'point', 12005.30965, 1e-7),
        ('pressure', 0, -1, -1, 'point', 11000.0, 1e-8),
        ('flow', 0, -1, -1, 'point', 100.0, 1e-16),
        ('area', 0, -1, -1, 'point', 1.0, 1e-5),
    ]),
    ('tube_r', [
        ('pressure', 0, 0, -1, 'point', 11005.30965, 1e-7),
        ('pressure', 0, -1, -1, 'point', 10000.0, 1e-8),
        ('flow', 0, -1, -1, 'point', 100.0, 1e-16),
        ('area', 0, -1, -1, 'point', 1.0, 1e-5),
    ]),
    ('tube_r_Pd', [
        ('pressure', 0, 0, -1, 'point', 12005.30965, 1e-7),
        ('pressure', 0, -1, -1, 'point', 11000.0, 1e-8),
        ('flow', 0, -1, -1, 'point', 100.0, 1e-16),
        ('area', 0, -1, -1, 'point', 1.0, 1e-5),
    ]),
    ('tube_r_stab', [
        ('pressure', 0, 0, -1, 'point', 11005.30965, 1e-7),
        ('pressure', 0, -1, -1, 'point', 10000.0, 1e-8),
        ('flow', 0, -1, -1, 'point', 100.0, 1e-16),
        ('area', 0, -1, -1, 'point', 1.0, 1e-5),
    ]),
    ('tube_stenosis_r', [
        ('pressure', 0, 0, -1, 'point', 10150.68211, 1e-6),
        ('pressure', 2, -1, -1, 'point', 10000.0, 1e-8),
        ('flow', 0, -1, -1, 'point', 100.0, 1e-10),
        ('area', 0, -1, -1, 'point', 10.0, 1e-4),
    ]),
    ('bifurcation_P', [
        ('pressure', 0, 0, -1, 'point', 4039.45953118937, 1e-5),
        ('pressure', 0, -1, -1, 'point', 4026.67220709878, 1e-5),
        ('pressure', 1, 0, -1, 'point', 4026.67220709878, 1e-5),
        ('pressure', 2, 0, -1, 'point', 4026.67220709878, 1e-5),
        ('pressure', 1, -1, -1, 'point', 4000.00, 1e-5),
        ('pressure', 2, -1, -1, 'point', 4000.00, 1e-5),
        ('flow', 1, -1, -1, 'point', 3.9925, 1e-6),
        ('flow', 2, -1, -1, 'point', 3.9925, 1e-6),
    ]),
    ('bifurcation_R', [
        ('pressure', 0, 0, -1, 'point', 3997.46433118937, 1e-5),
        ('pressure', 0, -1, -1, 'point', 3984.67700709878, 1e-5),
        ('pressure', 1, 0, -1, 'point', 3984.67700709878, 1e-5),
        ('pressure', 2, 0, -1, 'point', 3984.67700709878, 1e-5),
        ('pressure', 1, -1, -1, 'point', 3958.0048, 1e-5),
        ('pressure', 2, -1, -1, 'point', 3958.0048, 1e-5),
        ('flow', 1, -1, -1, 'point', 3.9925, 1e-5),
        ('flow', 2, -1, -1, 'point', 3.9925, 1e-5),
    ]),
    ('bifurcation_R_stab', [
        ('pressure', 0, 0, -1, 'point', 3997.46433118937, 1e-6),
        ('pressure', 0, -1, -1, 'point', 3984.67700709878, 1e-6),
        ('pressure', 1, 0, -1, 'point', 3984.67700709878, 1e-6),
        ('pressure', 2, 0, -1, 'point', 3984.67700709878, 1e-6),
        ('pressure', 1, -1, -1, 'point', 3958.0048, 1e-7),
        ('pressure', 2, -1, -1, 'point', 3958.0048, 1e-7),
        ('flow', 1, -1, -1, 'point', 3.9925, 1e-6),
        ('flow', 2, -1, -1, 'point', 3.9925, 1e-6),
    ]),
    ('bifurcation_RCR', [
        ('pressure', 0, 0, np.arange(100, 200), 'mean', 123878.022943, 1e-7),
        ('pressure', 0, 0, np.arange(100, 200), 'max', 168182.372624, 1e-7),
        ('pressure', 0, 0, np.arange(100, 200), 'min', 89237.6441223, 1e-7),
        ('pressure', 1, -1, np.arange(100, 200), 'mean', 123855.677783, 1e-7),
        ('pressure', 1, -1, np.arange(100, 200), 'max', 171598.373528, 1e-7),
        ('pressure', 1, -1, np.arange(100, 200), 'min', 87624.0897929, 1e-7),
        ('pressure', 2, -1, np.arange(100, 200), 'mean', 123855.677783, 1e-7),
        ('pressure', 2, -1, np.arange(100, 200), 'max', 171598.373528, 1e-7),
        ('pressure', 2, -1, np.arange(100, 200), 'min', 87624.0897929, 1e-7),
        ('flow', 0, 0, np.arange(100, 200), 'mean', 7.557147487534, 1e-7),
        ('flow', 1, -1, np.arange(100, 200), 'mean', 3.839024865141, 1e-7),
        ('flow', 1, -1, np.arange(100, 200), 'max', 24.0553490482, 1e-7),
        ('flow', 1, -1, np.arange(100, 200), 'min', -3.35029015773, 1e-7),
    ]),
        ('bifurcation_RCR_staticFunc', [
        ('pressure', 0, 0, -1, 'point', 3997.46433118937, 1e-6),
        ('pressure', 0, -1, -1, 'point', 3984.67700709878, 1e-6),
        ('pressure', 1, 0, -1, 'point', 3984.67700709878, 1e-6),
        ('pressure', 2, 0, -1, 'point', 3984.67700709878, 1e-6),
        ('pressure', 1, -1, -1, 'point', 3958.0048, 1e-7),
        ('pressure', 2, -1, -1, 'point', 3958.0048, 1e-7),
        ('flow', 1, -1, -1, 'point', 3.9925, 1e-6),
        ('flow', 2, -1, -1, 'point', 3.9925, 1e-6),
    ])
]


# Test the simulation results using legacy <name>.in files as input
@pytest.mark.parametrize("testCaseName, resultData", test_cases)
def test_simulation_results_legacy_input(tmpdir, exePath, testCaseName, resultData):
    run_simulation_and_assert_results(testCaseName, tmpdir, exePath, resultData, run_oned)


# This test suite verifies that (at the system level) we can:
#    1. Convert input files <name>.in -> <name>.json in all test cases
#    2. Run the simulation using the <name>.json input file and verify
#       the results of the simulation are as expected.
@pytest.mark.parametrize("testCaseName, resultData", test_cases)
def test_simulation_results_converted_to_json_input(tmpdir, exePath, testCaseName, resultData):
    run_simulation_and_assert_results(testCaseName, tmpdir, exePath, resultData, run_converted_json_input)

