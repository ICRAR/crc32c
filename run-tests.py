import os
import subprocess as sp
import sys

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))


def run(cmd):
    print(" ".join(cmd))
    sp.check_call(cmd)


def run_tests():
    run([sys.executable, "-mpytest", os.path.join(SCRIPT_DIR, "test")])
    run(
        [
            sys.executable,
            "-c",
            "import crc32c; import time; x = b' ' * int(1e9); n = 10; s = time.time(); [crc32c.crc32c(x) for _ in range(n)]; print('Ran at %.3f [GB/s]' % (n/(time.time() - s),))",
        ]
    )


def main():
    run_tests()
    os.environ["CRC32C_SW_MODE"] = "force"
    run_tests()


if __name__ == "__main__":
    main()
