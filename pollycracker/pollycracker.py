import abc
import math
import random

from colorama import Fore, Style
from Crypto.Util import number

_verbose = False


def set_verbose(val):
    """enable/disable verbose output"""
    global _verbose
    _verbose = bool(val)


def COL(s, color):
    return f'{color}{s}{Fore.WHITE}'


class Key:
    """
    Wrapper class for the key (a single prime number).
    Intended to support pretty console output of the key.
    """

    def __init__(self, bits):
        self.key = number.getPrime(bits)

    def new(self, bits):
        self.key = number.getPrime(bits)
        return self

    def __call__(self):
        """Get the actual integer value of the key"""
        return self.key

    def __repr__(self):
        return f'{Style.BRIGHT}{COL("p", Fore.YELLOW)} = {self.key}{Style.RESET_ALL}'


class PCCyphertextList(list):
    """
    A list type to hold multiple encrypted bits.
    Supports vectorized decryption via the `.decrypt()` method.
    """

    def decrypt(self):
        return list(map(lambda c: c.decrypt(), self))


def PCCircuit(func):
    """
    Decorator that ensures that a function returning a `tuple`/`list` of cypertexts
    will actually return a `PCCyphertextList` instead.
    """
    def wrap(*args, **kwargs):
        r = func(*args, **kwargs)
        if isinstance(r, (tuple, list)) and all(map(lambda x: isinstance(x, PCCyphertextBase), r)):
            return PCCyphertextList(r)
        return r

    return wrap


class PCCyphertextBase(abc.ABC):
    """
    Abstract base class for all cyphertext objects.
    The objects hold the key needed for decryption.
    """

    def __init__(self, key):
        super().__init__()
        self.key = key

    @abc.abstractmethod
    def _get_m(self):
        pass

    @abc.abstractmethod
    def _get_r(self):
        pass

    @abc.abstractmethod
    def _get_m_repr(self):
        pass

    @abc.abstractmethod
    def _get_r_repr(self):
        pass

    @abc.abstractmethod
    def _get_q_repr(self):
        pass

    @abc.abstractmethod
    def __call__(self):
        """Get the actual integer value of the cyphertext"""
        pass

    def decrypt(self):
        return (self() % self.key) % 2

    def decrypt_repr(self):
        s = f'{Style.BRIGHT}D(C({COL(self._get_m_repr(), Fore.GREEN)})) = ' \
            f'{self()} ' \
            f'mod {COL(self.key, Fore.YELLOW)} ' \
            f'mod {COL(2, Fore.RED)}' \
            f' = {COL(self.decrypt(), Fore.GREEN)}{Style.RESET_ALL}'
        return s

    def check_valid(self):
        if self._get_r() >= self.key:
            print(f'{Fore.YELLOW}WARNING: noise term ({self._get_r()}) is larger than the key ({self.key}), decryption is no longer valid{Style.RESET_ALL}')

    def __repr__(self):
        if _verbose:
            return f'{Style.BRIGHT}C({COL(self._get_m_repr(), Fore.GREEN)}) = ' \
                   f'{COL(self._get_m_repr(), Fore.GREEN)} + ' \
                   f'2⋅{COL(self._get_r_repr(), Fore.RED)} + ' \
                   f'{COL(self._get_q_repr(), Fore.BLUE)}⋅{COL(self.key, Fore.YELLOW)} = ' \
                   f'{self()}{Style.RESET_ALL}'
        else:
            return f'{Style.BRIGHT}{self()}{Style.RESET_ALL}'

    def __add__(self, other):
        return PCCyphertextAddition(self, other)

    def __mul__(self, other):
        return PCCyphertextMultiplication(self, other)


class PCCyphertext(PCCyphertextBase):
    """
    Cyphertext of a single encrypted bit without any computations applied to it.
    """

    def __init__(self, m, key, r=None, q=None):
        assert m in [0, 1], f'`{m}` is not a bit'
        super().__init__(key)

        # random values, domains might not be optimal
        if r is None:
            r = random.randint(2, math.ceil(math.log(self.key)))  # noise
        if q is None:
            q = random.randint(int(self.key * 0.7), int(self.key * 1.2))

        self.r = r
        self.q = q
        self.m = m

        self.check_valid()

    def _get_m(self):
        return self.m

    def _get_r(self):
        return 2*self.r

    def _get_m_repr(self):
        return str(self.m)

    def _get_r_repr(self):
        return str(self.r)

    def _get_q_repr(self):
        return str(self.q)

    def __call__(self):
        return self.m + 2*self.r + self.q*self.key


class PCCyphertextAddition(PCCyphertextBase):
    """
    Addition of two cyphertexts
    """

    def __init__(self, c1, c2):
        assert c1.key == c2.key
        super().__init__(c1.key)

        self.c1 = c1
        self.c2 = c2

        self.check_valid()

    def _get_m(self):
        return self.c1._get_m() + self.c2._get_m()

    def _get_r(self):
        return 2*(self.c1._get_r() + self.c2._get_r())

    def _get_m_repr(self):
        return f'({self.c1._get_m_repr()} + {self.c2._get_m_repr()})'

    def _get_r_repr(self):
        return f'({self.c1._get_r_repr()} + {self.c2._get_r_repr()})'

    def _get_q_repr(self):
        return f'({self.c1._get_q_repr()} + {self.c2._get_q_repr()})'

    def __call__(self):
        return self.c1() + self.c2()


class PCCyphertextMultiplication(PCCyphertextBase):
    def __init__(self, c1, c2):
        assert c1.key == c2.key
        super().__init__(c1.key)
        self.c1 = c1
        self.c2 = c2

        self.check_valid()

    def _get_m_repr(self):
        return f'({self.c1._get_m_repr()}⋅{self.c2._get_m_repr()})'

    def _get_r_repr(self):
        r1 = self.c1._get_r_repr()
        r2 = self.c2._get_r_repr()
        m1 = self.c1._get_m_repr()
        m2 = self.c2._get_m_repr()
        return f'(2⋅{r1}⋅{r2} + {r1}⋅{m2} + {r2}⋅{m1})'

    def _get_q_repr(self):
        r1 = self.c1._get_r_repr()
        r2 = self.c2._get_r_repr()
        m1 = self.c1._get_m_repr()
        m2 = self.c2._get_m_repr()
        q1 = self.c1._get_q_repr()
        q2 = self.c2._get_q_repr()
        return f'({m1}⋅{q2} + {m2}⋅{q1} + 2⋅{r1}⋅{q2} + 2⋅{r2}⋅{q1} + {q1}⋅{q2}⋅{self.key})'

    def __call__(self):
        return self.c1() * self.c2()

    def _get_r(self):
        r1 = self.c1._get_r()
        r2 = self.c2._get_r()
        m1 = self.c1._get_m()
        m2 = self.c2._get_m()
        return 2*(2*r1*r2 + r1*m2 + r2*m1)

    def _get_m(self):
        return self.c1._get_m() * self.c2._get_m()
