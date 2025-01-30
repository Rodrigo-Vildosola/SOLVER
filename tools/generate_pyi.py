import inspect
import importlib
import re
import sys

# Utility function to extract signatures from docstrings
def extract_signature_from_doc(doc: str, module_name: str):
    if not doc:
        return None

    match = re.search(r"\((.*?)\)", doc)
    if match:
        signature = f"({match.group(1)})"
        # Clean up the module name from the extracted signature
        return signature.replace(f"{module_name}.", "")
    return None

def extract_return_type_from_doc(doc: str, module_name: str):
    """Extract the return type from a docstring if available."""
    if not doc:
        return None
    # Adjust regex to ensure we only capture the return type before any additional text
    match = re.search(r"->\s*([\w\[\]]+(?:, [\w\[\]]+)*\.*)\s*\n", doc)
    if match:
        return_type = match.group(1).strip()
        # Clean up the module name from the extracted type
        return clean_signature(return_type, module_name)
    return None


# Utility function to clean up fully qualified module names
def clean_signature(signature: str, module_name: str) -> str:
    return signature.replace(f"{module_name}.", "")

# Function to clean up docstrings by removing fully qualified module names
def clean_docstring(docstring: str, module_name: str) -> str:
    if not docstring:
        return docstring
    # Remove occurrences of fully qualified module names
    return docstring.replace(f"{module_name}.", "")


def generate_pyi(module_name: str, output_file: str, module_dir: str):
    print(f"-- Generating .pyi for module: {module_name}")
    
    # Temporarily add the module directory to sys.path
    original_sys_path = sys.path[:]
    sys.path.insert(0, module_dir)

    try:
        module = importlib.import_module(module_name)
        print(f"-- Successfully imported module: {module_name}")

        classes = [(name, obj) for name, obj in inspect.getmembers(module) if inspect.isclass(obj)]
        functions = [(name, obj) for name, obj in inspect.getmembers(module) if inspect.isroutine(obj)]

        with open(output_file, "w") as f:
                        # Write bare functions
            for func_name, func in functions:
                try:
                    signature = str(inspect.signature(func))
                except ValueError:
                    # Fallback if we cannot get the signature directly
                    docstring = getattr(func, "__doc__", None)
                    signature = extract_signature_from_doc(docstring, module_name) or "(...)"
                
                # Get the docstring
                func_doc = clean_docstring(getattr(func, "__doc__", None), module_name)
                
                # Extract return type from docstring if present
                return_type = extract_return_type_from_doc(func_doc, module_name) or "Any"

                # Write the function definition
                if return_type != "Any":
                    f.write(f"def {func_name}{signature} -> {return_type}:\n")
                else:
                    f.write(f"def {func_name}{signature}:\n")
                if func_doc:
                    f.write(f'    """{func_doc.strip()}"""\n')
                f.write("    ...\n\n")

            for i, (name, obj) in enumerate(classes, start=1):
                # Write class definition
                f.write(f"class {name}:\n")
                
                # Retrieve class docstring
                class_doc = getattr(obj, "__doc__", None)
                if class_doc:
                    f.write(f'    """{class_doc.strip()}"""\n')
                
                # Write methods
                for func_name, func in inspect.getmembers(obj, predicate=inspect.isroutine):
                    try:
                        signature = str(inspect.signature(func))
                    except ValueError:
                        docstring = getattr(func, "__doc__", None)
                        signature = extract_signature_from_doc(docstring, module_name) or "(self, *args, **kwargs)"
                    
                    # Get the docstring
                    method_doc = clean_docstring(getattr(func, "__doc__", None), module_name)
                    
                    # Extract return type from docstring if present
                    return_type = extract_return_type_from_doc(method_doc, module_name) or "Any"
                    
                    # Write the method definition with return type (if it's not Any)
                    if return_type != "Any":
                        f.write(f"    def {func_name}{signature} -> {return_type}:\n")
                    else:
                        f.write(f"    def {func_name}{signature}:\n")
                    if method_doc:
                        f.write(f'        """{method_doc.strip()}"""\n')
                    f.write("        ...\n")

                f.write("\n")

                # Write properties for data descriptors
                for member_name, member in inspect.getmembers(obj, predicate=inspect.isdatadescriptor):
                    descriptor_doc = getattr(member, "__doc__", None)
                    f.write(f"    @property\n")
                    f.write(f"    def {member_name}(self):\n")
                    if descriptor_doc:
                        f.write(f'        """{clean_docstring(descriptor_doc.strip(), module_name)}"""\n')
                    f.write("        ...\n")

        print(f"-- .pyi file successfully generated: {output_file}")
    except ImportError as e:
        print(f"-- Failed to import module '{module_name}': {e}")
    finally:
        # Restore the original sys.path
        sys.path = original_sys_path
