tack(1)

# NAME

*tack* - A recreation of Gary Bernhardt's Selecta in C

# SYNOPSIS

cat (ls \*.txt | tack)

# DESCRIPTION

*tack* reads input from stdin, then presents a blocking UI through /dev/tty
which allows the user to search for and select a specific line from the
input. If a line is chosen it is written to stdout. If *tack* is cancelled, the
entire process group is killed.

## OPTIONS

*-H, --height* <lines>
	Specify the height of the UI (including the prompt line). Use *"full"*
	to tell *tack* to use the full height of the terminal. Note that the
	height is "clamped" to the total number of input lines and the height of
	the terminal, whichever is lower. The default height is 21 lines.

*-s, --search* <search>
	Specify the initial search query.

*-h, --help*
	Show the help message and exit.

*-v, --version*
	Show the version and exit.

## UI KEYS

*<Enter>*
	Choose the selected line, write it to stdout and exit *tack*.

*^C*
	Cancel selection and exit *tack*, killing the process group.

*^N*
	Move selection downward (wraps).

*^P*
	Move selection upward (wraps).

*^U*
	"Delete line" in query (resets query to "").

*^W*
	Delete word in query.

# DIAGNOSTICS

*tack* exits 0 if a line is chosen, 1 if it is cancelled.

# BUGS

*tack* does not support multi-byte characters or Unicode.

# AUTHORS

*tack* was written by Zandr Martin, but is a recreation of Selecta by Gary
Bernhardt.
