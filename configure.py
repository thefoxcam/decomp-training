#!/usr/bin/env python3
import glob
import io
import os
import json
import sys
import platform
from tools.ninja_syntax import Writer, serialize_path
from pathlib import Path
from typing import Any, Dict, List, Optional, Set, Tuple, Union, cast

binutils_tag = "2.42-1"
compilers_tag = "20250513"
dtk_tag = "v1.0.0" # currently unused?
objdiff_tag = "v2.7.1"
sjiswrap_tag = "v1.1.1"
wibo_tag = "0.6.11"

linker_version = "GC/3.0a5.2"

target_src_dir = "training_answers"
base_src_dir = "training_template"

tools_dir = Path("tools")
build_dir = Path("build")
out_dir = "build"

target_build_dir = os.path.join(build_dir, "src", "target")
target_out_dir = os.path.join(out_dir, "src", "target")
base_build_dir = os.path.join(build_dir, "src", "base")
base_out_dir = os.path.join(out_dir, "src", "base")

def is_windows() -> bool:
    return os.name == "nt"

# On Windows, we need this to use && in commands
CHAIN = "cmd /c " if is_windows() else ""
# Native executable extension
EXE = ".exe" if is_windows() else ""

# TODO: Debug?
RELEASE_MWCC_FLAGS = [
    # System
    "-nodefaults",
    "-nosyspath",
    "-proc gekko",
    "-align powerpc",
    "-enum int",
    "-fp hardware",
    "-Cpp_exceptions off",
    '-pragma "cats off"',
    "-opt all",
    "-inline auto",
    # User
    "-i src/runtime",
    "-Isrc/shared",
]

TARGET_MWCC_FLAGS = \
    RELEASE_MWCC_FLAGS + [
    f"-Isrc/{target_src_dir}/main_content",
]

BASE_MWCC_FLAGS = \
    RELEASE_MWCC_FLAGS + [
    f"-Isrc/{base_src_dir}/main_content",
]

# TODO: Debug?
RELEASE_MWLD_FLAGS = [
    "-fp hard",
    "-nodefaults",
    "-mapunused",
    "-listclosure",
    "-lcf " + os.path.join("$build_dir", "ldscript.lcf"),
]

class BuildObject:
    def __init__(self, path: str, should_diff: bool, **options: Any) -> None:
        self.name = os.path.splitext(path)[0]
        self.file_path = path
        self.should_diff = should_diff
        if should_diff:
            self.target_path = os.path.join(target_src_dir, path)
            self.base_path = os.path.join(base_src_dir, path)
            self.target_obj = os.path.join(target_src_dir, self.name + ".o")
            self.base_obj = os.path.join(base_src_dir, self.name + ".o")
        else:
            self.target_path = path
            self.base_path = path
            self.target_obj = self.name + ".o"
            self.base_obj = self.name + ".o"
        self.options: Dict[str, Any] = {
            "mw_version": "GC/1.2.5n",
        }

build_objects = [
    BuildObject('main_content/00_basic_assembly_and_isa.c', True),
    BuildObject('main_content/01_abi_basics.c', True),
    BuildObject('runtime/runtime_core.c', False),
    BuildObject('runtime/runtime_exception.c', False),
    BuildObject('runtime/main.cpp', False),
    BuildObject('shared/stuff.c', False),
    BuildObject('shared/sample_functions.c', False),
]

def write_objdiff(build_objects: list) -> None:

    objdiff_config: Dict[str, Any] = {
        "min_version": "2.0.0-beta.5",
        "custom_make": "ninja",
        "build_target": False,
        "watch_patterns": [
            "*.c",
            "*.cp",
            "*.cpp",
            "*.h",
            "*.hpp",
            "*.inc",
            "*.py",
            "*.yml",
            "*.txt",
            "*.json",
        ],
        "units": [],
        "progress_categories": [],
    }

    # decomp.me compiler name mapping
    COMPILER_MAP = {
        "GC/1.0": "mwcc_233_144",
        "GC/1.1": "mwcc_233_159",
        "GC/1.2.5": "mwcc_233_163",
        "GC/1.2.5e": "mwcc_233_163e",
        "GC/1.2.5n": "mwcc_233_163n",
        "GC/1.3": "mwcc_242_53",
        "GC/1.3.2": "mwcc_242_81",
        "GC/1.3.2r": "mwcc_242_81r",
        "GC/2.0": "mwcc_247_92",
        "GC/2.5": "mwcc_247_105",
        "GC/2.6": "mwcc_247_107",
        "GC/2.7": "mwcc_247_108",
        "GC/3.0a3": "mwcc_41_51213",
        "GC/3.0a3.2": "mwcc_41_60126",
        "GC/3.0a3.3": "mwcc_41_60209",
        "GC/3.0a3.4": "mwcc_42_60308",
        "GC/3.0a5": "mwcc_42_60422",
        "GC/3.0a5.2": "mwcc_41_60831",
        "GC/3.0": "mwcc_41_60831",
        "Wii/1.0RC1": "mwcc_42_140",
        "Wii/0x4201_127": "mwcc_42_142",
        "Wii/1.0a": "mwcc_42_142",
        "Wii/1.0": "mwcc_43_145",
        "Wii/1.1": "mwcc_43_151",
        "Wii/1.3": "mwcc_43_172",
        "Wii/1.5": "mwcc_43_188",
        "Wii/1.6": "mwcc_43_202",
        "Wii/1.7": "mwcc_43_213",
    }

    for build_object in build_objects:
        if build_object.should_diff:
            compiler_version = COMPILER_MAP.get(build_object.options["mw_version"])
            if compiler_version is None:
                print(f"Missing scratch compiler mapping for {build_object.options['mw_version']}")
            else:
                unit_config = {
                  "name": build_object.file_path,
                  "target_path": os.path.join(target_build_dir, build_object.target_obj),
                  "base_path": os.path.join(base_build_dir, build_object.base_obj),
                  "scratch": {
                    "platform": "gc_wii",
                    "compiler": compiler_version,
                    "c_flags": " ".join(BASE_MWCC_FLAGS),
                    "ctx_path": os.path.join(base_build_dir, build_object.target_obj),
                    "build_ctx": True
                  },
                  "metadata": {
                    "complete": False,
                    "reverse_fn_order": False,
                    "source_path": os.path.join("src", build_object.target_path),
                    "auto_generated": False
                  }
                }
                objdiff_config["units"].append(unit_config)

    # Write objdiff.json
    with open("objdiff.json", "w", encoding="utf-8") as w:

        def unix_path(input: Any) -> str:
            return str(input).replace(os.sep, "/") if input else ""

        json.dump(objdiff_config, w, indent=4, default=unix_path)


out_buf = io.StringIO()
n = Writer(out_buf)

n.variable("ninja_required_version", "1.3")
n.newline()

n.variable("build_dir", build_dir)
n.variable("out_dir", out_dir)
n.newline()
n.variable("target_build_dir", target_build_dir)
n.variable("target_out_dir", target_out_dir)
n.variable("base_build_dir", base_build_dir)
n.variable("base_out_dir", base_out_dir)
n.newline()

n.variable("mw_version", Path(linker_version))

# The command line args and the ability to pass in an executable/compiler
# folder could be added if needed.

###
# Tooling
###
n.comment("Tooling")

build_tools_path = build_dir / "tools"

download_tool = tools_dir / "download_tool.py"
n.rule(
    name="download_tool",
    command=f"$python {download_tool} $tool $out --tag $tag",
    description="TOOL $out",
)

dtk = build_tools_path / f"dtk{EXE}"
n.build(
    outputs=dtk,
    rule="download_tool",
    implicit=download_tool,
    variables={
        "tool": "dtk",
        "tag": dtk_tag,
    },
)

objdiff = build_tools_path / f"objdiff-cli{EXE}"
n.build(
    outputs=objdiff,
    rule="download_tool",
    implicit=download_tool,
    variables={
        "tool": "objdiff-cli",
        "tag": objdiff_tag,
    },
)

sjiswrap = build_tools_path / "sjiswrap.exe"
n.build(
    outputs=sjiswrap,
    rule="download_tool",
    implicit=download_tool,
    variables={
        "tool": "sjiswrap",
        "tag": sjiswrap_tag,
    },
)

# Only add an implicit dependency on wibo if we download it
wrapper = None
wrapper_implicit: Optional[Path] = None
if (
    wibo_tag is not None
    and sys.platform == "linux"
    and platform.machine() in ("i386", "x86_64")
):
    wrapper = build_tools_path / "wibo"
    wrapper_implicit = wrapper
    n.build(
        outputs=wrapper,
        rule="download_tool",
        implicit=download_tool,
        variables={
            "tool": "wibo",
            "tag": wibo_tag,
        },
    )
if not is_windows() and wrapper is None:
    wrapper = Path("wine")
wrapper_cmd = f"{wrapper} " if wrapper else ""

compilers = build_dir / "compilers"
compilers_implicit = compilers
n.build(
    outputs=compilers,
    rule="download_tool",
    implicit=download_tool,
    variables={
        "tool": "compilers",
        "tag": compilers_tag,
    },
)

binutils = build_dir / "binutils"
binutils_implicit = binutils
n.build(
    outputs=binutils,
    rule="download_tool",
    implicit=download_tool,
    variables={
        "tool": "binutils",
        "tag": binutils_tag,
    },
)

n.newline()

###
# Helper rule for downloading all tools
###
n.comment("Download all tools")
n.build(
    outputs="tools",
    rule="phony",
    inputs=[dtk, sjiswrap, wrapper, compilers, binutils, objdiff],
)
n.newline()

###
# Build rules
###

compiler_path = compilers / "$mw_version"

# MWCC
mwcc = compiler_path / "mwcceppc.exe"
mwcc_cmd = f"{wrapper_cmd}{mwcc} $cflags -MMD -c $in -o $basedir"
mwcc_implicit: List[Optional[Path]] = [compilers_implicit or mwcc, wrapper_implicit]

mwld = compiler_path / "mwldeppc.exe"
mwld_cmd = f"{wrapper_cmd}{mwld} $ldflags -o $out @$out.rsp"
mwld_implicit: List[Optional[Path]] = [compilers_implicit or mwld, wrapper_implicit]

n.newline()

n.rule(
    "mwcc",
    command=mwcc_cmd,
    description="MWCC $out",
    depfile="$out.d",
    deps="gcc",
)
n.rule(
    "mwld",
    command=mwld_cmd,
    description="MWLD $out",
    rspfile="$out.rsp",
    rspfile_content="$in_newline",
)

n.comment("Generate DOL")
n.rule(
    name="elf2dol",
    command=f"{dtk} elf2dol $in $out",
    description="DOL $out",
)
n.newline()

# TODO: this signature is pretty bad
def write_build_object(out_files: list, in_file: str, input_build_dir: str, mwcc_flags: list, options: Dict[str, Any]):
    out_file = os.path.join(f"${input_build_dir}", os.path.splitext(in_file)[0] + ".o")
    out_files.append(out_file)

    n.build(
        outputs=out_file,
        rule="mwcc",
        inputs=os.path.join("src", in_file),
        variables={
            "cflags": " ".join(mwcc_flags),
            "basedir": os.path.join(f"${input_build_dir}", os.path.dirname(in_file)),
            "mw_version": options["mw_version"]
        },
        implicit=mwcc_implicit,
    )

def write_link(out_files: list, input_out_dir: str):
    n.build(
        outputs=os.path.join(f"${input_out_dir}", "main.elf"),
        rule="mwld",
        inputs=out_files,
        variables={"ldflags": " ".join(RELEASE_MWLD_FLAGS)},
        implicit=mwld_implicit,
    )
    
    n.build(
        outputs=os.path.join(f"${input_out_dir}", "main.dol"),
        rule="elf2dol",
        inputs=os.path.join(f"${input_out_dir}", "main.elf"),
        implicit=dtk,
    )

target_out_files = []
base_out_files = []

for build_object in build_objects:
    write_build_object(target_out_files, build_object.target_path, "target_build_dir", TARGET_MWCC_FLAGS, build_object.options)
    write_build_object(base_out_files, build_object.base_path, "base_build_dir", BASE_MWCC_FLAGS, build_object.options)

write_link(target_out_files, "target_out_dir")
write_link(base_out_files, "base_out_dir")

write_objdiff(build_objects)

with open("build.ninja", "w") as out_file:
    out_file.write(out_buf.getvalue())
n.close()
