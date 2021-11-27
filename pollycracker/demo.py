from colorama import Fore, Style

import pollycracker

key = pollycracker.Key(10)


def encrypt(bit):
    return pollycracker.PCCyphertext(bit, key())


def decrypt(c):
    print(c.decrypt_repr())


def help():
    help_text = f"""{Fore.LIGHTYELLOW_EX}Help:
'key'           : view the current key
'key.new(nbits)': generate a new key to replace the old one
'encrypt(bit)'  : encrypt a bit using the current key
'decrypt(c)'    : decrypt an encrypted bit
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
                                     })
    shell.interact(banner=prompt, exitmsg='exiting PollyCracker interactive demo...')


if __name__ == '__main__':
    start_console()
