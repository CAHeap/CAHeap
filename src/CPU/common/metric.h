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

struct FSDPerform{
	double wmre;	// Weighted Mean Relative Error
};

struct EntropyPerform{
	double real;
	double est;
	double re;
};

struct DDoSPerform{
	int real;
	int get;
	double recall;
	double precision;
};

struct PersistPerform
{
	double precision;
	double recall;
};

struct FlowletPerform
{
	double precision;
	double recall;
	double f1;
};

struct BurstPerform
{
	double precision;
	double recall;
	double f1;
};

#define NFRC(x) (x*1.3 > 1 ? 1 : x*1.3)

#endif