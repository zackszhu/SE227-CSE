# Answer sheet

## Question 1

> Use your own word to describe “end-to-end” argument.

'end-to-end' argument is something stored in the packets. When the receiver client received packet, it will directly ask the sender client to verify if the argument is the corresponding one. All the actions will be done at application layer instead of low-level layer.

## Question 2

> Give at lease three cases that are suitable to use this principle.


- Delivery guarantees: Using RFNM mechanic at application level

- Secure transmission of data: Encryption and decryption are all provided at application layer and no need of low-level part to provide

- Duplicate message suppression: The application layer determine if it has got a duplicate message by end-to-end communication

- Other applications: Guaranteeing FIFO message delivery, Transaction management

## Question 3

> Give at lease three cases that are NOT suitable to use this principle.

- On micro-controller like MCU, the CPU and memory not allow too much high level calculation and communication, end-to-end is not suitable.

- When you don't know where your corresponding end is, then end-to-end is not suitable.

- For high security data, users don't want to re-send their message twice.

## Question 4

> Consider the design of the file system based on inode (as we learn from class and the lab). Which part(s) of the design do you think can be removed? In another word, which part(s) of the design is(are) not flexible enough and should be implemented by the user? (Hint: don’t be limited by the FS API)

The journal behavior. File system would use journal to log the transaction happened before, and it may be not flexible enough. It should offer an API to allow user to change Log's behavior.

## Question 5

> The same question, for the OS.

The desktop environment. It's outside the kernel and can be implement by user, just like KDE and GNOME.

## My question

If the end itself is wrong, like it always fake a right response instead of asking the sender, is it still necessary to implement low-level security guarantee?