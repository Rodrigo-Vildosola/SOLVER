
from dataclasses import dataclass, field
from typing import Any, List

@dataclass
class TestCase:
    description: str
    expression: str
    expected_result: Any = None
    margin: float = 1e-6
    expect_exception: Any = None
    debug: bool = False
    setup_variables: dict = field(default_factory=dict)  # New field
    passed: bool = False
    error_message: str = ""

@dataclass
class TestSuite:
    name: str
    test_cases: List[TestCase] = field(default_factory=list)
