# Search folder
This project was done in the context of the System's Programming semester project at [HEPIA](http://hepia.hesge.ch/).

**Authors:** David Gonzalez & Claudio Sousa

## Summary
The project consists in developing a tool that creates a dynamic folder containing the result of a file search.

The tool is used by specifying a *destination* folder, a *source* folder and an optional filtering expression.
The *destination* folder is created and inside is maintained an up-to-date list of symbolic links pointing to the files found within the *source* folder matchin the filtering expression.

The filtering expression syntax is closely inspired by the *find* command. The user may filter files according to the following criteria: *name*, *group*, *owner*, *group owner*, *permission*, *size*, *access/modified/metadata time*.

The criteria above may be combined using logical operators (*and, or, not*) or parenthesis.

## Examples

This example links all backup files that are bigger that 10 mega whose creation date is older than 30 days:

`./searchfolder destdir backup -name .bkp -size +10M -ctime -30d`

This example links all files on the system whose permission are for everyone and
the group owner is *root* or the owner is not *root*:

`./searchfolder destdir / (-group root -or -not -user root) -and -perm 777`

## Documentation
For further information please have a look at:
 - [Code documentation](https://hepia-projects.gitlab.io/smart-folder/)
 - [Architecture document](https://gitlab.com/hepia-projects/smart-folder/raw/master/report/SmartFolder-architecture-document.pdf)

