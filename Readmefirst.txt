
You can find instructions how to install, run, and reproduce all results in Table 1 and Figure 17 of then paper using 
material provided in this package. Here is list of files and folders:

+ Readmefirst.txt: this current file.
+ install.txt: how to download and install Persist.
+ install_other_tools.txt: how to download and install other tools (Memorax, Musketeer, Trencher, and Remmex).
+ manual.txt: how to run and custermize input files with Persist.
+ commands.txt: common commands to use with Trencher, Persist, Memorax, Musketeer, Remmex.
+ examples:  algorithm directory using in Table 1 and Figure 17 of paper.
+ run_test.sh: file for automatic running all tests, each test includes Persist, Trencher, Musketeer, Memorax, and 
Remmex if available. To do this work automatically, please put executable files of Memorax (memorax), Trencher (trencher),
Persst (PERSIST), Musketeer (goto-cc, musketeer, fence-insert.py) into build folder.
+ fence-inserter.py: script file used by Musketeer version 17 April 2014, in the case cannot find it from Musketeer's 
website.
+ test_cases.txt: file used by run_test.sh. 
+ src: source file directory.

NOTE: using run_test.sh, except Persist, some examples will take longer than 40 mins (in a machine of 2.4 Ghz 2 cores, 
4GB RAM), or run out of memory; you may need to stop it yourself!!!

##########################
#This is the end of file.#
##########################
