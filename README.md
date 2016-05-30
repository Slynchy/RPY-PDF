# RPY-eBook
For parsing .rpy script into eBook-friendly format (or txt, for further formatting)

# How to compile
Link with latest versions of libpng, libhpdf, and zlib. Compile as normal.

# How to use
Setup the corresponding text file to match parameters of the RPY script you wish to parse:

| File                                                 | What it does                                                                    |
|------------------------------------------------------|---------------------------------------------------------------------------------|
| charnames.txt                                        | List of short-hand character names that are used by script                      |
| charnames_fixed.txt                                  | List of formal/long-hand character names to replace ones in charnames.txt with  |
| debugstrings.txt                                     | You can list strings to find and hard-code C++ functions to run if the string is found in a line of script |
| prefixestoremove.txt                                 | List of prefixes to narration that are not character names that need to be removed |
| scriptfiles.txt                                      | List of script files to load in the correct order                               |
| stringstoremove.txt                                  | List of strings to remove if found in a line of script                          |
| stringstoreplace.txt                                 | List of strings to find and replace with respective string from replacementstrings.txt |
| replacementstrings.txt                               | See above.                                                                      |

Then, provide a font for the PDF to use, as well as an icon to put at the top of every page. Call these "font.ttf" and "logo.png". 

Once it is all set-up, simply run the EXE and it will spit out a TXT and a PDF file. The PDF file is better for reading on a computer/laptop, while the TXT file works excellently on Kindle.

# "Will this work with X?"
This is designed to work with games written on the Ren'Py engine, not any specific game. If you set-up the text files properly, it should work with any software built on the engine.
However, it doesn't work out-the-box; the scripts need to be decompiled from .rpyc to .rpy, and any extraneous content (other arcs, decisions, etc.) need to be trimmed from the script.

# Legality
The Ren'Py engine is covered largely by the MIT license, which permits modification free of restriction. The games that are produced on it, however, may not be. No warranty is provided for this software, nor will any support be provided for converting any Ren'Py game that you do not own the rights to, or been given express permission to modify. 
If your game is being converted to eBook format using this tool, that is a matter between yourself and the person converting the game.