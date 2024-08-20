import os
import subprocess as sp
import sys

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))


def run(cmd):
    print(" ".join(cmd))
    sp.check_call(cmd)


def run_tests(crc32c_sw_mode):
    os.environ["CRC32C_SW_MODE"] = crc32c_sw_mode
    os.environ["CRC32C_SKIP_HW_PROBE"] = ["0", "1"][crc32c_sw_mode == "none"]
    message = "# Tests for CRC32C_SW_MODE: %s #" % crc32c_sw_mode
    hashes = "#" * len(message)
    print("\n" + hashes)
    print(message)
    print(hashes + "\n")
    run([sys.executable, "-m", "pytest", "-v", os.path.join(SCRIPT_DIR, "test")])


def main():
    run_tests("auto")
    run_tests("force")
    run_tests("none")


if __name__ == "__main__":
    main()
