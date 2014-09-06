persistence
===========

Persist is a tool that checks the Persistent property of concurrent programs under Total-Store-Order (TSO) memory models such as in Intel x86 and prevents non-intended behaviours by inserting a minimal set of fences.

Persistence is a weaker definition than data race, and defined by program order and total store order. Keeping a program Persist under TSO guarantees sequential consistent properties of the program. Another way to avoid non-intended behaviours of programs is keeping it Robust [1]. Robustness is based on program order, store order, and read-from relations that is considered in many previous approaches [2,3,4]. The stronger one of Robustness and Persistence is Triangular race free (TRF) [5].

One of important differences between Persistence and Robustnes that we can verify persistent properties of programs with abstraction making our tool is more scalable, especially with algorithms of multiple processes that are common in concurrent systems.

Source code for Persist: You can find instructions how to install, run, and reproduce all results in Table 1 and Figure 17 of the paper using material provided in this package. Here is list of files and folders:

+ Readmefirst.txt: this current file.
+ install.txt: how to download and install Persist.
+ install_other_tools.txt: how to download and install other tools (Memorax, Musketeer, Trencher, and Remmex).
+ manual.txt: how to run and custermize input files with Persist.
+ commands.txt: common commands to use with Trencher, Persist, Memorax, Musketeer, and Remmex.
+ examples:  algorithm directory using in Table 1 and Figure 17 of paper.
+ run_test.sh: file for automatic running all tests, each test includes Persist, Trencher, Musketeer, Memorax, and Remmex if available. To do this work automatically, please put executable files of Memorax (memorax), Trencher (trencher), Persist (PERSIST), Musketeer (goto-cc, musketeer, fence-insert.py) into build folder. 
+ fence-inserter.py: script file used by Musketeer version 17 April 2014, in the case cannot find it from Musketeer's website.
+ test_cases.txt: file used by run_test.sh. 
+ src: source file directory.

NOTE: using run_test.sh, except Persist, some examples will take longer than 40 mins (in a machine of 2.4 Ghz 2 cores, 4GB RAM), or run out of memory; you may need to stop it yourself!!!

[1] D. Shasha and M. Snir. Efficient and correct execution of parallel programs that share memory. ACM Trans. Program. Lang. Syst., 10(2):282–312, Apr. 1988.

[2] A. Bouajjani, E. Derevenetc, and R. Meyer. Checking and enforcing robustness against tso. In ESOP, pages 533–553, 2013.

[3]  S. Burckhardt and M. Musuvathi. Effective program verification for relaxed memory models. In CAV, volume 5123 of LNCS, pages 107–120. Springer, 2008.

[4] J. Alglave, D. Kroening, V. Nimal, and D. Poetzl. Don’t sit on the fence: A static analysis approach to automatic fence insertion. In CAV, volume 8559 of LNCS. Springer, 2014.

[5] S. Owens. Reasoning about the implementation of concurrency abstractions on x86-tso. In ECOOP, volume 6183 of LNCS, pages 478-503, Springer 2010.

