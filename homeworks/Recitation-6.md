# Answer sheet

## Question 1

> Use your own word to describe different types of “RAID”. Why there are so many different types of RAID? Which scenarios are suitable for different types?

RAIDs:

- RAID1: Every data disk has a corresponding backup check disk in a group.

- RAID2: Data were bit-interleaved in all data disks in a group, while several check disks were set to determine which disk crashed and recovery it.

- RAID3: Data were bit-interleaved in all data disks in a group, while only one check disk were set to recovery data. Which disk was crashed was determined by each data disk itself.

- RAID4: Data were not bit-interleaved any more, same files were stored in same data disks. Only one disk were set to recovery data, while which disk was crashed was determined by each data disk itself.

- RAID5: Data were not bit-interleaved any more, same files were stored in same data disks. The checksum was not stored in one check disk anymore, it was distributed in every disk instead. Moreover, the disks still need to determine if itself was crashed.

The reason of so many RAID types is because both the development of hardware and different scenarios. 

The RAID1 is suitable for all scenarios when you don't care about space utility and have a primitive RAID controller.

Both RAID2 and RAID3 are suitable for single large file storage which mostly appeared in supercomputers. RAID3 requires a more advanced RAID controller.

Both RAID4 and RAID5 are suitable for both single large file storage and several small storage which mostly appeared in transaction-processing system. RAID5 required a more advanced RAID controller too.

## Question 2

> Modern RAID arrays use parity information and standby disks to provide a highly reliable storage medium even in the face of hardware failures. A highly reliable system, however, requires more than just a highly reliable storage medium. Consider a networked server handling network transactions (a web server or bank central computer, perhaps). Think about other components of this system whose failure could result in a loss of service. Identify one other component (software or hardware) of such a system that could be appropriately designed along the lines of one or another of the RAID levels, and describe the approach briefly or demonstrate in a picture, indicating how it relates to which RAID level.

In a distributed RPC server, the client sends requests to the master server. If a request requires many actions or functions to be processed, the master server will assign the works to worker server groups. Worker servers in the worker server group can tell the check server in the same group if itself was crashed and then the check server can recover the failure by redo the work according to parity information store in itself like original parameters. 

This approach is designed along the 3th layer of RAID layers. The master server acts as the RAID controller, the check servers act as the check disk in each group and the worker servers act as the data disk in each group.

## My question

What if the checksum in the check disk in layer 4 stored seperately according to different data disks and each part provides seperate I/O ports and can R/W concurrently, can the performance be as good as layer 5? Is it easier to implement?



