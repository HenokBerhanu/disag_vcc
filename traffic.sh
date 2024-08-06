#################### Collect CPU and memory consumption for gNB-DU ####################################

./enter_container.sh rfsim5g-oai-nr-ue
./enter_container.sh rfsim5g-oai-du

inside the rfsim5g-oai-du container: iperf3 -s
inside the rfsim5g-oai-nr-ue container:  iperf3 -u -c 192.168.0.15 -b 128k -t 36000  # 192.168.0.15
inside the rfsim5g-oai-nr-ue container:  iperf3 -u -c 192.168.0.14 -b 4M -t 36000    # 192.168.0.15

ping -c 2400 192.168.0.15

iperf3 -u -c 192.168.0.14 -b 128k -t 36000    # 192.168.0.15