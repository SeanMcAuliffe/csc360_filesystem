CSC 360: Programming Assignment 3
A Simple File System
Sean McAuliffe, V00913346
April 4, 2022


*--------------*
| Introduction |
*--------------*

This repo provides four disk utility programs which operate on simple FAT
formatted disk image files.


*--------------------*
| Build instructions |
*--------------------*

Option 1) To build all disk utilities, run: $ make 
Option 2) Any of the following executables can be built individually,
          run: $ make diskinfo disklist diskget diskput


*---------------------------*
| Part I - Disk Information |
*---------------------------*

Usage: $ ./diskinfo <disk_image_file>

Example Output: 
user@host: file_system_utils > ./diskinfo test.dmg
Super block information:
Block size: 512
Block count: 6400
FAT starts: 2
FAT blocks: 50
Root directory start: 53
Root directory blocks: 8

FAT information:
Free Blocks: 6192
Reserved Blocks: 50
Allocated Blocks: 158


*---------------------*
| Part II - Disk List |
*---------------------*

Usage: $ ./disklist <disk_image_file>

Example Output:
user@host: file_system_utils > ./disklist test.dmg
F        735                      mkfile.cc 2005/11/15 12:00:00
F       2560                        foo.txt 2005/11/15 12:00:00
F       3940                    disk.img.gz 2009/08/04 21:11:13


*---------------------*
| Part III - Dist Get |
*---------------------*

Usage: $ ./diskget <disk_image_file> /path/to/diskImageFile <filename>

This program should produce no output unless an error has occurred.
It should create a new file with <filename> containing the data
retrieved from the specified path.

If the specified file does not exist at the specified path, an error
message will be shown.


*--------------------*
| Part IV - Disk Put |
*--------------------*

Usage: $ ./diskput <disk_image_file> <filename> /path/to/diskImageFile

This program should produce no output unless an error has occurred.