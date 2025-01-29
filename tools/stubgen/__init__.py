from __future__ import annotations

import importlib
import logging
import re
from argparse import ArgumentParser, Namespace
from pathlib import Path

from .parser.interface import IParser
from .parser.mixins.error_handlers import (
    IgnoreAllErrors,
    IgnoreInvalidExpressionErrors,
    IgnoreInvalidIdentifierErrors,
    IgnoreUnresolvedNameErrors,
    LogErrors,
    LoggerData,
    SuggestCxxSignatureFix,
    TerminateOnFatalErrors,
)
from .parser.mixins.filter import (
    FilterClassMembers,
    FilterInvalidIdentifiers,
    FilterPybind11ViewClasses,
    FilterPybindInternals,
    FilterTypingModuleAttributes,
)
from .parser.mixins.fix import (
    FixBuiltinTypes,
    FixCurrentModulePrefixInTypeNames,
    FixMissing__all__Attribute,
    FixMissing__future__AnnotationsImport,
    FixMissingEnumMembersAnnotation,
    FixMissingFixedSizeImport,
    FixMissingImports,
    FixMissingNoneHashFieldAnnotation,
    FixNumpyArrayDimAnnotation,
    FixNumpyArrayDimTypeVar,
    FixNumpyArrayFlags,
    FixNumpyArrayRemoveParameters,
    FixNumpyDtype,
    FixPEP585CollectionNames,
    FixPybind11EnumStrDoc,
    FixRedundantBuiltinsAnnotation,
    FixRedundantMethodsFromBuiltinObject,
    FixScipyTypeArguments,
    FixTypingTypeNames,
    FixValueReprRandomAddress,
    OverridePrintSafeValues,
    RemoveSelfAnnotation,
    ReplaceReadWritePropertyWithField,
    RewritePybind11EnumValueRepr,
)
from .parser.mixins.parse import (
    BaseParser,
    ExtractSignaturesFromPybind11Docstrings,
    ParserDispatchMixin,
)
from .printer import Printer
from .structs import QualifiedName
from .writer import Writer


def stub_parser_from_config(config: dict) -> IParser:
    """
    Constructs and returns a parser instance based on the provided configuration dictionary.

    Args:
        config (dict): Configuration dictionary with parsing options.

    Returns:
        IParser: A parser instance configured with the provided options.
    """
    error_handlers_top: list[type] = [
        LoggerData,
        *([IgnoreAllErrors] if config.get("ignore_all_errors", False) else []),
        *([IgnoreInvalidIdentifierErrors] if config.get("ignore_invalid_identifiers") else []),
        *([IgnoreInvalidExpressionErrors] if config.get("ignore_invalid_expressions") else []),
        *([IgnoreUnresolvedNameErrors] if config.get("ignore_unresolved_names") else []),
    ]

    error_handlers_bottom: list[type] = [
        LogErrors,
        SuggestCxxSignatureFix,
        *([TerminateOnFatalErrors] if config.get("exit_code", False) else []),
    ]

    numpy_fixes: list[type] = [
        *([FixNumpyArrayDimAnnotation] if config.get("numpy_array_wrap_with_annotated", False) else []),
        *([FixNumpyArrayDimTypeVar] if config.get("numpy_array_use_type_var", False) else []),
        *(
            [FixNumpyArrayRemoveParameters]
            if config.get("numpy_array_remove_parameters", False)
            else []
        ),
    ]

    class Parser(
        *error_handlers_top,  # type: ignore[misc]
        FixMissing__future__AnnotationsImport,
        FixMissing__all__Attribute,
        FixMissingNoneHashFieldAnnotation,
        FixMissingImports,
        FilterTypingModuleAttributes,
        FixPEP585CollectionNames,
        FixTypingTypeNames,
        FixScipyTypeArguments,
        FixMissingFixedSizeImport,
        FixMissingEnumMembersAnnotation,
        OverridePrintSafeValues,
        *numpy_fixes,  # type: ignore[misc]
        FixNumpyDtype,
        FixNumpyArrayFlags,
        FixCurrentModulePrefixInTypeNames,
        FixBuiltinTypes,
        RewritePybind11EnumValueRepr,
        FilterClassMembers,
        ReplaceReadWritePropertyWithField,
        FilterInvalidIdentifiers,
        FixValueReprRandomAddress,
        FixRedundantBuiltinsAnnotation,
        FilterPybindInternals,
        FilterPybind11ViewClasses,
        FixRedundantMethodsFromBuiltinObject,
        RemoveSelfAnnotation,
        FixPybind11EnumStrDoc,
        ExtractSignaturesFromPybind11Docstrings,
        ParserDispatchMixin,
        BaseParser,
        *error_handlers_bottom,  # type: ignore[misc]
    ):
        pass

    parser = Parser()

    # Apply configurations to the parser
    if "enum_class_locations" in config and config["enum_class_locations"]:
        parser.set_pybind11_enum_locations(dict(config["enum_class_locations"]))
    if "ignore_invalid_identifiers" in config and config["ignore_invalid_identifiers"] is not None:
        parser.set_ignored_invalid_identifiers(config["ignore_invalid_identifiers"])
    if "ignore_invalid_expressions" in config and config["ignore_invalid_expressions"] is not None:
        parser.set_ignored_invalid_expressions(config["ignore_invalid_expressions"])
    if "ignore_unresolved_names" in config and config["ignore_unresolved_names"] is not None:
        parser.set_ignored_unresolved_names(config["ignore_unresolved_names"])
    if "print_safe_value_reprs" in config and config["print_safe_value_reprs"] is not None:
        parser.set_print_safe_value_pattern(config["print_safe_value_reprs"])

    return parser


def run(
    parser: IParser,
    printer: Printer,
    module_name: str,
    out_dir: Path,
    sub_dir: Path | None,
    dry_run: bool,
    writer: Writer,
):
    module = parser.handle_module(
        QualifiedName.from_str(module_name), importlib.import_module(module_name)
    )
    parser.finalize()

    if module is None:
        raise RuntimeError(f"Can't parse {module_name}")

    if dry_run:
        return

    out_dir.mkdir(exist_ok=True, parents=True)
    writer.write_module(module, printer, to=out_dir, sub_dir=sub_dir)
