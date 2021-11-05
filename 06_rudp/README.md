# Reliable UDP (Part 2)

# Instructions
The purpose of this assignment is to complement your existing Reliable UDP (RUDP) stop-and-wait protocol with a go-back-N protocol.

You will once again use the `tc.sh` script to emulate a lossy network or ssh over to our new testbed.

##  Testbed

```
+------------------------------+
|           localhost          |
+------------------------------+
               | ssh              1) ssh into netdev
               V
+------------------------------+
| netdev.open.sice.indiana.edu |
+------------------------------+
    | ssh                 | ssh   2) ssh into client/server separately
    V                     V
+--------+            +--------+
| client |            | server |  3) Run iperf3 between client/server
+--------+            +--------+     baratheon:~$ iperf3 -s
          \          /               targaryen:~$ iperf3 -c server
           +--------+
           | router |
           +--------+
```

# Task 1 - Implement go-back-N protocol
As we have learned, stop-and-wait will be horribly slow when transferring any significant amount of data, especially as latency in the network increases.  With a working implementation of the previous assignment, you should be able to observe this slow behavior on the lossy network emulator.

For go-back-N, you will need at a minimum:

 - An allocated buffer that stores your client's window of data.  This may be a fixed-size buffer with reasonable limits (e.g. 128 "slots").
 - The ability to buffer and send data while keeping track of which "slots" have been acknowledged by the receiver.
 - A timer that indicates when it is time to *go-back-N* and retransmit.

## Setup

The *netster* framework includes a build system to reduce the number
of new files added for each assignment. The following instructions
assume that you are starting in the root directory of your personal
class git repository and have set up our repository as [upstream](https://github.iu.edu/SICE-Networks/Net-Fall21/wiki/Submission#remote-setup).

Begin by fetching the most recent content from our repository. You can
now build the third assignment in the `src` directory. Complete your work
within he repository's `src` directory.

```
$ git fetch upstream
$ git merge upstream/master master
$ cd src
$ make part4
```

In order to build your code, run `make` in the `src` directory. You may
also clean your directory automatically with `make clean`.

## Instructions

For this assignment, there is only one task: extend your `gbn_client` and `gbn_server` functions to use go-back-N to transfer the files.

  * This should function correctly for all 3 port ranges, similar to the previous assignment

# Task 2 (Graduate Sections) - Add simple congestion control

You must implement a congestion control "window" that begins small and grows as data is successfully acknowledged. Your strategy for acknowledgements (including negative acknowledgement) is up to you.

The goal is to prevent the go-back-N protocol from blasting the network with a full window of data at once, but start slow and then "probe" the network to determine how large the window may grow to avoid loss.
