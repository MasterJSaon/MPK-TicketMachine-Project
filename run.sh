if [[ $1 == "-r" ]]; then
    # run without compiling (run executable)
    ./QtApp
else 
    make            # To build the program
    make clean      # To remove object files and intermediate files
    ./QtApp         # To run the built program
fi