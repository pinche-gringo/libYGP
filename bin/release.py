#!/usr/bin/env python

if __name__ == "__main__":
    import argparse
    import os.path
    import re
    import subprocess
    import sys


    # The version in the project()-call of meson.build: version: '<major>.<minor>.<patch>'
    VERSION_REGEX = re.compile(r"^(\s*version:\s*')(\d+)\.(\d+)\.(\d+)(')", re.MULTILINE)
    PROJECT_REGEX = re.compile(r"^project\(\s*'([^']+)'", re.MULTILINE)
    # A package name differing from the project name: package_name = '<name>' or
    # <conf>.set_quoted('PACKAGE_NAME', '<name>'|<variable>)
    PACKAGE_REGEX = re.compile(r"^\s*package_name\s*=\s*'([^']+)'", re.MULTILINE)
    CONF_PACKAGE_REGEX = re.compile(r"\.set_quoted\(\s*'PACKAGE_NAME'\s*,\s*(?:'([^']+)'|(\w+)\s*\))")


    def _package_name(content):
        """Returns the package name defined in the content of meson.build (or None)"""
        if match := PACKAGE_REGEX.search(content):
            return match[1]
        if match := CONF_PACKAGE_REGEX.search(content):
            if match[1]:
                return match[1]
            # Value is a variable: Use its (string-)assignment
            if value := re.search(fr"^\s*{match[2]}\s*=\s*'([^']+)'", content, re.MULTILINE):
                return value[1]
        return None


    class ExecuteError(Exception):
        pass


    def _execute(*args):
        try:
            with subprocess.Popen(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE) as proc:
                try:
                    stdout, stderr = proc.communicate()
                except OSError as err:
                    raise ExecuteError(f"Error communicating with {args[0]}") from err
        except (OSError, FileNotFoundError) as err:
            raise ExecuteError(f"File '{args[0]} not found") from err

        stdout, stderr = [output.decode(sys.stdout.encoding) for output in (stdout, stderr)]
        if proc.returncode:
            raise ExecuteError("\n".join([f"Error {proc.returncode} running {args[0]}", stdout, stderr]))
        return stdout, stderr


    def main():
        parser = argparse.ArgumentParser(
            description="Tags the version in meson.build, pushes the tag and increases the version")
        parser.add_argument("-r", "--remote",
                            help="Git remote to push to (default: the remote of the current branch)")
        parser.add_argument("comment", nargs="?", help="Additional text for the tag-message")
        args = parser.parse_args()

        try:
            current_branch = _execute("git", "symbolic-ref", "--short", "-q", "HEAD")[0].strip()
            topdir = _execute("git", "rev-parse", "--show-toplevel")[0].strip()
        except ExecuteError as err:
            print(err, file=sys.stderr)
            return 1
        if not re.match(r"V\d+\.\d+$", current_branch):
            print(f"The active branch '{current_branch}' is not a release branch", file=sys.stderr)
            return -2

        # The tag must match the committed version, so meson.build mustn't have local changes
        mesonfile = os.path.join(topdir, "meson.build")
        try:
            _execute("git", "diff", "--quiet", "HEAD", "--", mesonfile)
        except ExecuteError:
            print(f"{mesonfile} has uncommitted changes", file=sys.stderr)
            return -2

        with open(mesonfile, encoding="utf-8") as fobj:
            content = fobj.read()

        project = PROJECT_REGEX.search(content)
        version = VERSION_REGEX.search(content)
        if not project or not version:
            print(f"No project-name or version (<major>.<minor>.<patch>) found in {mesonfile}",
                  file=sys.stderr)
            return -2
        project = _package_name(content) or project[1]
        major, minor, patch = [int(part) for part in version.group(2, 3, 4)]

        info = f"Version {major}.{minor}.{patch}"
        if args.comment:
            info = ": ".join([info, args.comment])

        # Named like expected by the release-jobs in .gitlab-ci.yml
        tag = f"{project}_V{major}.{minor}.{patch}"

        remote = args.remote
        try:
            if not remote:
                remote = _execute("git", "config", f"branch.{current_branch}.remote")[0].strip()
        except ExecuteError:
            print(f"Branch '{current_branch}' has no remote; specify one with -r", file=sys.stderr)
            return -2
        try:
            _execute("git", "remote", "get-url", remote)
        except ExecuteError:
            print(f"Remote '{remote}' doesn't exist", file=sys.stderr)
            return -2

        try:
            _execute("git", "tag", tag, "-m", info)
            # Push only the new tag; GitLab doesn't create pipelines for more than 4 tags at once
            _execute("git", "push", remote, f"refs/tags/{tag}")
        except ExecuteError as err:
            print(err, file=sys.stderr)
            return 1

        patch += 1
        content = VERSION_REGEX.sub(fr"\g<1>{major}.{minor}.{patch}\g<5>", content, count=1)
        with open(mesonfile, "wb") as fobj:
            fobj.write(content.encode("utf-8"))

        try:
            _execute("git", "add", mesonfile)
            _execute("git", "commit", "-m", f"meson.build: Bump version to {major}.{minor}.{patch}")
            _execute("git", "push", "-o", "ci.skip", remote, current_branch)
        except ExecuteError as err:
            print(err, file=sys.stderr)
            return 1
        return 0

    sys.exit(main())
