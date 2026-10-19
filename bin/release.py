#!/usr/bin/env python

if __name__ == "__main__":
    import os.path
    import re
    import subprocess
    import sys


    # The version in the project()-call of meson.build: version: '<major>.<minor>.<patch>'
    VERSION_REGEX = re.compile(r"^(\s*version:\s*')(\d+)\.(\d+)\.(\d+)(')", re.MULTILINE)
    PROJECT_REGEX = re.compile(r"^project\(\s*'([^']+)'", re.MULTILINE)


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


    def main(args):
        if len(args) > 2:
            print("Invalid number of arguments", file=sys.stderr)
            print("Usage:", sys.argv[0], "[COMMENT]", file=sys.stderr)
            return -1

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
        project = project[1]
        major, minor, patch = [int(part) for part in version.group(2, 3, 4)]

        info = f"Version {major}.{minor}.{patch}"
        if len(args) > 1:
            info = ": ".join([info, args[1]])

        # Named like expected by the release-jobs in .gitlab-ci.yml
        tag = f"{project}_V{major}.{minor}.{patch}"

        try:
            remote = _execute("git", "config", f"branch.{current_branch}.remote")[0].strip()
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
            _execute("git", "commit", "-m", f"meson.build: Increased version to {major}.{minor}.{patch}")
            _execute("git", "push", "-o", "ci.skip")
        except ExecuteError as err:
            print(err, file=sys.stderr)
            return 1
        return 0

    sys.exit(main(sys.argv))
