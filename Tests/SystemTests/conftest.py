import pytest
import numpy as np

from tempfile import TemporaryDirectory

@pytest.fixture
def tempdir():
    """Temporary directory for test purposes."""
    with TemporaryDirectory() as tempdir:
        yield tempdir

def pytest_addoption(parser):
    parser.addoption("--relativeExePath", action="store")
    