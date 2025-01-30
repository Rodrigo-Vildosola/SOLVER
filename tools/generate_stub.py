import sys
import os
from pathlib import Path
from .stubgen import stub_parser_from_config, run
from .stubgen.printer import Printer
from .stubgen.writer import Writer

def generate_stub(module_name: str, pyi_path: str, so_dir: str) -> None:
    # Make sure Python can import the .so module by adding its directory to sys.path
    sys.path.insert(0, so_dir)

    config = {
        "ignore_all_errors": True,
        "ignore_invalid_identifiers": None,
        "ignore_invalid_expressions": None,
        "ignore_unresolved_names": None,
        "numpy_array_wrap_with_annotated": False,
        "numpy_array_use_type_var": True,
        "numpy_array_remove_parameters": False,
        "exit_code": False,
        "enum_class_locations": [],
        "print_safe_value_reprs": None,
    }


    # Create the parser with default settings
    parser = stub_parser_from_config(config)


    # Create the printer for stub generation
    printer = Printer(invalid_expr_as_ellipses=True)

    # Set the output directory
    out_dir = Path(so_dir)

    # Generate the stub
    run(
        parser=parser,
        printer=printer,
        module_name=module_name,
        out_dir=out_dir,
        sub_dir=None,
        dry_run=False,
        writer=Writer(stub_ext="pyi"),
    )

    # Move the generated .pyi file to the desired path
    generated_pyi = out_dir / f"{module_name}.pyi"
    if generated_pyi.exists():
        os.replace(generated_pyi, pyi_path)
    else:
        print(f"WARNING: Could not find the expected stub file: {generated_pyi}")
