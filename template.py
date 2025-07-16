# Prototype CLI + help messages by leveraging click

import click


@click.command()
@click.option("-b", "print_bytes", is_flag=True, help="Print sizes in bytes")
@click.option("-d", "print_dirs", is_flag=True, help="Print directories only")
@click.option("-j", "indent_level", help="Print output as JSON with indentation LEVEL")
@click.option(
    "-I", "exclude", multiple=True, help="Exclude one or more files or directories"
)
@click.option("-L", "level", help="Descent LEVEL directories deep")
@click.argument("directory", required=False)
def main(directory):
    pass


if __name__ == "__main__":
    main()
