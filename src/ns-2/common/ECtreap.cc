#include "ECtreap.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include <sys/socket.h>   
#include <netinet/in.h>   
#include <arpa/inet.h>  
#include "packet.h"
#include "ip.h" 

static class ECtreapClass : public TclClass {
public:
	ECtreapClass() : TclClass("Connector/ECtreap") {}
	TclObject* create(int /* argc */, const char*const* /* argv */) {
		return (new ECtreap);
	}
	
} class_ECtreap;
ECtreap::ECtreap() 
{
	bind("tid_",&nid); 
	string file_path = RESULT_PATH;
	char str[25];
    sprintf(str, "%d", nid);
	file_path += string("/");
	file_path += string(str);
	fout.open(file_path.c_str());
	for(int i =0 ;i< TESTPOINT;++i)
		testPoint[i] = 0.132 + 0.002*i;

	testPoint[TESTPOINT] = 100;
	curTestPoint = 0; 
	flowcache.set_keylen(5);
	flowcache.setID(nid%128);
}


void ECtreap::recv(Packet* p, Handler* h)
{
	count++;
	/*Get the Flow ID 5-tuple*/
	char key[FLOWIDLEN]; /*src-IP src-PORT dst-IP dst-PORT Proto*/
	hdr_ip * iph = HDR_IP(p);
	hdr_cmn * cmnh = HDR_CMN(p);
	double now = Scheduler::instance().clock();
	int timeStamp = now * 1000000; // second->us
	uint32_t srcIP= iph->saddr();
	uint16_t srcPort= (uint16_t)iph->sport();
	uint32_t dstIP= iph->daddr();
	uint16_t dstPort= (uint16_t)iph->dport();
	uint8_t  proto = (uint8_t)cmnh->ptype();
	memcpy(key,&srcIP,4);
	memcpy(key+4,&srcPort,2);
	memcpy(key+6,&dstIP,4);
	memcpy(key+10,&dstPort,2);
	memcpy(key+12,&proto,1);

	/*Insert RS-AHeap*/
	int s_id = nid % 128;
	int k = 3;
	int parity = 128;
	int full_len = 15;
	FlowCacheInsert(flowcache, s_id, key, 13, 3, parity, timeStamp, 15);
	
	/*Insert TureLoss*/
	string str_key = string(key,FLOWIDLEN);
	if(TrueLoss.find(str_key)==TrueLoss.end())
		TrueLoss[str_key] = 0;
	TrueLoss[str_key] += 1;

	/*Seralize Data Structure*/
	if(now > testPoint[curTestPoint])
	{
  		printf("%d test ok %f \n",nid,testPoint[curTestPoint]);
		//flag[curTestPoint] = 0;
		string fpath = FLOWCACHE_RES;
		char str[25];
    	sprintf(str, "%d", nid);
		fpath += string(str);

		char str2[25];
    	sprintf(str2, "%d", curTestPoint);
		fpath += string(str2);


		ofstream fcache(fpath);
  		boost::archive::text_oarchive oa(fcache);
		oa << flowcache;
		fcache.close();

		string Tpath = TRUE_RES;

		Tpath += string(str);
		Tpath += string(str2);
		ofstream Tmap(Tpath);
  		boost::archive::text_oarchive oat(Tmap);
		oat << TrueLoss;
		Tmap.close();

		curTestPoint++;
	}  
	/*Log File*/
	char src[15];
	inet_ntop(AF_INET,&iph->saddr(), src, sizeof(src)); 
	char dst[15];
	inet_ntop(AF_INET,&iph->daddr(), dst, sizeof(dst));
	fout<<count<<" "<<timeStamp<<" "<<src<<" "<<iph->sport()<<" "<<dst<<" "<<iph->dport()<<" "<<cmnh->ptype()<<std::endl;
	send(p, h);
}





