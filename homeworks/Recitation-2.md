# Answer

## Question 1

> What is the problem this paper tried to solve? (Hint: please state the problem using data)

The original file system is of 512 byte file and only provides only about two percent of the maximum disk bandwidth or about 20 Kb per second per arm. It is too slow and the file system should upgrade into 4096 byte file with not so much space wasted. Several updates are applied to improve the performance.

## Question 2

> What is the root cause of the problem? In one sentence.

The block size of the file system is too small.
 
## Question 3

> For each optimization, how does the author get the idea? (Hint: just imagine you’re the author. Given the problem, what experiment would you do to get more info?)

- increse block size into 4096 bytes: the memory page is 4k byte

- take fragments in a block: would like to act more likely to the old one in space utility

- file system parameterization: thinking of serve a specific computer instead provide a common answer

- layout system: using layer design to specify every parts' work

## Question 4

> For each optimization, does it always work? In which case it cannot work?

No. 

- For extend to 4096 byte block size, I think when it meets a disk with a lot of small files, then it will waste a lot, and even worse than old version.

- For the fragment method, from the chart, I can see the CPU is used a lot. If the processor is too bad, it will performance bad. In addition, it can only store 10% less files in the disk.

## Question 5

> Do you have some better ideas to implement a faster FS?

When write small things use a cache to record, if the cache is full then write back to the disk.

## My question

What is the real imprevement of this FS?
