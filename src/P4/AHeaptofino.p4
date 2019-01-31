/* -*- P4_14 -*- */
#ifdef __TARGET_TOFINO__
#include <tofino/constants.p4>
#include <tofino/intrinsic_metadata.p4>
#include <tofino/primitives.p4>
//Include the blackbox definition
#include <tofino/stateful_alu_blackbox.p4>
#else
#warning This program is intended for Tofino P4 architecture only
#endif

#define DLEFT1_LEN 2048
#define DLEFT2_LEN 1024
#define WINNER3_LEN 512
#define WINNER4_LEN 256
#define DLEFT1_HASH_LEN 11
#define DLEFT2_HASH_LEN 10
#define WINNER3_HASH_LEN 9
#define WINNER4_HASH_LEN 8

header_type ethernet_t {
    fields {
        dmac : 48;
        smac : 48;
        ethertype : 16;
    }
}
header ethernet_t ethernet;

header_type ipv4_t {
    fields {
        ver : 4;
        len : 4;
        diffserv : 8;
        totalLen : 16;
        id : 16;
        flags : 3;
        offset : 13;
        ttl : 8;
        proto : 8;
        csum : 16;
        sip : 32;
        dip : 32;
    }
}
header ipv4_t ipv4;

header_type tcp_t {
    fields {
        sPort : 16;
        dPort : 16;
        seqNo : 32;
        ackNo : 32;
        dataOffset : 4;
        res : 3;
        ecn : 3;
        ctrl : 6;
        window : 16;
        checksum : 16;
        urgentPtr : 16;
    }
}
header tcp_t tcp;

header_type udp_t {
    fields {
        sPort : 16;
        dPort : 16;
        hdr_length : 16;
        checksum : 16;
    }
}
header udp_t udp;

header_type user_metadata_t {
    fields {
        timestamp : 32;
        matched_1 : 1;
        matched_2 : 1;
        matched_3 : 1;
        matched_4 : 1;
        flowid : 32;
        tmptimestamp_1 : 32;
        tmptimestamp_2 : 32;
        min : 32;
        min2 : 32;
        minlayer : 3;
        random : 22;
        shifted : 20;
        mymask : 22;
    }
}
metadata user_metadata_t md;

parser start {
    return parse_ethernet;
}

parser parse_ethernet {
    extract(ethernet);
    return select(latest.ethertype) {
        0x0800 : parse_ipv4;
    }
}
parser parse_ipv4 {
    extract(ipv4);
    return select(latest.proto) {
        6 : parse_tcp;
        17: parse_udp;
    }
}
parser parse_tcp {
    extract(tcp);
    return ingress;
}
parser parse_udp {
    extract(udp);
    return ingress;
}

/******************************************************************************
 *
 * IPV4 ROUTING
 *
 *****************************************************************************/
action hop_ipv4(egress_port) {
    modify_field(ig_intr_md_for_tm.ucast_egress_port, egress_port);
}

table ipv4_routing {
    reads {
        ipv4.dip : exact;
    }
    actions {
        hop_ipv4;
    }
    size: 512;
}
/******************************************************************************
 *
 * Initialize
 *
 *****************************************************************************/
register current_timestamp{
    width : 32;
    instance_count : 1;
}

blackbox stateful_alu get_time{
    reg : current_timestamp;

    update_lo_1_value : register_lo + 1;

    output_value : alu_lo;
    output_dst : md.timestamp;
}

action Initialize() {
    get_time.execute_stateful_alu(0);
	modify_field(md.flowid, ipv4.sip);//32bit RScode of flowid, for simplicity, we now just use the source ip address
}

@pragma stage 0
table InitializeTable {
	actions {Initialize;}
	size: 1;
}

/******************************************************************************
 *
 * layer1 A
 *
 *****************************************************************************/
//layer1 A

field_list flow_id_list{
    md.flowid;
}

field_list_calculation dleft_hash_1 {
    input { flow_id_list; }
    algorithm: crc32;
    output_width: DLEFT1_HASH_LEN;
}

register flowid_1 {
    width : 32;
    instance_count: DLEFT1_LEN;
}

blackbox stateful_alu flowid_alu_1 {
    reg: flowid_1;

    condition_lo: register_lo == md.flowid;
    update_hi_1_predicate: condition_lo;
    update_hi_1_value: 1;
    update_hi_2_predicate: not condition_lo;
    update_hi_2_value: 0;

    output_value : alu_hi;
    output_dst : md.matched_1;
}

action layer_1A() {
    flowid_alu_1.execute_stateful_alu_from_hash(dleft_hash_1);
}

@pragma stage 1
table layer_1A_table {
    actions { layer_1A; }
    size : 1;
}

/******************************************************************************
 *
 * layer1 B layer2 A
 *
 *****************************************************************************/
//layer1 B

register timestamp_1{
    width : 32;
    instance_count : DLEFT1_LEN;
}

blackbox stateful_alu timestamp_alu_1 {
    reg: timestamp_1;

    condition_lo : md.matched_1 == 1;
    condition_hi : register_lo - md.timestamp + 10000 < 0;
    update_lo_1_predicate: condition_lo;
    update_lo_1_value: md.timestamp;
    update_hi_1_predicate: not condition_lo and condition_hi;
    update_hi_1_value: 0;
    update_hi_2_predicate: not condition_lo and not condition_hi;
    update_hi_2_value: register_lo;

    output_value : alu_hi;
    output_dst : md.tmptimestamp_1;
}

action layer_1B() {
    timestamp_alu_1.execute_stateful_alu_from_hash(dleft_hash_1);
}

@pragma stage 2
table layer_1B_table {
    actions { layer_1B; }
    size : 1;
}


//layer2 A

field_list_calculation dleft_hash_2 {
    input { flow_id_list; }
    algorithm: crc32;
    output_width: DLEFT2_HASH_LEN;
}

register flowid_2 {
    width : 32;
    instance_count: DLEFT2_LEN;
}

blackbox stateful_alu flowid_alu_2 {
    reg: flowid_2;

    condition_lo: register_lo == md.flowid;
    update_hi_1_predicate: condition_lo;
    update_hi_1_value: 1;
    update_hi_2_predicate: not condition_lo;
    update_hi_2_value: 0;

    output_value : alu_hi;
    output_dst : md.matched_2;
}

action layer_2A() {
    flowid_alu_2.execute_stateful_alu_from_hash(dleft_hash_2);
}

@pragma stage 2
table layer_2A_table {
    actions { layer_2A; }
    size : 1;
}

/******************************************************************************
 *
 * layer1 C layer2 B layer3 A
 *
 *****************************************************************************/
//layer1 C

action layer_1C(){
    modify_field(md.minlayer, 1);
}

@pragma stage 3
table layer_1C_table {
    actions { layer_1C; }
    size : 1;
}

//layer2 B

register timestamp_2{
    width : 32;
    instance_count : DLEFT2_LEN;
}

blackbox stateful_alu timestamp_alu_2 {
    reg: timestamp_2;

    condition_lo : md.matched_2 == 1;
    condition_hi : register_lo - md.timestamp + 10000 < 0;
    update_lo_1_predicate: condition_lo;
    update_lo_1_value: md.timestamp;
    update_hi_1_predicate: not condition_lo and condition_hi;
    update_hi_1_value: 0;
    update_hi_2_predicate: not condition_lo and not condition_hi;
    update_hi_2_value: register_lo;

    output_value : alu_hi;
    output_dst : md.tmptimestamp_2;
}

action layer_2B() {
    timestamp_alu_2.execute_stateful_alu_from_hash(dleft_hash_2);
}

@pragma stage 3
table layer_2B_table {
    actions { layer_2B; }
    size : 1;
}


//layer3 A

field_list_calculation winner_hash_3 {
    input { flow_id_list; }
    algorithm: crc32;
    output_width: WINNER3_HASH_LEN;
}

register flowid_3 {
    width : 32;
    instance_count: WINNER3_LEN;
}

blackbox stateful_alu flowid_alu_3 {
    reg: flowid_3;

    condition_lo: register_lo == md.flowid;
    update_hi_1_predicate: condition_lo;
    update_hi_1_value: 1;
    update_hi_2_predicate: not condition_lo;
    update_hi_2_value: 0;

    output_value : alu_hi;
    output_dst : md.matched_3;
}

action layer_3A() {
    flowid_alu_3.execute_stateful_alu_from_hash(winner_hash_3);
}

@pragma stage 3
table layer_3A_table {
    actions { layer_3A; }
    size : 1;
}

/******************************************************************************
 *
 * layer2 C layer3 B layer4 A
 *
 *****************************************************************************/
//layer2 C

action layer_2C(){
    modify_field(md.minlayer, 2);
}

@pragma stage 4
table layer_2C_table {
    actions { layer_2C; }
    size : 1;
}

//layer3 B

register timestamp_3{
    width : 32;
    instance_count : WINNER3_LEN;
}

blackbox stateful_alu timestamp_alu_3 {
    reg: timestamp_3;

    condition_lo : md.matched_3 == 1;
    update_lo_1_predicate: condition_lo;
    update_lo_1_value: md.timestamp;
    update_hi_1_predicate: not condition_lo;
    update_hi_1_value: register_lo;

    output_value : alu_hi;
    output_dst : md.min;
}

action layer_3B() {
    timestamp_alu_3.execute_stateful_alu_from_hash(winner_hash_3);
}

@pragma stage 4
table layer_3B_table {
    actions { layer_3B; }
    size : 1;
}


//layer4 A

field_list_calculation winner_hash_4 {
    input { flow_id_list; }
    algorithm: crc32;
    output_width: WINNER4_HASH_LEN;
}

register flowid_4 {
    width : 32;
    instance_count: WINNER4_LEN;
}

blackbox stateful_alu flowid_alu_4 {
    reg: flowid_4;

    condition_lo: register_lo == md.flowid;
    update_hi_1_predicate: condition_lo;
    update_hi_1_value: 1;
    update_hi_2_predicate: not condition_lo;
    update_hi_2_value: 0;

    output_value : alu_hi;
    output_dst : md.matched_4;
}

action layer_4A() {
    flowid_alu_4.execute_stateful_alu_from_hash(winner_hash_4);
}

@pragma stage 4
table layer_4A_table {
    actions { layer_4A; }
    size : 1;
}

/******************************************************************************
 *
 * layer3 C layer4 B
 *
 *****************************************************************************/
 //layer3 C

action layer_3C(){
    modify_field(md.minlayer, 3);
}

@pragma stage 5
table layer_3C_table {
    actions { layer_3C; }
    size : 1;
}

//layer4 B

register timestamp_4{
    width : 32;
    instance_count : WINNER4_LEN;
}

blackbox stateful_alu timestamp_alu_4_1 {
    reg: timestamp_4;

    update_lo_1_value: md.timestamp;

}

blackbox stateful_alu timestamp_alu_4_2 {
    reg: timestamp_4;

    condition_lo: md.min - register_lo > 0;
    update_hi_1_predicate: condition_lo;
    update_hi_1_value: register_lo;
    update_hi_2_predicate: not condition_lo;
    update_hi_2_value: md.min;

    output_value : alu_hi;
    output_dst : md.min2;
}
action layer_4B_1() {
    timestamp_alu_4_1.execute_stateful_alu_from_hash(winner_hash_4);
}

action layer_4B_2() {
    timestamp_alu_4_2.execute_stateful_alu_from_hash(winner_hash_4);
}

@pragma stage 5
table layer_4B_table_1 {
    actions { layer_4B_1; }
    size : 1;
}

@pragma stage 5
table layer_4B_table_2 {
    actions { layer_4B_2; }
    size : 1;
}

/******************************************************************************
 *
 * layer4 C
 *
 *****************************************************************************/
 //layer4 C

action layer_4C(){
    modify_field(md.minlayer, 4);
    modify_field(md.min, md.min2);
}

@pragma stage 6
table layer_4C_table {
    actions { layer_4C; }
    size : 1;
}

/******************************************************************************
 *
 * recirculate
 *
 *****************************************************************************/
field_list resubmit_fields {
    //md.flowid;
    //md.timestamp;
    md.minlayer;
}

action resubmit_with_fields() {
    resubmit(resubmit_fields);
}

table resubmit_table {
    actions {
        resubmit_with_fields;
    }
    size : 1;
}


/******************************************************************************
 *
 * Recirculated Package
 *
 *****************************************************************************/
blackbox stateful_alu reget_time{
    reg : current_timestamp;

    update_lo_1_value : register_lo;

    output_value : alu_lo;
    output_dst : md.timestamp;
}

action reinitialize(){
    reget_time.execute_stateful_alu(0);
    modify_field(md.flowid, ipv4.sip);
}

table reinitialize_table{
    actions{
        reinitialize;
    }
    size : 1;
}

/******************************************************************************
 *
 * Reinsert 1
 *
 *****************************************************************************/
blackbox stateful_alu flowid_ins_1 {
    reg: flowid_1;

    update_lo_1_value: md.flowid;

}

blackbox stateful_alu timestamp_ins_1 {
    reg: timestamp_1;

    update_lo_1_value: md.timestamp;

}

action insert_flowid_1(){
    flowid_ins_1.execute_stateful_alu_from_hash(dleft_hash_1);
}

action insert_timestamp_1(){
    timestamp_ins_1.execute_stateful_alu_from_hash(dleft_hash_1);
}

table insert_flowid_table_1{
    actions{
        insert_flowid_1;
    }
    size : 1;
}

@pragma stage 2
table insert_timestamp_table_1{
    actions{
        insert_timestamp_1;
    }
    size : 1;
}

/******************************************************************************
 *
 * Reinsert 2
 *
 *****************************************************************************/
blackbox stateful_alu flowid_ins_2 {
    reg: flowid_2;

    update_lo_1_value: md.flowid;

}

blackbox stateful_alu timestamp_ins_2 {
    reg: timestamp_2;

    update_lo_1_value: md.timestamp;

}

action insert_flowid_2(){
    flowid_ins_2.execute_stateful_alu_from_hash(dleft_hash_2);
}

action insert_timestamp_2(){
    timestamp_ins_2.execute_stateful_alu_from_hash(dleft_hash_2);
}

table insert_flowid_table_2{
    actions{
        insert_flowid_2;
    }
    size : 1;
}

@pragma stage 3
table insert_timestamp_table_2{
    actions{
        insert_timestamp_2;
    }
    size : 1;
}

/******************************************************************************
 *
 * Reinsert 3
 *
 *****************************************************************************/
blackbox stateful_alu flowid_ins_3 {
    reg: flowid_3;

    update_lo_1_value: md.flowid;

}

blackbox stateful_alu timestamp_ins_3 {
    reg: timestamp_3;

    update_lo_1_value: md.timestamp;

}

action insert_flowid_3(){
    flowid_ins_3.execute_stateful_alu_from_hash(winner_hash_3);
}

action insert_timestamp_3(){
    timestamp_ins_3.execute_stateful_alu_from_hash(winner_hash_3);
}

table insert_flowid_table_3{
    actions{
        insert_flowid_3;
    }
    size : 1;
}

@pragma stage 4
table insert_timestamp_table_3{
    actions{
        insert_timestamp_3;
    }
    size : 1;
}

/******************************************************************************
 *
 * Reinsert 4
 *
 *****************************************************************************/
blackbox stateful_alu flowid_ins_4 {
    reg: flowid_4;

    update_lo_1_value: md.flowid;

}

blackbox stateful_alu timestamp_ins_4 {
    reg: timestamp_4;

    update_lo_1_value: md.timestamp;

}

action insert_flowid_4(){
    flowid_ins_4.execute_stateful_alu_from_hash(winner_hash_4);
}

action insert_timestamp_4(){
    timestamp_ins_4.execute_stateful_alu_from_hash(winner_hash_4);
}

table insert_flowid_table_4{
    actions{
        insert_flowid_4;
    }
    size : 1;
}

@pragma stage 5
table insert_timestamp_table_4{
    actions{
        insert_timestamp_4;
    }
    size : 1;
}

/******************************************************************************
 *
 * Ingress & Egress
 *
 *****************************************************************************/

control ingress {
    apply(ipv4_routing);
    //regular packet
    if(ig_intr_md.resubmit_flag==0){
        //stage 0
        apply(InitializeTable);
        //stage 1
        apply(layer_1A_table);
        //stage 2
        apply(layer_1B_table);
        apply(layer_2A_table);
        //stage 3
        if(md.matched_1 == 0 and md.tmptimestamp_1 == 0){
            apply(layer_1C_table);
        }
        apply(layer_2B_table);
        apply(layer_3A_table);
        //stage 4
        if(md.minlayer == 0 and md.matched_2 == 0 and md.tmptimestamp_2 == 0){
            apply(layer_2C_table);
        }
        apply(layer_3B_table);
        apply(layer_4A_table);
        //stage 5
        if(md.minlayer == 0 and md.matched_3 == 0){
            apply(layer_3C_table);
        }
        if(md.matched_4 == 1){
            apply(layer_4B_table_1);
        }
        else{
            apply(layer_4B_table_2);
        }
        //stage 6
        if(md.matched_4 == 0 and md.minlayer == 3){
            if(md.min != md.min2){
                apply(layer_4C_table);
            }
        } 
        //stage 7
        if(md.minlayer != 0){
            apply(resubmit_table);
        }
    }
    //recirculate packet
    else{
        apply(reinitialize_table);
        if(md.minlayer == 1){
            apply(insert_flowid_table_1);
            apply(insert_timestamp_table_1);
        }
        if(md.minlayer == 2){
            apply(insert_flowid_table_2);
            apply(insert_timestamp_table_2);
        }
        if(md.minlayer == 3){
            apply(insert_flowid_table_3);
            apply(insert_timestamp_table_3);
        }
        if(md.minlayer == 4){
            apply(insert_flowid_table_4);
            apply(insert_timestamp_table_4);
        }
    }
}

control egress {

}