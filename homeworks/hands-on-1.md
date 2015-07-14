#Answer sheet

## Question One

> Please briefly answer the role of DNS in your own words.

DNS is a hierachical name scheme which binds the IP address to domain names.

## Question Two

> The type field have a few different values to indicate the kind of this record. What do "A", "NS" and "CNAME" mean?

- CNAME: The data of this kind of record is another domain name, with which the dig will continue to connect.

- A: The data of this kind of record is an IP address which signals the finish of the DNS lookup.

- NS: The data of this kind of record is the name server which is used for DNS lookup.

## Question Three 
> How can we ask a specific dns server (instead of the default) for information about a domain name? When I use "dig www.baidu.com",the DNS server is 192.168.110.2. However if this server crashed and I have to ask the server 8.8.8.8, what command should I use?

`dig @8.8.8.8 www.baidu.com`

## Question Four

> Do you know the process of solving domain name "lirone.csail.mit.edu"? 

1. `dig . ns` to get a root name server(Here I choose a.root-server.net).

2. `dig @a.root-server.net edu.` to get a name server, which can lookup `edu.`(Here I choose 192.5.6.30).

3. `dig @192.5.6.30 mit.edu.` to get a name server which can lookup `mit.edu.`(Here I choose 184.26.161.64).

4. `dig @184.26.161.64 csail.mit.edu.` to get a name server which can lookup `csail.mit.edu.`(Here I choose 128.30.2.123).

5. `dig @128.30.2.123 lirone.csail.mit.edu.` to get the final IP address 18.26.1.36.

## Question Five

> Please explain the above phenomenon. Have a guess!

### The difference between `dig www.twitter.com +trace` and `dig www.baidu.com +trace`

In the trace of www.twitter.com, I found that the lookup terminated at com. instead of www.twitter.com., while the one of www.baidu.com is normally ended with www.baidu.com.(and a.shifen.com.)

### The difference between `dig www.twitter.com @1.0.0.0` and `dig www.baidu.com @1.0.0.0`

From command `ping 1.0.0.0` I found that it was not existed. However strangely, `dig www.twitter.com @1.0.0.0` can actually return an IP address, while `dig www.baidu.com @1.0.0.0` can not. 

### About `dig www.twitter.com +trace`

After running `dig www.twitter.com +trace`, I found that the return IP address differs each time. The address of the IP address varied all around world. After wiki-ed GFW, I found that to those blocked websites like twitter, because of the DNS poisoning, it will randomly return a valid IP address abroad, which results to the strange situation.

I think when your request of www.twitter.com reach Chinese DNS name server, it will first lookup in its filter list. If it is on the list, it will directly fake an IP address without ask for the real IP address.

## Question Six

> The ips which dig returns to you belong to google indeed. Give the reason for the above phenomenon.

Actually I do not know the exact answer, instead I have a guess that it is because the different policy of GFW. To some of the DNS lookup, it may return a valid fake IP address, to other DNS lookup, it may return the genuine IP address while block your access during your surfing. 
