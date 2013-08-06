import sys
import optparse

REMAINDER = "remainder"

class PropSet:
    def __init__ (self):
        pass

class ArgumentParser:
    def __init__ (self, **kwargs):
        self.kwargs    = kwargs
        self.arguments = []
        self.options   = []
        self.defaults  = {}

    def add_argument (self, *args, **kwargs):
        arg = PropSet()
        arg.args   = args
        arg.kwargs = kwargs

        if args[0].startswith("-"):
            # Option
            self.options.append(arg)
        else:
            # Positional argument
            self.arguments.append(arg)

    def add_subparsers (self, **kwargs):
        self.subparsers = {}
        return self

    def add_parser (self, name, **kwargs):
        self.subparsers[name] = ArgumentParser (**kwargs)
        return self.subparsers[name]

    def set_defaults (self, **kwargs):
        for key in kwargs:
            self.defaults[key] = kwargs[key]

    def parse_args (self):
        description = self.kwargs.get("description")
        self.kwargs["description"] = description \
            + " Please use Python >2.7 to get a full help on this program's usage."

        parser = optparse.OptionParser (**self.kwargs)
        for option in self.options:
            parser.add_option (*option.args, **option.kwargs)

        try:
            subcommand = sys.argv[1]
        except IndexError:
            sys.stderr.write ("No subcommand provided!\n")
            sys.exit (1)

        try:
            subparser = self.subparsers[subcommand]
        except KeyError:
            # If help was requested, optparse will display it and exit
            parser.parse_args ()

            # Error
            sys.stderr.write ("Unknown subcommand: `%s`\n" % subcommand)
            sys.exit (1)

        # Let optparse handle options
        for option in subparser.options:
            parser.add_option (*option.args, **option.kwargs)
        (options, cl_args) = parser.parse_args(sys.argv[2:])

        # Handle Arguments
        i = 0
        for argument in subparser.arguments:
            dest  = argument.args[0]
            nargs = argument.kwargs.get("nargs", 1)

            if nargs == REMAINDER:
                val = cl_args[i:]
                i = len (cl_args)
            elif nargs == "?":
                if i<len(cl_args):
                    val = cl_args[i]
                    i += 1
                else:
                    val = argument.kwargs["default"]
            elif nargs == 1:
                val = cl_args[i]
                i += 1
            else:
                val = cl_args[i:i+nargs]
                i += nargs
            setattr (options, dest, val)

        # Set default values
        for key in subparser.defaults:
            if not hasattr (options, key) or getattr(options, key) is None:
                setattr (options, key, subparser.defaults[key])

        return options

if __name__ == "__main__":
    def start (arg):
        print arg
    update = index = load = scan = trace = clean = kill = stop = start
    complete = grep = findDefinition = start

    parser = ArgumentParser (
        description = "Index a source code base using libclang.")

    parser.add_argument ("--debug", "-d",
                         action = 'store_true', default = False,
                         help = "print debugging information")

    subparsers = parser.add_subparsers (metavar = "SUBCOMMAND")


    # Manage the server
    subparsers.add_parser (
        "start",
        help = "start the clang-tags server",
        description = "Start the clang-tags server."
        ).set_defaults (fun = start)

    subparsers.add_parser (
        "stop",
        help = "shutdown the clang-tags server",
        description = "Shutdown the clang-tags server."
        ).set_defaults (fun = stop)

    subparsers.add_parser (
        "kill",
        help = "kill the clang-tags server",
        description = "Kill the clang-tags server."
        ).set_defaults (fun = kill)

    subparsers.add_parser (
        "clean",
        help = "clean-up after a non-nice server shutdown",
        description = "Clean up after a non-nice server shutdown:"
        " pid file and server socket are wiped."
        ).set_defaults (fun = clean)


    # Create the compilation database
    parserTrace = subparsers.add_parser (
        "trace", help = "trace a build command",
        description =
        "Create a compilation database by tracing a build command.")
    parserTrace.add_argument (
        "command",
        metavar = "COMMAND",
        nargs = REMAINDER,
        help = "build command line")
    parserTrace.set_defaults (sourceType = "trace")
    parserTrace.set_defaults (fun = trace)


    parserScan = subparsers.add_parser (
        "scan", help = "scan a source directory",
        description =
        "Create a compilation database by scanning a source directory")
    parserScan.add_argument ("--compiler", "-c",
                             default = "gcc",
                             help = "compiler name (default: gcc)")
    parserScan.add_argument ("srcdir",
                             help = "top sources directory")
    parserScan.add_argument ("clangArgs",
                             metavar = "CLANG_ARGS",
                             nargs = REMAINDER,
                             help = "additional clang command-line arguments")
    parserScan.set_defaults (sourceType = "scan")
    parserScan.set_defaults (fun = scan)


    # Manage the source files index
    parserLoad = subparsers.add_parser (
        "load", help = "load a compilation database",
        description = "Read the compilation commands needed to build the"
        " project from a compilation database.")
    parserLoad.add_argument ("compilationDB",
                             metavar = "FILEPATH",
                             help = "path to the compilation database",
                             nargs = "?",
                             default = "compile_commands.json")
    parserLoad.set_defaults (fun = load)


    parserIndex = subparsers.add_parser (
        "index", help = "index source code base",
        description = "Create an index of all tags in the source code base."
        " Source files and compilation commands are taken from a clang"
        " \"compilation database\" in JSON format, previously read using"
        " the \"load\" subcommand.")
    parserIndex.add_argument (
        "--exclude", "-e",
        metavar = "DIR",
        action = "append",
        help = "do not index files under DIR")
    parserIndex.add_argument (
        "--exclude-clear", "-E",
        dest="exclude",
        action="store_const", const=[],
        help = "reset exclude list")
    parserIndex.set_defaults (exclude = ["/usr"])
    parserIndex.set_defaults (fun = index)


    parserUpdate = subparsers.add_parser (
        "update",
        help = "update index",
        description =
        "Update the source code base index, using the same arguments as "
        "previous call to `index'")
    parserUpdate.set_defaults (fun = update)


    # IDE-like features
    parserFindDef = subparsers.add_parser (
        "find-def",
        help = "find the definition of an identifier in a source file",
        description = "Find the definition location of an identifier in a "
        "source file.")
    parserFindDef.add_argument ("fileName", metavar = "FILE_NAME",
                                help = "source file name")
    parserFindDef.add_argument ("offset", metavar = "OFFSET",
                                help = "offset in bytes")
    parserFindDef.add_argument ("--index", "-i",
                                dest = "fromIndex",
                                action = "store_true",
                                help = "look for the definition in the index")
    parserFindDef.add_argument ("--recompile", "-r",
                                dest = "fromIndex",
                                action = "store_false",
                                help =
                                "recompile the file to find the definition")
    parserFindDef.set_defaults (fromIndex = True)
    parserFindDef.set_defaults (fun = findDefinition)


    parserGrep = subparsers.add_parser (
        "grep",
        help = "find all uses of a definition",
        description =
        "Find all uses of a definition, identified by its USR (Unified Symbol "
        "Resolution). Outputs results in a grep-like format.")
    parserGrep.add_argument ("usr",
                             metavar = "USR",
                             help = "USR for the definition")
    parserGrep.set_defaults (fun = grep)


    parserComplete = subparsers.add_parser (
        "complete",
        help = "find completions at point",
        description =
        "Find possible code completions at a given position in the source code.")
    parserComplete.add_argument ("fileName", metavar = "FILE_NAME",
                                 help = "source file name")
    parserComplete.add_argument ("line", metavar = "LINE",
                                 help = "Line number")
    parserComplete.add_argument ("column", metavar = "COLUMN",
                                 help = "Column number")
    parserComplete.set_defaults (fun = complete)


    args = parser.parse_args ()
    args.fun (args)
