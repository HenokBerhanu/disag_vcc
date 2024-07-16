#################### Collect data for the latency computation ####################################

./enter_container.sh ue
mkdir result
ping -c 100 8.8.8.8 > result/pingwithoutblockchain.txt
ping -c 10 8.8.8.8 > result/pingwithblockchain.txt
sudo docker cp a0a4569e9073:/ue/result/pingwithblockchain.txt /home/henok/ntn-5g-bl/results

# tc qdisc add dev <interface> root netem delay <delay> loss <loss>
# tc qdisc add dev eth0 root netem delay 20ms loss 5%   # for leo: 150ms 10%, for geo: 250ms 15%
# tc qdisc add dev eth0 root netem delay 8ms
# tc qdisc del dev uesimtun0 root netem

# iptables -A INPUT -m statistic --mode random --probability <probability> -j DROP
# iptables -A INPUT -m statistic --mode random --probability 0.05 -j DROP
# iptables -D INPUT -m statistic --mode random --probability 0.2 -j DROP

iptables -A INPUT -m statistic --mode random --probability 0.05 -j DROP
iptables -A INPUT -m statistic --mode random --probability 0.08 -j DROP
iptables -A INPUT -m statistic --mode random --probability 0.11 -j DROP
iptables -A INPUT -m statistic --mode random --probability 0.13 -j DROP

iptables -D INPUT -m statistic --mode random --probability 0.05 -j DROP
iptables -D INPUT -m statistic --mode random --probability 0.08 -j DROP
iptables -D INPUT -m statistic --mode random --probability 0.11 -j DROP
iptables -D INPUT -m statistic --mode random --probability 0.13 -j DROP


tc qdisc add dev <interface> root netem delay <delay> loss <loss>
tc qdisc add dev eth0 root netem delay 5ms loss 2%   # for leo: 150ms 10%, for geo: 250ms 15%
tc qdisc add dev eth0 root netem delay 10ms loss 4%
tc qdisc add dev eth0 root netem delay 15ms loss 6%
tc qdisc add dev eth0 root netem delay 25ms loss 10%
iperf3 -u -c 192.168.30.3 -B 192.168.30.5 -p 5201 -b 128k -R -i 1 -t 20 > result/iperfwithblockchainnew.txt
tc qdisc add dev eth0 root netem delay 8ms
tc qdisc del dev eth0 root netem
tc qdisc del dev eth0 root


# ./enter_container ue
# mkdir res
iperf3 -u -c 192.168.30.3 -B 192.168.30.5 -p 5201 -b 128k -R -i 1 -t 100 > result/iperfwithblockchain1.txt
iperf3 -u -c 192.168.15.3 -B 10.60.0.1 -p 5201 -b 128k -R -i 1 -t 250 > res/voipuav.txt
iperf3 -u -c <container_ip> -B <client_bind_ip> -p 5201 -b 128k -R -i 1 -t 100 > result/iperfwithblockchain1.txt
iperf3 -u -c 192.168.30.3 -B 192.168.30.5 -p 5201 -b 128k -R -i 1 -t 100 > result/iperfwithblockchain2.txt
iperf3 -u -c 192.168.30.3 -B 192.168.30.5 -p 5201 -b 128k -R -i 1 -t 100 > result/iperfwithblockchain3.txt
iperf3 -u -c 192.168.30.3 -B 192.168.30.5 -p 5201 -b 128k -R -i 1 -t 100 > result/iperfwithblockchain4.txt

sudo docker cp a0a4569e9073:/ue/result/iperfwithblockchainnew.txt /home/henok/ntn-5g-bl/results
sudo docker cp 4a2230482e47:/ue/result/iperfwithblockchain2.txt /home/henok/ntn-5g-bl/results
sudo docker cp f00ce0e67998:/ue/result/iperfwithblockchain3.txt /home/henok/ntn-5g-bl/results
sudo docker cp f00ce0e67998:/ue/result/iperfwithblockchain4.txt /home/henok/ntn-5g-bl/results

iperf3 -s -i 1


# Authorize the traffic from gNB to the gateway
curl -X POST http://172.23.0.3:5000/authorize_device -H "Content-Type: application/json" -d '{"device_address": "0x7db8a5b543406b96390a0703ce2e28bc84eb6529"}'
# Authorize the traffic from gateway to the satellite
curl -X POST http://172.23.0.3:5000/authorize_device -H "Content-Type: application/json" -d '{"device_address": "0xd98f4441038010281073ce4e96a8c4fa1a2b1d96"}'
# Authorize the traffic from the satellite to the terminal
curl -X POST http://172.23.0.3:5000/authorize_device -H "Content-Type: application/json" -d '{"device_address": "0x5b69f20f75ab4b52455261fa0b887b1974e7a355"}'