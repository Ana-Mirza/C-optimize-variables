## C-optimize variables

This is a script for optimizing the constants in [C-step-counter](https://github.com/Ana-Mirza/C-Step-Counter).

## Guide

 - Clone the repo
 - Edit *dataset[]* in main to your CSV files collected from accelerometer.
 - Set *correct_answer* to the amount of steps in the CSV file
 - Run cmake
 - Execute the binary

After you've done this the program finds better and better variables and prints it.

If you see that the search range is not enough you can edit the for-loops in main to have a bigger range.
