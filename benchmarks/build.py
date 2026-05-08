#!/usr/bin/env python3

import argparse
import os
from pathlib import Path
import shutil
import subprocess
import sys

def run_command(cmd, cwd=None, error_msg="Error"):
    result = subprocess.run(cmd, cwd=cwd, shell=False)
    if result.returncode != 0:
        print(f"ERROR: {error_msg}")
        sys.exit(1)

def build(project_root, script_dir, tag):
    build_path = project_root / "build"
    engines_dir = script_dir / "engines"

    try:
        # if build_path.exists():
        #     shutil.rmtree(build_path)

        run_command([
            "cmake", "-G", "Ninja",
            "-S", str(project_root),
            "-B", str(build_path),
            "-DCMAKE_BUILD_TYPE=Release"
        ], error_msg="CMake configuration failed")

        run_command([
            "cmake", "--build", str(build_path),
            "--target", "cli",
            "--config", "Relase",
            "-j", str(os.cpu_count() or 1)
        ], error_msg="Build failed")

        engines_dir.mkdir(parents=True, exist_ok=True)
        source_binary = build_path / "src" / "cli" / "cli"
        shutil.copy2(source_binary, engines_dir / tag)

    finally:
        print("Returning to original branch...")

def main():

    script_dir = Path(__file__).parent.absolute()
    project_root = script_dir.parent

    parser = argparse.ArgumentParser(
        description="Build tool for chess engine supporting git history"
    )
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument("git_tag", nargs="?", help="The git tag of the engine version")
    group.add_argument("--current", action="store_true", help="Build the current version")
    args = parser.parse_args()

    def build_git():
        if subprocess.check_output(["git", "status", "--porcelain"]).strip():
            print("Uncommitted changes detected")
            sys.exit(1)
        
        run_command(
            ["git", "checkout", f"tags/{args.git_tag}", "--detach"], 
            error_msg="Git checkout failed"
        )

        build(project_root, script_dir, args.git_tag)
        run_command(["git", "checkout", "-"], error_msg="Returning failed")

    def build_current():
        build(project_root, script_dir, "dev")

    if args.current:
        print("Building current version")
        build_current()
    else:
        print(f"Building from Git Tag: {args.git_tag}")
        build_git()

    print("Finish")

if __name__ == "__main__":
    main()