# Search folder
This project was done in the context of the System's Programming semester project at [HEPIA](http://hepia.hesge.ch/).

**Authors:** David Gonzalez & Claudio Sousa

## Summary
The project consists in developing a tool that creates a dynamic folder containing the result of a file search.

The tool is used by specifying a *destination* folder, a *source* folder and an optional filtering expression.
The *destination* folder is created and inside is maintained an up-to-date list of symbolic links pointing to the files found within the *source* folder matchin the filtering expression.

The filtering expression syntax is closely inspired by the *find* command. The user may filter files according to the following criteria: *name*, *group*, *owner*, *group owner*, *permission*, *size*, *access/modified/metadata time*.

The criteria above may be combined using logical operators (*and, or, not*) or parenthesis.

## Documentation
For further information please have a look at:
 - [Code documentation](https://hepia-projects.gitlab.io/smart-folder/)
 - [Architecture document](https://gitlab.com/hepia-projects/smart-folder/raw/241c3db48603ed35ae2515ab321b59c2d513c413/report/SmartFolder-architecture-document.pdf)
