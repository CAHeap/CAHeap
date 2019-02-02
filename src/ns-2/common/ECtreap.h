	#ifndef ECTREAP_H
	#define ECTREAP_H


	#include <fstream>

	
	#include <boost/archive/text_oarchive.hpp>
	#include <boost/archive/text_iarchive.hpp>
	#include <boost/archive/binary_oarchive.hpp>
	#include <boost/archive/binary_iarchive.hpp>
	#include <boost/serialization/split_member.hpp>
	#include <boost/serialization/export.hpp>
	#include "unordered_map_serialization.h"
	#include <unordered_map>


	#include "connector.h"
	#include "./ALGO/srcAHeap/FlowCache.h"
	#include "./ALGO/srcAHeap/Util.h"
		
	#define RESULT_PATH ""
	#define FLOWCACHE_RES ""
	#define TRUE_RES ""

	using stl_serialization::MapSerialization;

	#define FLOWIDLEN 13
	#define TESTPOINT 6
	class ECtreap : public Connector{
	public:
		ECtreap();
		void store();
		//std::unordered_map<std::string,int> TrueLoss;
		MapSerialization<std::string,int>  TrueLoss;
		FlowCache flowcache = FlowCache(2800, 21, -1);
		int count = 0;
		//bool flag[TESTPOINT];
		int nid;
		int curTestPoint;
		double testPoint[TESTPOINT+1];
		std::ofstream fout;
		void recv(Packet* p, Handler*);
		inline void send(Packet* p, Handler* h) { target_->recv(p, h); }
	};

	#endif
