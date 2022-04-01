CSC 360: Programming Assignment 3
A Simple File System
Sean McAuliffe, V00913346
April 4, 2022


*--------------*
| Introduction |
*--------------*

This repo provides four disk utility programs which operate on simple FAT formatted disk image files.

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


*---------------------*
| Part II - Disk List |
*---------------------*

Usage: $ ./disklist <disk_image_file>


*---------------------*
| Part III - Dist Get |
*---------------------*

Usage: $ ./diskget <disk_image_file> /path/to/diskImageFile <filename>


*--------------------*
| Part IV - Disk Put |
*--------------------*

Usage: $ ./diskput <disk_image_file> <filename> /path/to/diskImageFile