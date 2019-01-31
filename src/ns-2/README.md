# CAHeap ns-2 project
***
## NS-2 Add CAHeap at the Node-Entry
#### 1)At folder: ns-allinone-2.35/ns-2.35/common

add ECtreap.h

```
#ifndef ECTREAP_H
#define ECTREAP_H

#include "packet.h"
#include "ip.h"
#include "connector.h"

class ECtreap : public Connector {
 public:
	ECtreap();
	int count = 0;
	int nid;
	void recv(Packet* p, Handler*);
	inline void send(Packet* p, Handler* h) { target_->recv(p, h); }
	int command(int argc, const char*const* argv);
};

#endif
```

add ECtreap.cc

```
#include "ECtreap.h"
#include <stdio.h>

static class ECtreapClass : public TclClass {
public:
	ECtreapClass() : TclClass("Connector/ECtreap") {}
	TclObject* create(int /* argc */, const char*const* /* argv */) {
		return (new ECtreap);
	}
} class_ECtreap;

ECtreap::ECtreap() {bind("tid_",&nid);}

int ECtreap::command(int argc, const char*const* argv)
{
	return Connector::command(argc, argv);
}

void ECtreap::recv(Packet* p, Handler* h)
{
    if(!count)
	printf("hello ECtreap this is %d\n",nid);
	count++;
	/*##################################
	
	Flow-Cache
	
	##################################*/
	
	send(p, h);
}
```

#### 2) At Makefile

```
OBJ_CC =....
		....
		common/ECtreap.o\
```

#### 3) ns-allinone-2.35/ns-2.35/tcl/lib/ns-default.tcl

```
Connector/ECtreap set tid_ 10086
```

#### 4) ns-node.tcl

Behind

![1547021912457](C:\Users\dkzcx\AppData\Roaming\Typora\typora-user-images\1547021912457.png)

Add

```
Connector/ECtreap set tid_ $id_
```

Behind

![1547021969428](C:\Users\dkzcx\AppData\Roaming\Typora\typora-user-images\1547021969428.png)

Add

```
$self insert-entry  "Base" [new Connector/ECtreap] "target"
```

#### 4) ns-node.tcl

Because EC-Treap is a Connector not a Classifier!!!

```
Line341: set nxtclsfr_ [$classifier_ target]
....
Line362: set mpathClsfr_($id) [new Classifier/MultiPath]
			if {$routes_($id) > 0} {
				assert "$routes_($id) == 1"
				$mpathClsfr_($id) installNext \
						[$nxtclsfr_ in-slot? $id]
			}
			$nxtclsfr_ install $id $mpathClsfr_($id)
```

## NS-2 & Boost Serialize

#### 1) Download & Install Boost

Google boost

Modify Makefile

```
INCLUDE= ...
		-I /home/zcxy/Desktop/boost_1_69_0 
LIB   = ...
		-L/home/zcxy/Desktop/boost_1_69_0/stage/lib -lboost_serialization\
```

#### 2) Add Files & Folder

Common/ALGO

Common/unordered_map_serialization.h

Common/ECtreap.h

Common/ECtreap.cc

#### 3) Add Result Process Files

Add Common/calres.cc & Modify File Path

#### 4) Modify FILE Path

```
ECtreap.h
	#define RESULT_PATH "/home/zcxy/Desktop/ECres"
	#define FLOWCACHE_RES "/home/zcxy/Desktop/ECres/ECtreap"
	#define TRUE_RES "/home/zcxy/Desktop/ECres/TRUE"
```

## Encode and Decode