
Note: You must have more than 600 GB of free space to safely accomodate the Gaia DR2 Data (in binary form. The CSV file formats will require even more space)

First, you will need to run gaia2writebin.c (use gcc to compile all these files individually)
This will write out all of the Gaia DR2 data from a csv.gz file format to a binary format
Then run gaia2datasort.c
This will sort the data by ra and dec
Run gaia2idBin.c and then gaia2idsort.c to create a file that allows for quick ID queries.

Note that you may need to change the directories hard-coded into each of the C files to accomodate your computer
