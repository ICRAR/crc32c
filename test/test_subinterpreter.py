import contextlib
from typing import Iterable, Tuple, no_type_check

import pytest

import crc32c

try:
    from test.support.interpreters import Interpreter  # type: ignore
    from test.support.interpreters import create, queues
except ImportError:
    pytest.skip("No sub-interpreter support", allow_module_level=True)


@pytest.fixture(name="interpreter")
def interpreter_fixture() -> Iterable[Interpreter]:
    interpreter = create()
    with contextlib.closing(interpreter):
        yield interpreter


def test_crc32c_can_be_loaded(interpreter: Interpreter) -> None:
    interpreter.exec("import crc32c")
    # all good, no exception raised


@pytest.mark.calculates_crc32c
def test_crc32c_can_run(interpreter: Interpreter) -> None:

    queue = queues.create()
    VALUE = b"test"
    interpreter.prepare_main(queue_id=queue.id, value_in=VALUE)

    @no_type_check
    def crc32c_in_subinterpreter() -> None:
        from test.support.interpreters.queues import Queue  # type: ignore

        import crc32c

        queue = Queue(queue_id)
        value_out = crc32c.crc32c(value_in)
        queue.put(value_out)

    thread = interpreter.call_in_thread(crc32c_in_subinterpreter)
    result = queue.get(timeout=5)
    thread.join()
    assert result == crc32c.crc32c(VALUE)
