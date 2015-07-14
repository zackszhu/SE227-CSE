# Answer sheet

## Question 1

> A listing of all processes that you are currently running on the Athena machine you are using, sorted by the command name in alphabetical order (i.e. a process running acroread should be listed before a process runningzwgc). The output should consist only of the processes you are running, and nothing else (i.e. if you are running 6 processes, the output should only have 6 lines). 

Command: `ps -e --sort=cmd`

## Question 2

>The number of words in the file /usr/share/dict/words which do not contain any of the letters a, e, i, o, or u.

Command: `cat /usr/share/dict/words | grep -c -v "a\|e\|i\|o\|u"`

## Question 3

> A 5x6 matrix of entries of alternating 1's and 0's. 

Command: yes '1 0' | fmt -10 | head -6

## Question 4

> A "long" listing of the smallest 5 files in the /etc directory whose name contains the string ".conf", sorted by increasing file size. 

Command: ls -Slr | grep .conf | head -5

## Question 5

> Try the above commands in a few different directories. What happens if you try both of the commands in the /etc directory on athena? How else can the second command not produce the same output as the first? Can you think of any negative side effects that the second construction might cause for the user? You might want to think about the commands you used to solve the first four questions and consider their behavior.

When I tried these commands in the /etc directory, it shows that I do not have the permission to create file 'temp'

If the disk is too full that temp cannot be created, it will also not produce the same output. 

If there is already a file named 'temp' in the disk, the second command will overwrite the old one, while the first command one will not.

## Question 6

> Compare the two temp files. Based on your understanding of file I/O in UNIX, what is going on here, and why? Is this different from what you would expect? (If there is more than one difference between the two files, it is the ordering of the letters y and n that we are interested in). 

The temp file create with `;` outputs `y n y n` while the one create with `&` outputs `y y n n`.

I think the reason is that `;` means to do two things one by one while `&` means to do things synchronously. Because of the sleep before two echo, the 'y' of second 'myyes' goes directly after the first 'y', and it is just what I expected.

## Question 7

> The paper describes the Unix system call interface in some detail. In particular, the read and write system calls do not take the offset as an argument. Why did the Unix designers not include the offset as an argument to read and write? How would an application write to a specific offset in a file?

There is a pointer maintained by the system which indicates the next byte to be read or written, so there is no need to take the offset as an argument. If an application would like to write to a specific offset in a file, it can use the function`seek`

## Question 8

> Change to the '.' entry in your new directory. What happens to your working directory? Next, change to the '..' entry. What happens to your working directory?

After I `cd .`, nothing happens.

After I `cd ..`, it changed into the parent directory of current directory.

## Question 9

> Describe a scenario where you might need to use the '.' directory.

When I need to represent all files in the current directory, I will need '.' directory. For example, when I need to add all files to my git, I can type `git add .`.

## Question 10

> What has changed in the stat output and why has it changed?

Nothing changed except the times including atime, ctime and mtime.

## Question 11 

> What has changed in the stat output this time and why has it changed? Why does the link count only change when you create a new directory?

This time the number of links increased. Because only a new directory will produce a new '..' link while new files can not, so the link count only changed when creating a new directory.

## Question 12

> One reason for supporting symbolic links is to allow linking from one disk to another disk, but you can also create a symbolic link to a file on the same disk. Name one advantage and one disadvantage of using symbolic links on the same disk.

Advantage: We can create shortcut to compress the deep directory tree by symbolic links.

Disadantage: The dangling-link problem may occur.

## Question 13

> What happened? Why?

The `ls` result shows that there is a directory named 'b', however `cd` shows that it can not be cd to '../b'.

The result is that you are actually in the '/d/e' directory while when you type `ls`, the shell will do the optimization that shows the parent directory of your link, but `cd` will not do this optimization.

## Question 14

> How would you change the file system to make this command (cd d/e; cd ../b) actually change to your current working directory (a/b or equally d/e)?

By using the command, we can use `cd -P ../b`.

By changing the file system, I think we don't need to create a new file and  just using a not-adding-reference type of hard link is OK.

## Question 15

> What happened to ls? Why is not it listing files like it did before? (Hint: set your path back to its original state: export PATH=$OLDPATH)

`ls` output nothing except a cute statement. It is because the environment variable is changed and `ls` is found not in '/usr/bin' but in this directory with this fake `ls`.

## Question 16

> What does 1 represent in the first argument of write?

It represents 'stdout' which output contents on the screen.

## Question 17 

> Name one advantage and one disadvantage of the linked list strategy.

Advantage: easy to maintain free list.

Disadvantage: not support random accessing.

## Question 18

> Some shells like bash try to make '..' always work properly, namely, cd d/e; cd ../b will place you in your current directory. Does bash always get this behavior correct?

It seems to be correct.
