all: testflowsize testheavychange testflowsizedistribution testentropy testcard testheavyhitter testddos testspreader testrecentflow testpersistent testflowlet

objs = testparam testflowsize testheavychange testflowsizedistribution testentropy testcard testheavyhitter testddos testspreader testrecentflow testpersistent testflowlet

HEADERS = ../common/BOBHash32.h ../common/IO.h ../common/util.h ../common/metric.h

AHeapTestHeader = ../tasks/AHeap_CountingTasks.h ../tasks/AHeap_KVTasks.h ../AHeap/AHeap.h ../AHeap/ExpBas.h ../AHeap/Flow.h ../AHeap/Util.h ../AHeap/CODEC.h ../AHeap/Burst.h

IBLTTestHeader = ../tasks/FlowRadar_CountingTasks.h  ../tasks/FlowRadar_KVTasks.h ../FlowRadar/insertable_iblt.h

isal = /usr/lib/libisal.a

sourceDir = ../tests

testparam: ${sourceDir}/testparam.cpp  $(HEADERS) $(AHeapTestHeader)
	g++ -std=c++11 ${sourceDir}/testparam.cpp -o testparam $(isal)

testflowsize: ${sourceDir}/testflowsize.cpp $(HEADERS) $(AHeapTestHeader) $(IBLTTestHeader)
	g++ -std=c++11 ${sourceDir}/testflowsize.cpp -o testflowsize $(isal)

testheavychange: ${sourceDir}/testheavychange.cpp $(HEADERS) $(AHeapTestHeader) $(IBLTTestHeader)
	g++ -std=c++11 ${sourceDir}/testheavychange.cpp -o testheavychange $(isal)

testflowsizedistribution: ${sourceDir}/testflowsizedistribution.cpp $(HEADERS) $(AHeapTestHeader) $(IBLTTestHeader)
	g++ -std=c++11 ${sourceDir}/testflowsizedistribution.cpp -o testflowsizedistribution $(isal)

testentropy: ${sourceDir}/testentropy.cpp $(HEADERS) $(AHeapTestHeader) $(IBLTTestHeader)
	g++ -std=c++11 ${sourceDir}/testentropy.cpp -o testentropy $(isal)

testcard: ${sourceDir}/testcard.cpp $(HEADERS) $(AHeapTestHeader) $(IBLTTestHeader)
	g++ -std=c++11 ${sourceDir}/testcard.cpp -o testcard $(isal)

testheavyhitter: ${sourceDir}/testheavyhitter.cpp $(HEADERS) $(AHeapTestHeader) $(IBLTTestHeader)
	g++ -std=c++11 ${sourceDir}/testheavyhitter.cpp -o testheavyhitter $(isal)

testddos: ${sourceDir}/testddos.cpp $(HEADERS) $(AHeapTestHeader) $(IBLTTestHeader)
	g++ -std=c++11 ${sourceDir}/testddos.cpp -o testddos $(isal)

testspreader: ${sourceDir}/testspreader.cpp $(HEADERS) $(AHeapTestHeader) $(IBLTTestHeader)
	g++ -std=c++11 ${sourceDir}/testspreader.cpp -o testspreader $(isal)

testrecentflow: ${sourceDir}/testrecentflow.cpp $(HEADERS) $(AHeapTestHeader) $(IBLTTestHeader)
	g++ -std=c++11 ${sourceDir}/testrecentflow.cpp -o testrecentflow $(isal)

testpersistent: ${sourceDir}/testpersistent.cpp $(HEADERS) $(AHeapTestHeader) $(IBLTTestHeader)
	g++ -std=c++11 ${sourceDir}/testpersistent.cpp -o testpersistent $(isal)

testflowlet: ${sourceDir}/testflowlet.cpp $(HEADERS) $(AHeapTestHeader) $(IBLTTestHeader)
	g++ -std=c++11 ${sourceDir}/testflowlet.cpp -o testflowlet $(isal)

testburst: ${sourceDir}/testburst.cpp $(HEADERS) $(AHeapTestHeader) $(IBLTTestHeader)
	g++ -std=c++11 ${sourceDir}/testburst.cpp -o testburst $(isal)
testspeed: ${sourceDir}/cpu_throughput.cpp $(HEADERS) $(AHeapTestHeader) $(IBLTTestHeader)
	g++ -O3 -std=c++11  ${sourceDir}/cpu_throughput.cpp -o testspeed $(isal)


.PHONY: clean
clean:
	-rm -f *.out $(objs)
