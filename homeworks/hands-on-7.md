# Answer sheet

## Question 1

> Wal-sys displays the current state of the database contents after you type show_state. Why doesn't the database show studentB?

Because studentB was initialized in action 2 which is not commited or ended.

## Question 2

> When the database recovers, which accounts should be active, and what values should they contain?

StudentA will be 900 while studentC will be 3100

## Question 3

>  Can you explain why the "DB" file does not contain a record for studentC and contains the pre-debit balance for studentA?

Because action 3 is not ended before so that studentC is not actually wrote into db, while studentA is initialized in ended action 1 so that DB contains it.

## Question 4

> What do you expect the state of "DB" to be after wal-sys recovers? Why?

The state:

Account: studentA Value: 900
Account: studentC Value: 3100

Because all commited actions like action 1 and action 3 will be redo and save to db, while all uncommited actions like action 2 will be undo.

## Quesiont 5

> Run wal-sys again to recover the database. Examine the "DB" file. Does the state of the database match your expectations? Why or why not?

Yes. Because the commit actions were not changed so the state of the database match my expectations.

## Question 6

> During recovery, wal-sys reports the action_ids of those recoverable actions that are "Losers", "Winners", and "Done". What is the meaning of these categories?

Winners means the commited but not ended actions which will be redoed, losers means the uncommited actions which will be undoed and done means ended actions which will be redoed.

## Question 7

> Why are the results of recoverable action 2's create_account 2 studentB 2000 command not installed in "DB" by the checkpoint command on the following line?

Because action 2 hasn't commited or ended, so it will be pendding until it is commited or ended.

## Question 8

> How many lines were rolled back? What is the advantage of using checkpoints?

Only six lines were rolled back because of the check points.

The advantage of checkpoints is that it can reduce the cost of recovery by reducing the lines of the log which it should scan.

## Question 9

> Does the second run of the recovery procedure (for sequence 2) restore "DB" to the same state as the first run? What is this property called?

Yes. The property is called durability, which means the transactions committed will permanently survive after recoveries and the uncommitted will never survive.

## Question 10

> Compare the action_ids of "Winners", "Losers", and "Done" from the second recovery with those from the first. The lists are different. How does the recovery procedure guarantee the property from Question 9 even though the recovery procedure can change? (Hint: Examine the "LOG" file).

No. Because the recovery procedure will end all actions after it finishes.

It will only add end to committed actions so that the commit actions which are the one sthat actually change the database are kept unchanged, so the property is guaranteed.

