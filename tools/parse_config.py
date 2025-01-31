# parse_config.py
import json
import os
from dataclasses import dataclass, field
from typing import List, Optional

@dataclass
class BuildConfig:
    library_name: str
    author: str
    description: str
    url: str

    version_file: str
    readme_file: str

    install_requires: List[str]
    classifiers: List[str]
    python_requires: str
    zip_safe: bool

    # Optional fields with defaults
    build_directory: str = ".build"
    extra_directories: List[str] = field(default_factory=list)
    tests_directory: Optional[str] = None
    headers_directory: str = "include"
    source_directory: str = "str"
    docstrings_file: str = "docstrings.h"

    root_directory: str = "."


    @staticmethod
    def load_from_file(path: str = "build_config.json") -> "BuildConfig":
        """
        Loads and validates config data from a JSON file, then returns a BuildConfig instance.
        """
        if not os.path.exists(path):
            raise FileNotFoundError(f"Could not find config file: {path}")

        with open(path, "r") as f:
            data = json.load(f)

        data.pop("$schema", None)

        # Perform minimal validation or set defaults if needed
        required_keys = {
            "library_name", "author", "description", "url",
            "version_file", "readme_file",
            "install_requires", "classifiers", "python_requires",
            "zip_safe"
            # 'build_directory', 'extra_directories', 'tests_directory' now optional
        }
        missing = required_keys - data.keys()
        if missing:
            raise ValueError(f"Missing required keys in config: {missing}")

        # Check existence of the version/readme files
        if not os.path.exists(data["version_file"]):
            raise FileNotFoundError(f"Version file not found: {data['version_file']}")
        if not os.path.exists(data["readme_file"]):
            raise FileNotFoundError(f"Readme file not found: {data['readme_file']}")

        # Convert the dictionary into a BuildConfig instance
        return BuildConfig(**data)
