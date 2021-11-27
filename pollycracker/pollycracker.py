import abc
import math
import random

from colorama import Fore, Style
from Crypto.Util import number


def COL(s, color):
    return f'{color}{s}{Fore.WHITE}'


class Key:
    def __init__(self, bits):
        self.key = number.getPrime(bits)

    def new(self, bits):
        self.key = number.getPrime(bits)
        return self

    def __call__(self):
        return self.key

    def __repr__(self) -> str:
        return f'{Style.BRIGHT}{COL("p", Fore.YELLOW)} = {self.key}{Style.RESET_ALL}'


class PCCyphertextBase(abc.ABC):
    def __init__(self, key):
        super().__init__()
        self.key = key

    @abc.abstractmethod
    def get_r(self):
        pass

    @abc.abstractmethod
    def get_m(self):
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
        if self.get_r() >= self.key:
            print(f'{Fore.YELLOW}WARNING: noise term ({self.get_r()}) is larger than the key ({self.key}){Style.RESET_ALL}')

    def __add__(self, other):
        return PCCyphertextAddition(self, other)

    def __mul__(self, other):
        return PCCyphertextMultiplication(self, other)

    def __repr__(self):
        s = f'{Style.BRIGHT}C({COL(self._get_m_repr(), Fore.GREEN)}) = ' \
            f'{COL(self._get_m_repr(), Fore.GREEN)} + ' \
            f'2⋅{COL(self._get_r_repr(), Fore.RED)} + ' \
            f'{COL(self._get_q_repr(), Fore.BLUE)}⋅{COL(self.key, Fore.YELLOW)} = ' \
            f'{self()}{Style.RESET_ALL}'
        return s


class PCCyphertext(PCCyphertextBase):

    def __init__(self, bit, key, r=None, q=None):
        assert bit in [0, 1]
        super().__init__(key)
        if r is None:
            r = random.randint(2, math.ceil(math.log(self.key)))  # noise
        if q is None:
            q = random.randint(int(self.key * 0.7), int(self.key * 1.2))

        self.r = r
        self.q = q
        self.bit = bit

        self.check_valid()

    def _get_m_repr(self):
        return str(self.bit)

    def _get_r_repr(self):
        return str(self.r)

    def _get_q_repr(self):
        return str(self.q)

    def __call__(self):
        return self.bit + 2*self.r + self.q*self.key

    def get_r(self):
        return 2*self.r

    def get_m(self):
        return self.bit


class PCCyphertextAddition(PCCyphertextBase):
    def __init__(self, c1, c2):
        assert c1.key == c2.key
        super().__init__(c1.key)
        self.c1 = c1
        self.c2 = c2

        self.check_valid()

    def _get_m_repr(self):
        return f'({self.c1._get_m_repr()} + {self.c2._get_m_repr()})'

    def _get_r_repr(self):
        return f'({self.c1._get_r_repr()} + {self.c2._get_r_repr()})'

    def _get_q_repr(self):
        return f'({self.c1._get_q_repr()} + {self.c2._get_q_repr()})'

    def __call__(self):
        return self.c1() + self.c2()

    def get_r(self):
        return 2*(self.c1.get_r() + self.c2.get_r())

    def get_m(self):
        return self.c1.get_m() + self.c2.get_m()


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

    def get_r(self):
        r1 = self.c1.get_r()
        r2 = self.c2.get_r()
        m1 = self.c1.get_m()
        m2 = self.c2.get_m()
        return 2*(2*(r1+r2) + r1*m2 + r2*m1)

    def get_m(self):
        return self.c1.get_m() * self.c2.get_m()
