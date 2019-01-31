# CAHeap OVS project
***
## Environment
- `OVS-2.5.1` 
- `iperf-2.0.5`
- `Linux kernel version 4.4.0`

## How to Install
1. Copy `OVS/*.h` to `$OVS_DIR/datapath`
2. Replace the `$OVS_DIR/datapath/datapath.h` and `$OVS_DIR/datapath/datapath.c` with `datapath.h` and `datapath.c`

## How to Run
1. Compile and start OVS
2. We need at least two processes to start the measurement with iperf, one as server and the other as client.

```
Terminal1:
    # add the namespaces
    ip netns add ns1
    ip netns add ns2
    # create the switch
    BRIDGE=ovs-test
    ovs-vsctl add-br $BRIDGE
    #
    #### PORT 1
    # create a port pair
    ip link add tap1 type veth peer name ovs-tap1
    # attach one side to ovs
    ovs-vsctl add-port $BRIDGE ovs-tap1 
    # attach the other side to namespace
    ip link set tap1 netns ns1
    # set the ports to up
    ip netns exec ns1 ip link set dev tap1 up
    ip link set dev ovs-tap1 up
    #
    #### PORT 2
    # create a port pair
    ip link add tap2 type veth peer name ovs-tap2
    # attach one side to ovs
    ovs-vsctl add-port $BRIDGE ovs-tap2 
    # attach the other side to namespace
    ip link set tap2 netns ns2
    # set the ports to up
    ip netns exec ns2 ip link set dev tap2 up
    ip link set dev ovs-tap2 up
    #
    ip netns exec ns1 bash
    ifconfig tap1 10.0.0.1 netmask 255.255.255.0 up
    iperf -s

Terminal2:
    ip netns exec ns2 bash
    ifconfig tap2 10.0.0.2 netmask 255.255.255.0 up
    iperf -c 10.0.0.1 -P 3
```