from pollycracker import PCCircuit


@PCCircuit
def or_gate(one, a, b):
    t0 = one + a
    t1 = one + b
    t2 = t0 * t1
    o = one + t2
    return o


@PCCircuit
def two_bit_adder(one, a0, a1, b0, b1):
    t0 = a0 * b0
    t1 = a1 + b1
    t2 = a1 * b1
    t3 = t0 * t1

    o0 = a0 + b0
    o1 = t0 + t1
    o2 = or_gate(one, t2, t3)

    return o2, o1, o0


class Bit:
    """
    Helper class to test circuits
    """

    def __init__(self, value):
        self.value = bool(value)

    def __add__(self, other):
        return Bit(self.value ^ other.value)

    def __mul__(self, other):
        return Bit(self.value and other.value)

    def __repr__(self):
        return '1' if self.value else '0'
