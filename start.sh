sec=1
cnt=0
name=Light
Thread=`ps -ef | grep $name | grep -v "grep"`
sudo chmod 777 /dev/ttyUSB*
cd /home/nvidia/code/Light/cmake-build-debug/
while [ 1 ]
do
count=`ps -ef | grep $name | grep -v "grep" | wc -l`
echo "Thread count: $count"
echo "Expection count: $cnt"
if [ $count -gt 1 ]; then
	echo "The $name is still alive!"
	sleep $sec
else 
	echo "Starting $name..."
    cd /home/nvidia/code/Light/cmake-build-debug/
    ./Light
    echo "$name has started!"		
	sleep $sec
	((cnt=cnt+1))
fi
done
exit 0

