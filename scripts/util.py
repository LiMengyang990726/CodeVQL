import os


def print_cli_banner():
    if not os.environ.get("NO_DISPLAY_CLI_BANNER"):
        # Print name of the APP
        from pyfiglet import Figlet
        title = Figlet(font='slant')
        subtitle = Figlet(font='bubble')
        print(title.renderText('EvoMe'))
        print(subtitle.renderText('Version-Aware Security Analysis'))
