./lock_server 12345 > lock_server.log 2>&1 &
sleep 1
echo lock_server finished
./extent_server 12346 > extent_server1.log 2>&1 &
sleep 1
echo extent_server finished
rm -rf yfs1
mkdir yfs1 || exit 1
sleep 1
echo yfs1 created
rm yfs_client1.log > /dev/null 2>&1
./run-client.sh yfs1 12346 12345
sleep 1
echo hahaha