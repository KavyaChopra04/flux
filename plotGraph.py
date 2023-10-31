import matplotlib
import matplotlib.pyplot as plt
matplotlib.use('TkAgg')
with open('log.txt') as f:
    lines = f.readlines()
lines = lines[3:]
xReceive = []
yReceive  = []
xSend = []
ySend = []
for line in lines:
    line = line.split(" ")
    if(line[0]=='S'):
        xSend.append(int(line[2]))
        ySend.append(int(line[1]))
    else:
        xReceive.append(int(line[2]))
        yReceive.append(int(line[1]))
print(len(xSend), len(xReceive))
plt.scatter(xSend, ySend, label = "Send", color ="blue")
plt.scatter(xReceive, yReceive, label = "Receive", color="orange")
plt.xlabel('Time (in microseconds)')
plt.ylabel('Offest = Number of packets * Maxbytes')
plt.show()
