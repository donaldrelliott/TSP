We have included the two other implementations in our submission, however
the Christofides' Algorithm (which isn't in subfolders) is the one to be
graded. All those source files reside at the root of the folder.

To compile the Christofides' Algorithm implementation navigate to the
directory where you uploaded the files and then enter the command:

make [PRESS ENTER]

This will build the program for you. To find paths from the .txt files given
run the program as such './tsp inputFile', without the quotes.
Each file we were to work with is shown below:

./tsp tsp_example_1.txt
./tsp tsp_example_2.txt
./tsp tsp_example_3.txt
./tsp test-input-1.txt
./tsp test-input-2.txt
./tsp test-input-3.txt
./tsp test-input-4.txt
./tsp test-input-5.txt
./tsp test-input-6.txt
./tsp test-input-7.txt

All solution files can be found by appending a .tour to the end of each
text filename.

To verify that our submissions are indeed correct you may run the following
command:

./tsp-verifier.py test-input-1.txt test-input-1.txt.tour

To test the other files just replace the input and output filenames
accordingly.
