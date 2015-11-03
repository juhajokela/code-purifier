Code Purifier

Version: 0.1
Author: Juha Jokela

Overview:

A code purifier written in C. Purifies source code files to satisfy Git formatting. Removes trailing whitespace and ensures that file is terminated with exactly one newline.

Technical specification:

 - Writes log to file.
 - Supports termination with CTRL-C.
 - Forks a new process for every directory.
 - Supports recursively traverse through nested directories.
 - Acquires file lock for every file processed, including log file.
 - Uses library for easy file locking.

Usage:

./codepurifier [files] [directories] [options]

files:

	list of filenames to be purified

directories:

	list of directories to be purified

options:

	--verbose    (-v)                  turn verbose mode on
	--recursive  (-r)                  recursively traverse directories
	--extensions (-e) .c,.py,...       define file extensions to be processed
	--logging    (-l)                  turn logging on with default logfile (code-purifier.log)
	--logging=[logfile] (-l=[logfile]) turn logging on with specific logfile

Issues:

	1. Error handling for situations where too many files (filedescriptors) are open is missing.
