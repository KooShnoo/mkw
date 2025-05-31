"""
Script for checking that headers can be compiled with clang.
"""


from pathlib import Path
import subprocess
import sys


COMMONFLAGS = [
    "-c",
    "-fshort-wchar",
    
    "-I", "./include",
    "-I", "./src",
    "-I", "lib/MSL/include",
    "-I", "lib/MSL/src",
    "-I", "lib",  # just for now, individual include directories for each lib is tidier
    
    "-nostdinc",
    "-target", "ppc-none-eabi",
    "-Wall",
    "-Wno-unused-private-field",
    "-Wextra",
    "-Wpedantic",
    "-Wno-c99-extensions",
    "-Wno-gnu-anonymous-struct",
    "-Wno-nested-anon-types",
    "-Werror",
]

CFLAGS = COMMONFLAGS + [
    "-x", "c",
    "-std=c18",
]

CXXFLAGS = COMMONFLAGS + [
    "-fno-exceptions",
    "-x", "c++",
]

if sys.platform == "win32" or sys.platform == "msys":
    DEV_NULL = "NUL"
else:
    DEV_NULL = "/dev/null"


def check_headers(extension, flags):
    source = ""
    for path in Path("src").rglob("*." + extension):
        source += "#include \""
        source += str(path).removeprefix("src/")
        source += "\"\n"

    # print(f"for ext {extension} running sources {source} cmd:",["clang"] + flags + ["-", "-o", DEV_NULL])
    process = subprocess.run(
        ["clang"] + flags + ["-", "-o", DEV_NULL],
        text=True,
        input=source,
    )

    # print(f"rab w {process.returncode} exitcode")
    return process.returncode

c_ret = check_headers("h", CFLAGS)
cpp_ret = check_headers("hpp", CXXFLAGS)

if c_ret != 0 or cpp_ret != 0:
    sys.exit(1)
