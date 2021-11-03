# Reliable UDP (Part 1)

# Instructions
The purpose of this assignment is to extend your *netster* code so that it supports reliable network communication over UDP - Reliable UDP (RUDP)! You already implemented an application protocol over TCP and UDP and you will now have an opportunity to test your existing code over an unreliable network. Once the limitations of UDP over an unreliable channel become clear you should feel motivated to solve the issue at hand with your own RUDP implementation.

This assignment requires both careful planning and likely several iterations of your design before it meets the specifications put forth in the tasks below. You will have more time to complete this assignment, but be sure to start early and begin experimenting with a lossy network to understand how things work. Good luck!

# Emulating your local coffee shop's free public Wi-Fi

On a Linux machine where you have `sudo` privileges, you can emulate a lossy network using `tc` (traffic control).

`tc` allows a user to simulate network properties on Linux machines; properties such as latency and packet loss rates. These properties can be restricted to specific TCP/UDP ports. This is the approach you will use in this assignment to test your `rudp` implementation. For your convenience, we have supplied a wrapper script [`tc.sh`](tc.sh) that makes toggling packet loss modes on and off very easy:

Toggle the traffic control on:

```
$ ./tc.sh add
```

Verify your applied changes:

```
$ ./tc.sh show
```


You will now have special port ranges with bad performance:

| dst port range | latency | loss |
| -------------- | ------- | ---- |
|      2048-2304 | 25ms    |   0% |
|      4096-4352 | 25ms    |   1% |
|      8192-8448 | 25ms    |  30% |

To remove these settings, use the following:

```
$ ./tc.sh del
```

# How to develop and test with packet loss and delay

* Other ports (not within the ranges described in the table above) are normal, with no added latency or loss behavior.
* The test ports add 25ms latency in both directions, or 50ms round-trip-time (RTT).

You can observe the impact of 30% loss across the network with two terminals.

Server terminal:

```
$ ./netster -u -p 8199
<...missing lots of input from the client...>
```

Client terminal:

```
$ ./netster -u -p 8199 localhost
Hello, I am a client
send a message: <...type lots of input...>
<...missing lots of responses from the server...>
```

# Task 1 - Implement alternating bit, stop-and-wait protocol
You will implement a stop-and-wait protocol (i.e. alternating bit protocol), called *rdt3.0* in the book and slides.

Since you implemented the UDP client/server in the previous assignment, you already have experience with a similar unreliable channel interface (i.e. `udt_send()` and `udt_recv()` from *rdt3.0*). You must now implement the `rdt_send()` and `rdt_rcv()` interface that your client and server will use when running in RUDP mode.  In other words, your `rdt_` methods should provide the reliable service for the calling application and use the underlying UDP `sendto()` and `recvfrom()` methods to access the unreliable channel.

There are a number of ways to implement this task correctly; however, there are a few key features and assumptions you can count on:

- You must introduce a new RUDP header that encapsulates the application data sent in via `rdt_send()`.  This should include fields to support sequence numbers, message type (ACK, NACK, etc.), and potentially other fields like length.
- You will need a countdown timer.
- You may assume unidirectional data transfer (e.g. client -> server).  You can make changes in your *netster* application code to ensure unidirectional flow (e.g. file transfer) and also to pace how often data is sent into your RUDP interface for debugging purposes.
- You may use both ACK and NACK control messages instead of duplicate ACKs.
- You may assume only a single client RUDP session at a time.
- You may assume that UDP handles the checksum error detection for you.  Your implementation should treat any potentially corrupt packets the same as if they were simply lost.

Your strategy for state management and handling control messages is up to you.  The *rdt3.0* state machines from the book and slides may be guides but the expectation is that many different implementations should arise from this assignment.

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
$ make part3
```

In order to build your code, run `make` in the `src` directory. You may
also clean your directory automatically with `make clean`.

## Testbed

```
+------------------------------+
|           localhost          |
+------------------------------+
               | ssh              1) ssh into one of IUs Linux systems
               V
+------------------------------+
|   silo, sharks, hulk, tank   |
+------------------------------+
               | ssh              2) ssh into netdev
               V
+------------------------------+
| netdev.open.sice.indiana.edu |
+------------------------------+
    | ssh                 | ssh   3) ssh into client/server separately
    V                     V
+--------+            +--------+
| client |            | server |  4) Run iperf3 between client/server
+--------+            +--------+     baratheon:~$ iperf3 -s
          \          /               targaryen:~$ iperf3 -c server
           +--------+
           | router |
           +--------+
```


## Instructions

For this assignment, there is only one task: extend your `file_client` and `file_server` functions to use stop-and-wait to transfer the files.

  * This should function correctly for all 3 port ranges

## TO SUBMIT

- Submit by uploading your files to the autograder [here](https://autograder.sice.indiana.edu/web/project/340). Make sure
  that you are submitting to the correct assignment.
- Upload your `stopandwait.c` file.
- If your code completes and you are happy with your grade, you are
  done (you may submit as many times as you like).
