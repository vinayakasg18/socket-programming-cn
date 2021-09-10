# Exploring the Domain Name System (DNS)

# Instructions

The Domain Name System (DNS) is used to translate human readable domain names to numerical IP addresses. This assignment will have you become familiar with the concept of DNS and give you some hands-on experience manually resolving DNS queries and interpreting their responses.

You will need to use either of the following DNS tools:

* `host`, `nslookup`, `dig` (bind package)
* `drill` (ldns package)

All of them have the same basic usage, shown here in the example of resolving the domain name `google.com`:

```
$ host google.com
$ nslookup google.com
$ dig google.com
$ drill google.com
```

However, their options and output formats differ quite a bit. Study the man pages for those tools to learn more about their usage:

```
$ man 1 host
$ man 1 nslookup
$ man 1 dig
$ man 1 drill
```

Alternatively, you can also consult the manpages on `linux.die.net/man`

* [host](https://linux.die.net/man/1/host)
* [nslookup](https://linux.die.net/man/1/nslookup)
* [dig](https://linux.die.net/man/1/dig)
* [drill](https://linux.die.net/man/1/drill)

## Questions

Answer the following questions and put them into a file called "answers". You may attach terminal output from dns tools to strengthen your point.

1. How can DNS be used to load balance services? Give a concrete explanation for google.com

2. What is the inherent weakness of traditional DNS? Give an example of how an attacker might utilize it.

3. Perform a manual iterative DNS query for mail-relay.iu.edu with dig starting from the root servers. List all commands and their outputs and explain why you issued every command. Do not use tracing features (`dig +trace`) for your final write-down.

4. You are sitting in a coffee shop and are connected to a public WLAN. You fire up wireshark and start sniffing the traffic of other customers. You notice that all of their traffic is over https so you cannot simply read it. You also notice something else, what is it and what are the implications?

5. Suppose that IU has an internal DNS cache. You are an ordinary user (no network admin). Can you determine (and if yes, how) if a given external website was recently accessed?

## To Submit

* Text file with your answers to questions including terminal outputs
