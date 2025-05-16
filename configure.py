#!/usr/bin/env python3
import glob
import io
import os
import json
from vendor.ninja_syntax import Writer
from typing import Any

target_src_dir = "training_answers"
base_src_dir = "training_template"

builddir = "build"
outdir = "build"

target_builddir = os.path.join(builddir, "target")
target_outdir = os.path.join(outdir, "target")
base_builddir = os.path.join(builddir, "base")
base_outdir = os.path.join(outdir, "base")

# TODO: Debug?
RELEASE_MWCC_FLAGS = [
    # System
    "-nodefaults",
    "-nosyspath",
    "-proc gekko",
    "-align powerpc",
    "-enum int",
    "-enc SJIS",
    "-fp hardware",
    "-Cpp_exceptions off",
    '-pragma "cats off"',
    "-ipa file",
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
    "-lcf " + os.path.join("$builddir", "ldscript.lcf"),
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
            "mw_version": "GC/3.0a5.2",
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
                  "target_path": os.path.join(target_builddir, build_object.target_obj),
                  "base_path": os.path.join(base_builddir, build_object.base_obj),
                  "scratch": {
                    "platform": "gc_wii",
                    "compiler": compiler_version,
                    "c_flags": " ".join(BASE_MWCC_FLAGS),
                    "ctx_path": os.path.join(base_builddir, build_object.target_obj),
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

n.variable("builddir", builddir)
n.variable("outdir", outdir)
n.newline()
n.variable("target_builddir", target_builddir)
n.variable("target_outdir", target_outdir)
n.variable("base_builddir", base_builddir)
n.variable("base_outdir", base_outdir)
n.newline()


# TODO: The non-Windows people aren't gonna be happy about this one
# NOTE: Perhaps DDD has the answer to this
n.variable("compiler", os.path.join("$builddir", "compiler", "mwcceppc.exe"))
n.variable("linker", os.path.join("$builddir", "compiler", "mwldeppc.exe"))
n.variable("dtk", os.path.join("$builddir", "dtk.exe"))
n.newline()

n.rule(
    "mwcc",
    command="$compiler $cflags -MMD -c $in -o $basedir",
    description="MWCC $out",
    depfile="$out.d",
    deps="gcc",
)
n.rule(
    "mwld",
    command="$linker $ldflags -o $out @$out.rsp",
    description="MWLD $out",
    rspfile="$out.rsp",
    rspfile_content="$in_newline",
)
n.rule("dol", command="$dtk elf2dol $in $out", description="DOL $out")

def write_build_object(out_files: list, in_file: str, input_builddir: str, mwcc_flags: list):
    out_file = os.path.join(f"${input_builddir}", os.path.splitext(in_file)[0] + ".o")
    out_files.append(out_file)

    n.build(
        outputs=out_file,
        rule="mwcc",
        inputs=os.path.join("src", in_file),
        variables={
            "cflags": " ".join(mwcc_flags),
            "basedir": os.path.join(f"${input_builddir}", os.path.dirname(in_file)),
        },
    )

def write_link(out_files: list, input_outdir: str):
    n.build(
        outputs=os.path.join(f"${input_outdir}", "main.elf"),
        rule="mwld",
        inputs=out_files,
        variables={"ldflags": " ".join(RELEASE_MWLD_FLAGS)},
    )
    
    n.build(
        outputs=os.path.join(f"${input_outdir}", "main.dol"),
        rule="dol",
        inputs=os.path.join(f"${input_outdir}", "main.elf"),
    )

target_out_files = []
base_out_files = []

for build_object in build_objects:
    write_build_object(target_out_files, build_object.target_path, "target_builddir", TARGET_MWCC_FLAGS)
    write_build_object(base_out_files, build_object.base_path, "base_builddir", BASE_MWCC_FLAGS)

write_link(target_out_files, "target_outdir")
write_link(base_out_files, "base_outdir")

write_objdiff(build_objects)

with open("build.ninja", "w") as out_file:
    out_file.write(out_buf.getvalue())
n.close()
