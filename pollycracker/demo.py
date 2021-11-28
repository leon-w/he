from colorama import Fore, Style

from pollycracker import (Key, PCCyphertext, PCCyphertextBase,
                          PCCyphertextList, set_verbose)

key = Key(10)

verbose_activated = False
set_verbose(verbose_activated)


def verbose(val=None):
    """toggle verbose output"""
    global verbose_activated
    if val is None:
        verbose_activated = not verbose_activated
    else:
        verbose_activated = bool(val)

    set_verbose(verbose_activated)

    if verbose_activated:
        print('Verbose mode is now on.')
    else:
        print('Verbose mode is now disabled.')


def encrypt(*bits):
    if len(bits) > 1:
        return PCCyphertextList(map(lambda b: PCCyphertext(b, key()), bits))
    return PCCyphertext(bits[0], key())


def decrypt(c):
    if verbose_activated and isinstance(c, PCCyphertextBase):
        print(c.decrypt_repr())
    return c.decrypt()


def help():
    help_text = f"""{Fore.LIGHTYELLOW_EX}Help:
'verbose()'     : toggle verbose mode
'key'           : view the current key
'key.new(nbits)': generate a new key to replace the old one
'encrypt(bits)' : encrypt one or more bits using the current key
'decrypt(c)'    : decrypt an encrypted bit or list of bits
'c1+c2 / c1*c2' : add/multiply bits{Style.RESET_ALL}"""
    print(help_text)


def start_console():
    global key
    prompt = f"""{Fore.LIGHTYELLOW_EX}   ___         __ __       _____                 __            
  / _ \ ___   / // /__ __ / ___/____ ___ _ ____ / /__ ___  ____
 / ___// _ \ / // // // // /__ / __// _ `// __//  '_// -_)/ __/
/_/    \___//_//_/ \_, / \___//_/   \_,_/ \__//_/\_\ \__//_/   
                  /___/                                        {Style.RESET_ALL}
Type 'help()' for help."""

    import code
    import readline  # optional, will allow Up/Down/History in the console

    shell = code.InteractiveConsole({'help': help,
                                     'key': key,
                                     'encrypt': encrypt,
                                     'decrypt': decrypt,
                                     'verbose': verbose
                                     }, filename='<PollyCracker Interactive Console>')
    shell.interact(banner=prompt, exitmsg='exiting PollyCracker Interactive Console...')


if __name__ == '__main__':
    start_console()
