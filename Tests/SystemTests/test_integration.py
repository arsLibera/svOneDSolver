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

def run_test_case_by_name(name, testdir, exePath):
    """Run a test case by its case name.

    Args:
        name: Name of the test case.
        testdir: Directory for performing the simulation.
    """

    # test file
    testfile = os.path.join(os.path.dirname(__file__), 'cases', name + '.in')
    testfile_tmp = os.path.join(testdir, name + '.in')

    # copy input file to temporary test dir
    shutil.copyfile(testfile, testfile_tmp)

    # run 1D simulation
    run_oned(testfile_tmp, testdir, exePath)

    # extract results
    return read_results_1d(testdir, 'results_' + name + '_seg*')


def run_oned(name, testdir, exePath):
    """
    Executes svOneDSolver with given input file
    """

    # Absolute path of the input file
    inp = os.path.join(testdir, name)

    # Run simulation in the testdir to keep the current directory clean. 
    # Because we are in a temporary directory, we have to pass the
    # locations as absolute paths.
    try:
        subprocess.check_output([exePath, inp], cwd=testdir)
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


def get_result(results, field, seg, node, time, fun):
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

def test_tube_pressure(tmpdir, exePath):
    results = run_test_case_by_name('tube_pressure', tmpdir, exePath)
    assert np.isclose(get_result(results, 'pressure', 0, 0, -1, 'point'), 11005.30965, rtol=1.0e-7)
    assert np.isclose(get_result(results, 'pressure', 0, -1, -1, 'point'), 10000.0, rtol=1.0e-8)
    assert np.isclose(get_result(results, 'flow', 0, -1, -1, 'point'), 100.0, rtol=1.0e-16)
    assert np.isclose(get_result(results, 'area', 0, -1, -1, 'point'), 1.0, rtol=1.0e-5)


def test_tube_pressure_wave(tmpdir, exePath):
    results = run_test_case_by_name('tube_pressure_wave', tmpdir, exePath)
    assert np.isclose(get_result(results, 'pressure', 0, 0, -1, 'point'), 10000.0 , rtol=1.0e-8)
    assert np.isclose(get_result(results, 'pressure', 0, -1, -1, 'point'), 9086.52306835, rtol=1.0e-4)
    assert np.isclose(get_result(results, 'flow', 0, -1, -1, 'point'), 90.8652306835, rtol=1.0e-4)
    assert np.isclose(get_result(results, 'area', 0, -1, -1, 'point'), 1.0, rtol=1.0e-5)


def test_tube_rcr(tmpdir, exePath):
    results = run_test_case_by_name('tube_rcr', tmpdir, exePath)
    assert np.isclose(get_result(results, 'pressure', 0, 0, -1, 'point'), 11005.30965, rtol=1.0e-7)
    assert np.isclose(get_result(results, 'pressure', 0, -1, -1, 'point'), 10000.0, rtol=1.0e-8)
    assert np.isclose(get_result(results, 'flow', 0, -1, -1, 'point'), 100.0, rtol=1.0e-16)
    assert np.isclose(get_result(results, 'area', 0, -1, -1, 'point'), 1.0, rtol=1.0e-5)


def test_tube_rcr_Pd(tmpdir, exePath):
    results = run_test_case_by_name('tube_rcr_Pd', tmpdir, exePath)
    assert np.isclose(get_result(results, 'pressure', 0, 0, -1, 'point'), 12005.30965, rtol=1.0e-7)
    assert np.isclose(get_result(results, 'pressure', 0, -1, -1, 'point'), 11000.0, rtol=1.0e-8)
    assert np.isclose(get_result(results, 'flow', 0, -1, -1, 'point'), 100.0, rtol=1.0e-16)
    assert np.isclose(get_result(results, 'area', 0, -1, -1, 'point'), 1.0, rtol=1.0e-5)


def test_tube_r(tmpdir, exePath):
    results = run_test_case_by_name('tube_r', tmpdir, exePath)
    assert np.isclose(get_result(results, 'pressure', 0, 0, -1, 'point'), 11005.30965, rtol=1.0e-7)
    assert np.isclose(get_result(results, 'pressure', 0, -1, -1, 'point'), 10000.0, rtol=1.0e-8)
    assert np.isclose(get_result(results, 'flow', 0, -1, -1, 'point'), 100.0, rtol=1.0e-16)
    assert np.isclose(get_result(results, 'area', 0, -1, -1, 'point'), 1.0, rtol=1.0e-5)


def test_tube_r_Pd(tmpdir, exePath):
    results = run_test_case_by_name('tube_r_Pd', tmpdir, exePath)
    assert np.isclose(get_result(results, 'pressure', 0, 0, -1, 'point'), 12005.30965, rtol=1.0e-7)
    assert np.isclose(get_result(results, 'pressure', 0, -1, -1, 'point'), 11000.0, rtol=1.0e-8)
    assert np.isclose(get_result(results, 'flow', 0, -1, -1, 'point'), 100.0, rtol=1.0e-16)
    assert np.isclose(get_result(results, 'area', 0, -1, -1, 'point'), 1.0, rtol=1.0e-5)


def test_tube_r_stab(tmpdir, exePath):
    results = run_test_case_by_name('tube_r_stab', tmpdir, exePath)
    assert np.isclose(get_result(results, 'pressure', 0, 0, -1, 'point'), 11005.30965, rtol=1.0e-7)
    assert np.isclose(get_result(results, 'pressure', 0, -1, -1, 'point'), 10000.0, rtol=1.0e-8)
    assert np.isclose(get_result(results, 'flow', 0, -1, -1, 'point'), 100.0, rtol=1.0e-16)
    assert np.isclose(get_result(results, 'area', 0, -1, -1, 'point'), 1.0, rtol=1.0e-5)


def test_tube_stenosis_r(tmpdir, exePath):
    results = run_test_case_by_name('tube_stenosis_r', tmpdir, exePath)
    assert np.isclose(get_result(results, 'pressure', 0, 0, -1, 'point'), 10150.68211, rtol=1.0e-6)
    assert np.isclose(get_result(results, 'pressure', 2, -1, -1, 'point'), 10000.0, rtol=1.0e-8)
    assert np.isclose(get_result(results, 'flow', 0, -1, -1, 'point'), 100.0, rtol=1.0e-10)
    assert np.isclose(get_result(results, 'area', 0, -1, -1, 'point'), 10.0, rtol=1.0e-4)


def test_bifurcation_P(tmpdir, exePath):
    results = run_test_case_by_name('bifurcation_P', tmpdir, exePath)
    assert np.isclose(get_result(results, 'pressure', 0, 0, -1, 'point'), 4039.45953118937, rtol=1e-5)
    assert np.isclose(get_result(results, 'pressure', 0, -1, -1, 'point'), 4026.67220709878, rtol=1e-5)
    assert np.isclose(get_result(results, 'pressure', 1, 0, -1, 'point'), 4026.67220709878, rtol=1e-5)
    assert np.isclose(get_result(results, 'pressure', 2, 0, -1, 'point'), 4026.67220709878, rtol=1e-5)
    assert np.isclose(get_result(results, 'pressure', 1, -1, -1, 'point'), 4000.00, rtol=1e-5)
    assert np.isclose(get_result(results, 'pressure', 2, -1, -1, 'point'), 4000.00, rtol=1e-5)
    assert np.isclose(get_result(results, 'flow', 1, -1, -1, 'point'), 3.9925, rtol=1e-6)
    assert np.isclose(get_result(results, 'flow', 2, -1, -1, 'point'), 3.9925, rtol=1e-6)


def test_bifurcation_R(tmpdir, exePath):
    results = run_test_case_by_name('bifurcation_R', tmpdir, exePath)
    assert np.isclose(get_result(results, 'pressure', 0, 0, -1, 'point'), 3997.46433118937, rtol=1e-5)
    assert np.isclose(get_result(results, 'pressure', 0, -1, -1, 'point'), 3984.67700709878, rtol=1e-5)
    assert np.isclose(get_result(results, 'pressure', 1, 0, -1, 'point'), 3984.67700709878, rtol=1e-5)
    assert np.isclose(get_result(results, 'pressure', 2, 0, -1, 'point'), 3984.67700709878, rtol=1e-5)
    assert np.isclose(get_result(results, 'pressure', 1, -1, -1, 'point'), 3958.0048, rtol=1e-5)
    assert np.isclose(get_result(results, 'pressure', 2, -1, -1, 'point'), 3958.0048, rtol=1e-5)
    assert np.isclose(get_result(results, 'flow', 1, -1, -1, 'point'), 3.9925, rtol=1e-5)
    assert np.isclose(get_result(results, 'flow', 2, -1, -1, 'point'), 3.9925, rtol=1e-5)


def test_bifurcation_R_stab(tmpdir, exePath):
    results = run_test_case_by_name('bifurcation_R_stab', tmpdir, exePath)
    assert np.isclose(get_result(results, 'pressure', 0, 0, -1, 'point'), 3997.46433118937, rtol=1e-6)
    assert np.isclose(get_result(results, 'pressure', 0, -1, -1, 'point'), 3984.67700709878, rtol=1e-6)
    assert np.isclose(get_result(results, 'pressure', 1, 0, -1, 'point'), 3984.67700709878, rtol=1e-6)
    assert np.isclose(get_result(results, 'pressure', 2, 0, -1, 'point'), 3984.67700709878, rtol=1e-6)
    assert np.isclose(get_result(results, 'pressure', 1, -1, -1, 'point'), 3958.0048, rtol=1e-7)
    assert np.isclose(get_result(results, 'pressure', 2, -1, -1, 'point'), 3958.0048, rtol=1e-7)
    assert np.isclose(get_result(results, 'flow', 1, -1, -1, 'point'), 3.9925, rtol=1e-6)
    assert np.isclose(get_result(results, 'flow', 2, -1, -1, 'point'), 3.9925, rtol=1e-6)


def test_bifurcation_RCR(tmpdir, exePath):
    results = run_test_case_by_name('bifurcation_RCR', tmpdir, exePath)
    assert np.isclose(get_result(results, 'pressure', 0,  0, np.arange(100, 200), 'mean'), 123878.022943, rtol=1e-7)
    assert np.isclose(get_result(results, 'pressure', 0,  0, np.arange(100, 200), 'max'), 168182.372624, rtol=1e-7)
    assert np.isclose(get_result(results, 'pressure', 0,  0, np.arange(100, 200), 'min'), 89237.6441223, rtol=1e-7)
    assert np.isclose(get_result(results, 'pressure', 1, -1, np.arange(100, 200), 'mean'), 123855.677783, rtol=1e-7)
    assert np.isclose(get_result(results, 'pressure', 1, -1, np.arange(100, 200), 'max'), 171598.373528, rtol=1e-7)
    assert np.isclose(get_result(results, 'pressure', 1, -1, np.arange(100, 200), 'min'), 87624.0897929, rtol=1e-7)
    assert np.isclose(get_result(results, 'pressure', 2, -1, np.arange(100, 200), 'mean'), 123855.677783, rtol=1e-7)
    assert np.isclose(get_result(results, 'pressure', 2, -1, np.arange(100, 200), 'max'), 171598.373528, rtol=1e-7)
    assert np.isclose(get_result(results, 'pressure', 2, -1, np.arange(100, 200), 'min'), 87624.0897929, rtol=1e-7)
    assert np.isclose(get_result(results, 'flow', 0,  0, np.arange(100, 200), 'mean'), 7.557147487534, rtol=1e-7)
    assert np.isclose(get_result(results, 'flow', 1, -1, np.arange(100, 200), 'mean'), 3.839024865141, rtol=1e-7)
    assert np.isclose(get_result(results, 'flow', 1, -1, np.arange(100, 200), 'max'), 24.0553490482, rtol=1e-7)
    assert np.isclose(get_result(results, 'flow', 1, -1, np.arange(100, 200), 'min'), -3.35029015773, rtol=1e-7)


def test_bifurcation_RCR_staticFunc(tmpdir, exePath):
    results = run_test_case_by_name('bifurcation_RCR_staticFunc', tmpdir, exePath)
    assert np.isclose(get_result(results, 'pressure', 0, 0, -1, 'point'), 3997.46433118937, rtol=1e-6,)
    assert np.isclose(get_result(results, 'pressure', 0, -1, -1, 'point'), 3984.67700709878, rtol=1e-6,)
    assert np.isclose(get_result(results, 'pressure', 1, 0, -1, 'point'), 3984.67700709878, rtol=1e-6,)
    assert np.isclose(get_result(results, 'pressure', 2, 0, -1, 'point'), 3984.67700709878, rtol=1e-6,)
    assert np.isclose(get_result(results, 'pressure', 1, -1, -1, 'point'), 3958.0048, rtol=1e-7)
    assert np.isclose(get_result(results, 'pressure', 2, -1, -1, 'point'), 3958.0048, rtol=1e-7)
    assert np.isclose(get_result(results, 'flow', 1, -1, -1, 'point'), 3.9925, rtol=1e-6,)
    assert np.isclose(get_result(results, 'flow', 2, -1, -1, 'point'), 3.9925, rtol=1e-6,)
