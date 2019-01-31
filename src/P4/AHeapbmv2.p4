/* -*- P4_16 -*- */
#include <core.p4>
#include <v1model.p4>

/*************************************************************************
*********************** H E A D E R S  ***********************************
*************************************************************************/
const bit<32> size1 = 2400;
const bit<32> size2 = 1200;
const bit<32> size3 = 600;
const bit<32> size4 = 300;
const bit<32> size5 = 150;
const bit<32> size6 = 75;
typedef bit<9>  egressSpec_t;
typedef bit<48> macAddr_t;
typedef bit<32> ip4Addr_t;

header ethernet_t {
    macAddr_t dstAddr;
    macAddr_t srcAddr;
    bit<16>   etherType;
}

header ipv4_t {
    bit<4>    version;
    bit<4>    ihl;
    bit<8>    diffserv;
    bit<16>   totalLen;
    bit<16>   identification;
    bit<3>    flags;
    bit<13>   fragOffset;
    bit<8>    ttl;
    bit<8>    protocol;
    bit<16>   hdrChecksum;
    ip4Addr_t srcAddr;
    ip4Addr_t dstAddr;
}

header tcp_t {
    bit<16> srcPort;
    bit<16> dstPort;
    bit<32> seqNo;
    bit<32> ackNo;
    bit<4>  dataOffset;
    bit<3>  res;
    bit<3>  ecn;
    bit<6>  ctrl;
    bit<16> window;
    bit<16> checksum;
    bit<16> urgentPtr;
}

register<bit<32>>(1) ctime;
register<bit<104>>(size1) flowtable1;
register<bit<32>>(size1) counttable1;
register<bit<32>>(size1) timetable1;
register<bit<104>>(size2) flowtable2;
register<bit<32>>(size2) counttable2;
register<bit<32>>(size2) timetable2;
register<bit<104>>(size3) flowtable3;
register<bit<32>>(size3) counttable3;
register<bit<32>>(size3) timetable3;
register<bit<104>>(size4) flowtable4;
register<bit<32>>(size4) counttable4;
register<bit<32>>(size4) timetable4;
register<bit<104>>(size5) flowtable5;
register<bit<32>>(size5) counttable5;
register<bit<32>>(size5) timetable5;
register<bit<104>>(size6) flowtable6;
register<bit<32>>(size6) counttable6;
register<bit<32>>(size6) timetable6;


struct metadata {
    bit<104> flowid;
    bit<32> count;
    bit<32> time;
    bit<1> finished;
}

struct headers {
    ethernet_t   ethernet;
    ipv4_t       ipv4;
    tcp_t      tcp;
}


/*************************************************************************
*********************** P A R S E R  ***********************************
*************************************************************************/

parser MyParser(packet_in packet,
                out headers hdr,
                inout metadata meta,
                inout standard_metadata_t standard_metadata) {
    state start {
        packet.extract(hdr.ethernet);
        packet.extract(hdr.ipv4);
        packet.extract(hdr.tcp);
        transition accept;
    }
}

/*************************************************************************
************   C H E C K S U M    V E R I F I C A T I O N   *************
*************************************************************************/

control MyVerifyChecksum(inout headers hdr, inout metadata meta) {   
    apply {}
}


/*************************************************************************
**************  I N G R E S S   P R O C E S S I N G   *******************
*************************************************************************/
control MyIngress(inout headers hdr,
                  inout metadata meta,
                  inout standard_metadata_t standard_metadata) {
    apply{
        //initialize
        meta.flowid[31:0] = hdr.ipv4.srcAddr;
        meta.flowid[47:32] = hdr.tcp.srcPort;
        meta.flowid[79:48] = hdr.ipv4.dstAddr;
        meta.flowid[95:80] = hdr.tcp.dstPort;
        meta.flowid[103:96] = hdr.ipv4.protocol;
        meta.count = 1;
        ctime.read(meta.time, 0);
        ctime.write(0, meta.time + 1);
        meta.finished = 0;
    }
}

/*************************************************************************
****************  E G R E S S   P R O C E S S I N G   *******************
*************************************************************************/

control MyEgress(inout headers hdr,
                 inout metadata meta,
                 inout standard_metadata_t standard_metadata) {
    apply{
        
        //1-3 dleft
        //layer1
        if(meta.finished == 0){
            bit<32> index;
            bit<32> tmpcount;
            hash(index, HashAlgorithm.crc16, (bit<32>)0, {meta.flowid}, size1);
            counttable1.read(tmpcount, index);
            if(tmpcount == 0){
                flowtable1.write(index, meta.flowid);
                counttable1.write(index, meta.count);
                timetable1.write(index, meta.time);
                meta.finished = 1;
            }
            else{
                bit<104> tmpflowid;
                flowtable1.read(tmpflowid, index);
                if(tmpflowid == meta.flowid){
                    counttable1.write(index, tmpcount + meta.count);
                    timetable1.write(index, meta.time);
                    meta.finished = 1;
                }
                else{
                    bit<32> tmptime;
                    timetable1.read(tmptime, index);
                    if(tmpcount == 1 || meta.time - tmptime > 10000){
                        flowtable1.write(index, meta.flowid);
                        counttable1.write(index, meta.count);
                        timetable1.write(index, meta.time);
                        meta.finished = 1;
                    }
                }
            }
        }
        //layer2
        if(meta.finished == 0){
            bit<32> index;
            bit<32> tmpcount;
            hash(index, HashAlgorithm.crc16, (bit<32>)0, {meta.flowid}, size2);
            counttable2.read(tmpcount, index);
            if(tmpcount == 0){
                flowtable2.write(index, meta.flowid);
                counttable2.write(index, meta.count);
                timetable2.write(index, meta.time);
                meta.finished = 1;
            }
            else{
                bit<104> tmpflowid;
                flowtable2.read(tmpflowid, index);
                if(tmpflowid == meta.flowid){
                    counttable2.write(index, tmpcount + meta.count);
                    timetable2.write(index, meta.time);
                    meta.finished = 1;
                }
                else{
                    bit<32> tmptime;
                    timetable2.read(tmptime, index);
                    if(tmpcount == 1 || meta.time - tmptime > 10000){
                        flowtable2.write(index, meta.flowid);
                        counttable2.write(index, meta.count);
                        timetable2.write(index, meta.time);
                        meta.finished = 1;
                    }
                }
            }
        }
        //layer3
        if(meta.finished == 0){
            bit<32> index;
            bit<32> tmpcount;
            hash(index, HashAlgorithm.crc16, (bit<32>)0, {meta.flowid}, size3);
            counttable3.read(tmpcount, index);
            if(tmpcount == 0){
                flowtable3.write(index, meta.flowid);
                counttable3.write(index, meta.count);
                timetable3.write(index, meta.time);
                meta.finished = 1;
            }
            else{
                bit<104> tmpflowid;
                flowtable3.read(tmpflowid, index);
                if(tmpflowid == meta.flowid){
                    counttable3.write(index, tmpcount + meta.count);
                    timetable3.write(index, meta.time);
                    meta.finished = 1;
                }
                else{
                    bit<32> tmptime;
                    timetable3.read(tmptime, index);
                    if(tmpcount == 1 || meta.time - tmptime > 10000){
                        flowtable3.write(index, meta.flowid);
                        counttable3.write(index, meta.count);
                        timetable3.write(index, meta.time);
                        meta.finished = 1;
                    }
                }
            }
        }
        //4-6 winnertree
        //layer4
        if(meta.finished == 0){
            bit<32> index;
            bit<32> tmpcount;
            hash(index, HashAlgorithm.crc16, (bit<32>)0, {meta.flowid}, size4);
            counttable4.read(tmpcount, index);
            if(tmpcount == 0){
                flowtable4.write(index, meta.flowid);
                counttable4.write(index, meta.count);
                timetable4.write(index, meta.time);
                meta.finished = 1;
            }
            else{
                bit<104> tmpflowid;
                flowtable4.read(tmpflowid, index);
                if(tmpflowid == meta.flowid){
                    counttable4.write(index, tmpcount + meta.count);
                    timetable4.write(index, meta.time);
                    meta.finished = 1;
                }
                else{
                    bit<32> tmptime;
                    timetable4.read(tmptime, index);
                    if(meta.count + meta.time > tmpcount + tmptime){
                        flowtable4.write(index, meta.flowid);
                        counttable4.write(index, meta.count);
                        timetable4.write(index, meta.time);
                        meta.flowid = tmpflowid;
                        meta.count = tmpcount;
                        meta.time = tmptime;
                    }
                }
            }
        }
        //layer5
        if(meta.finished == 0){
            bit<32> index;
            bit<32> tmpcount;
            hash(index, HashAlgorithm.crc16, (bit<32>)0, {meta.flowid}, size5);
            counttable5.read(tmpcount, index);
            if(tmpcount == 0){
                flowtable5.write(index, meta.flowid);
                counttable5.write(index, meta.count);
                timetable5.write(index, meta.time);
                meta.finished = 1;
            }
            else{
                bit<104> tmpflowid;
                flowtable5.read(tmpflowid, index);
                if(tmpflowid == meta.flowid){
                    counttable5.write(index, tmpcount + meta.count);
                    timetable5.write(index, meta.time);
                    meta.finished = 1;
                }
                else{
                    bit<32> tmptime;
                    timetable5.read(tmptime, index);
                    if(meta.count + meta.time > tmpcount + tmptime){
                        flowtable5.write(index, meta.flowid);
                        counttable5.write(index, meta.count);
                        timetable5.write(index, meta.time);
                        meta.flowid = tmpflowid;
                        meta.count = tmpcount;
                        meta.time = tmptime;
                    }
                }
            }
        }
        //layer6
        if(meta.finished == 0){
            bit<32> index;
            bit<32> tmpcount;
            hash(index, HashAlgorithm.crc16, (bit<32>)0, {meta.flowid}, size6);
            counttable6.read(tmpcount, index);
            if(tmpcount == 0){
                flowtable6.write(index, meta.flowid);
                counttable6.write(index, meta.count);
                timetable6.write(index, meta.time);
                meta.finished = 1;
            }
            else{
                bit<104> tmpflowid;
                flowtable6.read(tmpflowid, index);
                if(tmpflowid == meta.flowid){
                    counttable6.write(index, tmpcount + meta.count);
                    timetable6.write(index, meta.time);
                    meta.finished = 1;
                }
                else{
                    bit<32> tmptime;
                    timetable6.read(tmptime, index);
                    if(meta.count + meta.time > tmpcount + tmptime){
                        flowtable6.write(index, meta.flowid);
                        counttable6.write(index, meta.count);
                        timetable6.write(index, meta.time);
                        meta.flowid = tmpflowid;
                        meta.count = tmpcount;
                        meta.time = tmptime;
                    }
                }
            }
        }
    }
}

/*************************************************************************
*************   C H E C K S U M    C O M P U T A T I O N   **************
*************************************************************************/

control MyComputeChecksum(inout headers hdr, inout metadata meta) {
     apply {}
}

/*************************************************************************
***********************  D E P A R S E R  *******************************
*************************************************************************/

control MyDeparser(packet_out packet, in headers hdr) {
    apply {
        /* TODO: add deparser logic */
        packet.emit(hdr.ethernet);
        packet.emit(hdr.ipv4);
        packet.emit(hdr.tcp);
    }
}

/*************************************************************************
***********************  S W I T C H  *******************************
*************************************************************************/

V1Switch(
MyParser(),
MyVerifyChecksum(),
MyIngress(),
MyEgress(),
MyComputeChecksum(),
MyDeparser()
) main;




