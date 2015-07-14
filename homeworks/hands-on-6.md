# Answer sheet

## Question 1

> What are the IP addresses of maple and willow on this network? (Hint: Check the man page of tcpdump to discover how you can obtain the IP addresses)

After browsing the website, I found this:

> Display IP addresses and port numbers instead of domain and service names when capturing packets (note: on some systems you need to specify -nn to display port numbers):

	tcpdump -n

So I change the instruction into:

	tcpdump -n -r Downloads/tcpdump.dat > outfile2.txt   

Then I can get:

The IP address of maple.csail.mit.edu is 128.30.4.222

The IP address of willow.csail.mit.edu is 128.30.4.223

## Question 2

> How many kilobytes were transferred during this TCP session, and how long did it last? Based on these, what is the throughput (in KiloBytes/sec) of this TCP flow between willow and maple?

From the log, I got that 1572890 bytes(1536.025390625 KB) were transferred during this TCP session.

The time lasts 2.865979 seconds.

The throughput equals 1536.025390625 / 2.865979 which values 535.951376694 KB/sec

## Question 3

> What is the round-trip time (RTT) in seconds, between willow and maple, based on packet 1473:2921 and its acknowledgment? Look at outfile.txt and find the round-trip time of packet 13057:14505. Why are the two values different?

RTT of 1473:2921 = 0.007822s

RTT of 13057:14505 = 0.024381s

Because the large window size blocked the bottleneck which slowed down the RTT.


## Question 4

> You may notice that the white arrows occur always close to a "transition" in the green line. Do they occur to the left (i.e. before) or to the right of (i.e. after) these transitions? Why do you think this is the case?

The white arrows mostly occur to the right of the transitions. It happened because of the window theory: when an ACK received, a new packet will immediately be sent to get the max size of the window of the network. So, a white arrow will occur after a transition of green line.

## Question 5

> Find the lowest sequence number of a packet which was re-transmitted after 3 duplicate ACKs. You are permitted to round the sequence number to the nearest 1000 (e.g. you can say 13000:15000 instead of 13057:14505).

The lowest sequence number of the re-transmitted packet is 41500-44500

## Question 6

> Find the lowest sequence number of a packet which was re-transmitted after a time-out. You are permitted to round the sequence number to the nearest 1000.

The lowest sequence number of the re-transmitted packet is 662100-652000

## Question 7

> What aspect of TCP's behavior causes these white arrows to lie approximately on a line?

The sliding window promised that whenever an ACK was accepted, a new packet was sent, so the bottleneck is what that matters.

Because the bottleneck throughput is fixed, so the white arrows lied approximately on a line.

## Question 8

> Can you provide an equation that relates the slope of the above line to the average congestion window size (in Bytes) and the average round-trip time (RTT) of the TCP connection?

Slope *k* = windowSize / RTT

## Question 9

> For the path between willow and maple, the median inter-arrival time for a packet pair was estimated to be 0.0013 seconds. Each packet in the packet pair contained 1500 Bytes. What is the bottleneck capacity of the path (in KiloBytes/sec) between willow and maple, based on this inter-arrival time?

1500 / 0.0013 = 1153846.153846154 Byte/second = 1126.802884615 KB/s

## Question 10

> Give a potential reason why the throughput of the TCP connection, estimated in Question 2, is different from the capacity of the bottleneck link.

The TCP connection may not always at the max capacity of the bottleneck, instead, it may decrease the throughput when re-transmitted or time-out.

