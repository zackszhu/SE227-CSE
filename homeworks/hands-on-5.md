# Answer

## Question 1

> Indicate what percentage of packets sent resulted in a successful response. For the packets from which you received a response, write down the minimum, average, and maximum round trip times in milliseconds. Note that ping reports these times to you if you tell it how many packets to send on the command line.


	--- www.csail.mit.edu ping statistics ---
	10 packets transmitted, 10 received, 0% packet loss, time 9010ms
	rtt min/avg/max/mdev = 263.777/264.120/264.671/0.248 ms

	--- www.w3.berkeley.edu ping statistics ---
	10 packets transmitted, 10 received, 0% packet loss, time 9002ms
	rtt min/avg/max/mdev = 184.160/184.333/184.641/0.456 ms

	--- www.unsw.edu.au ping statistics ---
	10 packets transmitted, 10 received, 0% packet loss, time 10899ms
	rtt min/avg/max/mdev = 415.990/416.265/417.771/0.959 ms

	--- www.kyoto-u.ac.jp ping statistics ---
	10 packets transmitted, 10 received, 0% packet loss, time 9014ms
	rtt min/avg/max/mdev = 129.569/129.656/129.754/0.234 ms


## Question 2

> Explain the differences in minimum round trip time to each of these hosts.

The time of each host used is different. Some are short and some are long.

## Question 3

> Now send pings with 56, 512 and 1024 byte packets to the 4 hosts above. Write down the minimum, average, and maximum round trip times in milliseconds for each of the 12 pings. Why are the minimum round-trip times to the same hosts different when using 56, 512, and 1024 byte packets?


	--- www.csail.mit.edu ping statistics ---
	56 Bytes: rtt min/avg/max/mdev = 263.847/264.151/264.653/0.704 ms
	512 Bytes: rtt min/avg/max/mdev = 264.170/264.402/264.868/0.694 ms
	1024 Bytes: rtt min/avg/max/mdev = 264.226/264.496/264.726/0.391 ms

	--- www.w3.berkeley.edu ping statistics ---
	56 Bytes: rtt min/avg/max/mdev = 184.372/187.200/191.474/2.236 ms
	512 Bytes: rtt min/avg/max/mdev = 184.408/184.646/185.726/0.426 ms
	1024 Bytes: rtt min/avg/max/mdev = 184.588/184.894/185.878/0.542 ms

	--- www.unsw.edu.au ping statistics ---
	56 Bytes: rtt min/avg/max/mdev = 415.941/416.109/416.300/0.597 ms
	512 Bytes: rtt min/avg/max/mdev = 416.244/416.339/416.543/0.749 ms
	1024 Bytes: rtt min/avg/max/mdev = 416.445/416.656/417.094/0.680 ms

	--- www.kyoto-u.ac.jp ping statistics ---
	56 Bytes: rtt min/avg/max/mdev = 129.433/129.709/130.271/0.335 ms
	512 Bytes: rtt min/avg/max/mdev = 129.727/129.960/130.273/0.285 ms
	1024 Bytes: rtt min/avg/max/mdev = 130.038/130.191/130.380/0.271 ms


It seems that with the packet size increasing, the time which `ping` uses grows as well. So I think it's because the packet size that makes the time differs each time.

## Question 4

> For some of the hosts, you may not have received any responses for the packets you sent. What are some reasons as to why you might have not gotten a response? (Be sure to check the hosts in a web browser.)


	--- ccms.wits.ac.za ping statistics ---
	100 packets transmitted, 0 received, 100% packet loss, time 99321ms


This host can't be `ping`ed but the web browser can easily access it. I think it is because this host bans all `ping`s.

## Question 5

> In at most 50 words, explain how traceroute discovers a path to a remote host. The man page might be useful in answering this question.

Traceroute sends probe packets with a ttl increasing each time until it return 'hit'. The specific value of ttl means a corresponding step with which can tells the certain step in the trace.

## Question 6

>  Show the output of traceroute from each direction above.


	traceroute to www1.slac.stanford.edu (134.79.197.200), 30 hops max, 60 byte packets
	 1  59.78.22.254 (59.78.22.254)  1.665 ms  2.544 ms  3.370 ms
	 2  10.21.18.253 (10.21.18.253)  1.029 ms  1.432 ms  1.833 ms
	 3  10.3.2.118 (10.3.2.118)  0.391 ms  0.392 ms  0.391 ms
	 4  10.3.2.9 (10.3.2.9)  0.386 ms  0.579 ms  0.580 ms
	 5  10.3.2.13 (10.3.2.13)  0.574 ms  0.573 ms  0.767 ms
	 6  10.3.0.10 (10.3.0.10)  0.967 ms  0.974 ms  0.972 ms
	 7  10.255.38.34 (10.255.38.34)  3.099 ms  3.093 ms  2.926 ms
	 8  10.255.38.1 (10.255.38.1)  1.873 ms  2.278 ms  2.070 ms
	 9  10.255.38.254 (10.255.38.254)  1.006 ms  1.066 ms  1.063 ms
	10  202.112.27.1 (202.112.27.1)  2.316 ms  2.106 ms  2.508 ms
	11  101.4.115.174 (101.4.115.174)  1.362 ms  1.342 ms  1.337 ms
	12  101.4.117.30 (101.4.117.30)  33.097 ms  33.106 ms  33.178 ms
	13  101.4.116.118 (101.4.116.118)  33.570 ms  33.591 ms  33.388 ms
	14  101.4.112.69 (101.4.112.69)  32.260 ms * *
	15  101.4.116.134 (101.4.116.134)  80.433 ms 101.4.116.145 (101.4.116.145)  30.298 ms 101.4.116.134 (101.4.116.134)  80.430 ms
	16  202.112.53.6 (202.112.53.6)  35.260 ms  35.264 ms  35.061 ms
	17  210.25.189.65 (210.25.189.65)  64.974 ms  62.565 ms  62.741 ms
	18  210.25.189.18 (210.25.189.18)  27.412 ms  27.621 ms  27.607 ms
	19  210.25.189.50 (210.25.189.50)  178.181 ms  178.160 ms  178.007 ms
	20  210.25.189.142 (210.25.189.142)  183.666 ms  184.069 ms  183.852 ms
	21  rtr-border2-p2p-sunn-cr5.slac.stanford.edu (192.68.191.234)  182.201 ms  182.264 ms  182.234 ms
	22  * * *
	23  * * *
	24  * * *
	25  * * *
	26  * * *
	27  * * *
	28  * * *
	29  * * *
	30  * * *

	---

	traceroute to 59.78.22.110 (59.78.22.110), 30 hops max, 40 byte packets
	 1  rtr-servcore1-serv01-webserv.slac.stanford.edu (134.79.197.130)  0.668 ms  0.420 ms  0.397 ms
	 2  rtr-core2-p2p-serv01-01.slac.stanford.edu (134.79.254.65)  0.673 ms  0.689 ms  0.538 ms
	 3  rtr-fwcore1-trust-p2p-core1.slac.stanford.edu (134.79.254.134)  0.810 ms  0.855 ms  0.690 ms
	 4  rtr-core1-p2p-fwcore1-untrust.slac.stanford.edu (134.79.254.137)  0.939 ms  0.549 ms  0.821 ms
	 5  * * *
	 6  sunncr5-ip-c-slac.slac.stanford.edu (192.68.191.233)  1.731 ms  1.622 ms  1.532 ms
	 7  210.25.189.141 (210.25.189.141)  11.513 ms  10.428 ms  12.365 ms
	 8  210.25.189.49 (210.25.189.49)  155.397 ms  155.299 ms  155.504 ms
	 9  210.25.189.17 (210.25.189.17)  158.972 ms  157.072 ms  177.526 ms
	10  210.25.189.69 (210.25.189.69)  159.638 ms  164.738 ms  166.659 ms
	11  202.112.53.5 (202.112.53.5)  156.968 ms  158.514 ms  156.121 ms
	12  101.4.117.50 (101.4.117.50)  155.812 ms  155.968 ms  155.980 ms
	13  101.4.115.69 (101.4.115.69)  155.761 ms  155.810 ms  163.743 ms
	14  101.4.112.70 (101.4.112.70)  183.800 ms  182.682 ms  183.834 ms
	15  101.4.116.117 (101.4.116.117)  189.183 ms  182.453 ms  184.013 ms
	16  101.4.117.29 (101.4.117.29)  186.244 ms  181.608 ms  181.792 ms
	17  101.4.115.173 (101.4.115.173)  182.655 ms  182.741 ms  183.439 ms
	18  * * *
	19  * * *
	20  * * *
	21  * * *
	22  * * *
	23  * * *
	24  * * *
	25  * * *
	26  * * *
	27  * * *
	28  59.78.22.110 (59.78.22.110)  182.278 ms  182.387 ms  182.195 ms


## Question 7

> Describe anything unusual about the output. Are the same routers traversed in both directions? If not, why might this happen?(a sample is enclosed in the attachment)

It's asymmetric.

It's because the process of routing is asymmetric in order to speed up and enhance the utility of bandwidth.


## Question 8

> Show the output of the above command. Describe what is strange about the observed output, and why traceroute gives you such an output. Refer to the traceroute man page for useful hints.


	traceroute to 18.31.0.200 (18.31.0.200), 30 hops max, 60 byte packets
	 1  59.78.22.254 (59.78.22.254)  1.671 ms  2.534 ms  3.440 ms
	 2  10.21.18.253 (10.21.18.253)  1.040 ms  1.445 ms  1.828 ms
	 3  10.3.2.118 (10.3.2.118)  0.405 ms  0.411 ms  0.410 ms
	 4  10.3.2.9 (10.3.2.9)  0.401 ms  0.401 ms  0.590 ms
	 5  10.3.2.13 (10.3.2.13)  0.592 ms  0.589 ms  0.783 ms
	 6  10.3.0.10 (10.3.0.10)  0.982 ms  0.967 ms  0.967 ms
	 7  10.255.38.34 (10.255.38.34)  1.169 ms  1.168 ms  1.163 ms
	 8  10.255.38.1 (10.255.38.1)  1.865 ms  2.274 ms  3.571 ms
	 9  10.255.38.254 (10.255.38.254)  0.978 ms  1.042 ms  1.037 ms
	10  202.112.27.1 (202.112.27.1)  2.759 ms  2.755 ms  2.542 ms
	11  101.4.115.174 (101.4.115.174)  1.395 ms  1.382 ms  1.377 ms
	12  101.4.117.30 (101.4.117.30)  30.806 ms  30.794 ms  30.972 ms
	13  101.4.116.118 (101.4.116.118)  33.578 ms  33.805 ms  33.805 ms
	14  101.4.112.69 (101.4.112.69)  27.109 ms  27.248 ms  27.272 ms
	15  101.4.116.145 (101.4.116.145)  34.860 ms 101.4.116.134 (101.4.116.134)  31.478 ms  31.457 ms
	16  101.4.117.98 (101.4.117.98)  31.637 ms  31.636 ms  31.623 ms
	17  101.4.117.170 (101.4.117.170)  201.722 ms  200.873 ms  200.726 ms
	18  te0-3-0-10.ccr21.lax04.atlas.cogentco.com (38.88.196.185)  201.606 ms  201.577 ms  201.579 ms
	19  be2017.ccr22.lax01.atlas.cogentco.com (154.54.0.238)  200.649 ms  200.788 ms  200.748 ms
	20  be2066.ccr22.iah01.atlas.cogentco.com (154.54.7.53)  236.220 ms  236.322 ms  236.465 ms
	21  be2173.ccr42.atl01.atlas.cogentco.com (154.54.29.117)  250.688 ms  250.959 ms  250.700 ms
	22  be2171.mpd22.dca01.atlas.cogentco.com (154.54.31.110)  261.524 ms be2169.ccr22.dca01.atlas.cogentco.com (154.54.31.98)  261.697 ms be2171.mpd22.dca01.atlas.cogentco.com (154.54.31.110)  261.395 ms
	23  be2151.ccr42.jfk02.atlas.cogentco.com (154.54.40.74)  268.179 ms be2149.ccr42.jfk02.atlas.cogentco.com (154.54.31.126)  268.074 ms  268.018 ms
	24  be2096.ccr22.bos01.atlas.cogentco.com (154.54.30.42)  273.552 ms  273.600 ms  274.693 ms
	25  38.104.186.186 (38.104.186.186)  264.406 ms  264.017 ms  264.195 ms
	26  * * *
	27  backbone-rtr-1-dmz-rtr-1.mit.edu (18.192.1.2)  264.247 ms  264.002 ms  263.946 ms
	28  * * *
	29  * * mitnet.trantor.csail.mit.edu (18.4.7.65)  264.150 ms
	30  * * *


The strange thing is that in 22nd step, three packages are transmitted at the same time.

It is because the default setting of traceroute:

> -q nqueries, --queries=nqueries Sets the number of probe packets per hop. The default is 3.

Set it into 1 is OK.


## Question 9

> What are Autonomous Systems(AS)? What's the difference between Interior Gateway Protocol(IGP) and Border Gateway Protocol(BGP)?

### Autonumous Systems

An AS is owned and administered by a single commercial entity, and implements some set of policies in deciding how to route its packets to the rest of the Internet, and how to export its routes (its own, those of its customers, and other routes it may have learned from other ASes) to other ASes. Each AS is identified by a unique 16-bit number.

### Difference between IGP and BGP

BGP is used between the ASes where as IGP are used within the same ASes.

## Question 10

> From the path entry data, which Autonomous System (AS) number corresponds to MIT?

AS3