import os
import subprocess as sp
import sys

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))


def run(cmd):
    print(" ".join(cmd))
    sp.check_call(cmd)


def run_tests(crc32c_sw_mode):
    os.environ["CRC32C_SW_MODE"] = crc32c_sw_mode
    message = "# Tests for CRC32C_SW_MODE: %s #" % crc32c_sw_mode
    hashes = '#' * len(message)
    print("\n" + hashes)
    print(message)
    print(hashes + "\n")
    run(
        [
            sys.executable,
            "-c",
            ("import crc32c;"
             "print('Is big endian? ', crc32c.big_endian);"
             "print('Is hardware based? ', crc32c.hardware_based);")
        ]
    )
    run([sys.executable, "-m", "pytest", "-v", os.path.join(SCRIPT_DIR, "test")])
    run(
        [
            sys.executable,
            "-c",
            "import crc32c; import time; x = b' ' * int(1e8); n = 10; s = time.time(); [crc32c.crc32c(x) for _ in range(n)]; print('Ran at %.3f [GB/s]' % (n/10/(time.time() - s),))",
        ]
    )


def main():
    run_tests("auto")
    run_tests("force")


if __name__ == "__main__":
    main()
