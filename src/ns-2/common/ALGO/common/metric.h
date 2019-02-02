#ifndef _METRIC_H_
#define _METRIC_H_


struct FlowsizePerform{
	double accuracy;
	double aae;
	double are;
	double load_factor;
};

struct HeavyHitterPerform
{
	double recall;
	double precision;
};

struct HeavychangePerform{
	int real;
	int get;
	double recall;
	double precision;
};

#endif