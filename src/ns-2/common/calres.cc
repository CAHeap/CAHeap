#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/export.hpp>
#include "unordered_map_serialization.h"
#include "./ALGO/srcAHeap/FlowCache.h"
#include "./ALGO/srcAHeap/Util.h"
#include "./ALGO/common/metric.h"
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>

#include <sys/socket.h>   
#include <netinet/in.h>   
#include <arpa/inet.h>  

using stl_serialization::MapSerialization;


// #define ONE 128
// #define TWO 160
// #define THREE 192
int core[] = {204,205,206,207};
int TIMEPOINT;

unordered_map<string, int > getFLowCount(int id)
{
    std::string path = ""; // True File Path 
    path += to_string(id);
     path += to_string(TIMEPOINT);
    std::ifstream is(path);
    MapSerialization<std::string, int> d2;
    boost::archive::text_iarchive ia(is);  
    ia >> d2; 
    unordered_map<string, int > flowCount;

    for(auto it = d2.begin();it!=d2.end();++it)
    {
        flowCount[it->first] = it->second;
    }
    return flowCount;
}


void getFlowCache(FlowCache & dst, int id)
{
    std::string fpath = ""; // Estimate File Path 
    fpath += to_string(id);
    fpath += to_string(TIMEPOINT);



    std::ifstream fis(fpath);  

   // FlowCache flowcache(1000, 21, -1);  
    boost::archive::text_iarchive fia(fis);  
    fia >> dst; 
    return;
}


/*
    g++ calres.cc -std=c++11 -L/home/zcxy/Desktop/boost_1_69_0/stage/lib -lboost_serialization -I /home/zcxy/Desktop/boost_1_69_0 -lisal
*/

void printfKEY(string key)
{
            int sip,dip;
            uint16_t sp,dp;
            uint8_t t;
            memcpy(&sip,key.c_str(),4);
            memcpy(&sp,key.c_str()+4,2);
            memcpy(&dip,key.c_str()+6,4);
            memcpy(&dp,key.c_str()+10,2);
            memcpy(&t,key.c_str()+12,1);


            char src[15];
	        inet_ntop(AF_INET,&sip, src, sizeof(src)); 
	        char dst[15];
	        inet_ntop(AF_INET,&dip, dst, sizeof(dst));
            std::cout<<src<<" "<<sp<<" "<<dst<<" "<<dp<<" "<<int(t)<<std::endl;
}
string  getdst(string key)
{
     int sip,dip;
            uint16_t sp,dp;
            uint8_t t;
            memcpy(&sip,key.c_str(),4);
            memcpy(&sp,key.c_str()+4,2);
            memcpy(&dip,key.c_str()+6,4);
            memcpy(&dp,key.c_str()+10,2);
            memcpy(&t,key.c_str()+12,1);


        char src[15];
	    inet_ntop(AF_INET,&sip, src, sizeof(src)); 
	    char dst[15];
	    inet_ntop(AF_INET,&dip, dst, sizeof(dst));
        return string(dst);

}
vector<string> intersect(vector<unordered_map<string, int > > flowcounts)
{
    unordered_map<string, int > resmap;
    vector<string> res;
    if(flowcounts.size()==0)
        return res;
    resmap = flowcounts[0];
    for(int i = 1 ;i<flowcounts.size();++i)
    {
        unordered_map<string, int > tmp;
        for( auto & v: flowcounts[i])
         {
             if( resmap.find(v.first)!=resmap.end())
             {
                tmp[v.first] = 1;
             }
         }
        resmap = tmp;
    }
    for( auto & v: resmap)
    {
        res.push_back(v.first);
    }
    return res;
}

double sta_dev(vector<double> num)
{
    double avr =  0;
    for(auto&v:num)
        avr += v;
    avr /= num.size();
    double res = 0;
    for(auto&v:num)
        res += v*v;  
    return avr*avr/(res/num.size());
}
bool judge(string str, vector<string> dst)
{
    for(auto &v:dst)
    {
        if(str==v)
        return true;
    }
    return false;
}
int main(int    argc,    char    *argv[] )
{  

/*###########################################*/
/*Blackhole*/
/*###########################################*/
int count = 0;
TIMEPOINT = atoi(argv[1]);
printf("TIME%d\n",TIMEPOINT);
for(int x= 0;x<32;++x)
{
    int ONE,TWO,THREE, FOUR;
    ONE = 128+x;
    TWO = 160+x;
    int max = 0;        
    int flag =0;
    
    /*Target*/
    string str1,str2,str3,str4;
    str1 = to_string(4*x+0)+string(".0.0.0");
    //str2 = to_string(4*x+1)+string(".0.0.0");
    //str3 = to_string(4*x+2)+string(".0.0.0");
    //str4 = to_string(4*x+3)+string(".0.0.0");
    vector<string> dst;
    dst.push_back(str1);
    //dst.push_back(str2);
    //dst.push_back(str3);
    //dst.push_back(str4);

    for(int y = 0;y<4;++y)
    {
        THREE = 192 + (x%4)*4 + y; 
        printf("ONE%d-",ONE);
        printf("TWO%d-",TWO);
        printf("THREE%d\n",THREE); 
        FlowCache flowcache1;getFlowCache(flowcache1,ONE);
        FlowCache flowcache2;getFlowCache(flowcache2,TWO);
        FlowCache flowcache3;getFlowCache(flowcache3,THREE);

        vector<int> parasVec;
	    parasVec.push_back(3);	
        parasVec.push_back(128);	
        parasVec.push_back(13);	
	    parasVec.push_back(15);	
        parasVec.push_back(4);

        vector<FlowCache> fcVec;
	    fcVec.push_back(flowcache1);
        fcVec.push_back(flowcache2);
        fcVec.push_back(flowcache3);
        vector<pair<unordered_map<string, vector<MyItem> >, int> > AllRes;
        DecodeCaches(AllRes, fcVec, parasVec);

        for(auto & v:AllRes[1].first)
        {
            if(judge(getdst(v.first),dst))
            {
                int a,b;
                a=b=0;
                double aTime = 0;
                for(auto&n :v.second)
                {
                    aTime =n.my_value_time>aTime?n.my_value_time: aTime;
                    a+= n.my_value_count;
                }
                aTime = aTime /1000000;
                for(auto&n :AllRes[0].first[v.first])
                {
                    b+= n.my_value_count;
                    max =n.my_value_time>max?n.my_value_time: max;
                }
                if( double(a-b)/a>=0.3 && aTime >= 0.120){
                    std::cout<<a<<" "<<b<<" Before Blackchole"<<aTime<<endl;
                    flag = 1;
                    printfKEY(v.first);
                }
            }
        }
    }
    count+= flag;
    std::cout<< "Loss: "<<count<<endl;
    std::cout<< "Total: "<<x+1<<endl;
    std::cout<< "After Blackhole Time: "<<double(max)/1000000<<endl;
}

/*###########################################*/
/*Loop*/
/*###########################################*/
    //  int res = 0;
    // int tres = 0;
    // for(int x= 0;x<32;++x)
    // {
    //     int ONE,TWO,THREE;
    //     ONE =128+x;
    //     TWO = 160+x;
    //     THREE = 192 + (4*x)%16; 
    //     int tflag = 0;
    //     int mflag =0;
    // for(int y = 0;y<4;++y)
    // {
    //     THREE = 192 + (4*x)%16+y; 
    // unordered_map<string, int > flowCount1 = getFLowCount(ONE);
    // unordered_map<string, int > flowCount2 = getFLowCount(TWO);
    // unordered_map<string, int > flowCount3 = getFLowCount(THREE);
    // vector<unordered_map<string, int >> flowcounts;
    // flowcounts.push_back(flowCount1);
    // flowcounts.push_back(flowCount2);
    // flowcounts.push_back(flowCount3);
    // string str= to_string(x*4)+".0.0.0";
    // for(auto&v:intersect(flowcounts))
    // {   if(getdst(v)==str)
    //     {
    //         if(flowCount2[v]>flowCount3[v]){
    //         std::cout<<flowCount1[v]<<endl;
    //          std::cout<<flowCount2[v]<<endl;
    //          std::cout<<flowCount3[v]<<endl;
    //         printfKEY(v);
    //         tflag =1;
    //         break;
    //         }
    //     }
    // }

    // FlowCache flowcache1;getFlowCache(flowcache1,ONE);
    // FlowCache flowcache2;getFlowCache(flowcache2,TWO);
    // FlowCache flowcache3;getFlowCache(flowcache3,THREE);
    // vector<int> parasVec;
	// parasVec.push_back(3);	
    // parasVec.push_back(128);	
    // parasVec.push_back(13);	
	// parasVec.push_back(15);	
    // parasVec.push_back(4);
    // vector<FlowCache> fcVec;
	// fcVec.push_back(flowcache1);
    // fcVec.push_back(flowcache2);
    // fcVec.push_back(flowcache3);
    // vector<pair<unordered_map<string, vector<MyItem> >, int> > AllRes;
    // DecodeCaches(AllRes, fcVec, parasVec);
    // for(auto & v:AllRes[0].first)
    // {
    //     int a,b,c;
    //     a = b = c = 0;
    //     if(getdst(v.first)==str)
    //     {
    //         for(auto&m:v.second)
    //             a+= m.my_value_count;
    //         for(auto&m:AllRes[1].first[v.first])
    //             b+= m.my_value_count;
    //         for(auto&m:AllRes[2].first[v.first])
    //             c+= m.my_value_count;
    //         if(b>c)
    //          {
    //             std::cout<<a<<" "<<AllRes[0].second<<endl;
    //             std::cout<<b<<" "<<AllRes[1].second<<endl;
    //             std::cout<<c<<" "<<AllRes[2].second<<endl;
    //             printfKEY(v.first); 
    //             mflag = 1;
    //             break;
    //         }
    //     }
    // }
    // }
    // res += mflag;
    // tres += tflag;
    // std::cout<<tres<<"//"<<x+1<<endl;
    //  std::cout<<res<<"//"<<x+1<<endl;
    // }

/*###########################################*/
/*ECMP Load Imbalance*/
/*###########################################*/
    // unordered_map<string, int > flowCount1 = getFLowCount(ONE);
    // unordered_map<string, int > flowCount2 = getFLowCount(TWO);
    // // unordered_map<string, int > flowCount3 = getFLowCount(THREE);
    // // unordered_map<string, int > flowCount4 = getFLowCount(FOUR);
    // // unordered_map<string, int > flowCount5 = getFLowCount(FIVE);
    // // unordered_map<string, int > flowCount6 = getFLowCount(SIX);
    // vector<double> truenum;
    // for(int i = 0; i < 4; ++i)
    // {
    //     vector<unordered_map<string, int >> flowcounts;
    //     unordered_map<string, int > flowCount3 = getFLowCount(core[i]);
    //     flowcounts.push_back(flowCount1);
    //     flowcounts.push_back(flowCount2);
    //     flowcounts.push_back(flowCount3);
    //     double  tmp  = 0;
    //     for(auto & v :intersect(flowcounts))
    //          tmp+= flowCount3[v];
    //     //tmp  = intersect(flowcounts).size();
    //     std::cout<<tmp<<endl;
    //     std::cout<<"Core Pkt Num:" << intersect(flowcounts).size()<<endl;
    //     truenum.push_back(tmp);
    // }
    
    // FlowCache flowcache1;
    // getFlowCache(flowcache1,ONE);  
    // flowcache1.set_keylen(5);
 

    
    // FlowCache flowcache2;
    // getFlowCache(flowcache2, TWO); 
    // flowcache1.set_keylen(5);


    // vector<double> estnum;
    // for(int i = 0; i < 4; ++i)
    // {
    //     FlowCache flowcache3;
    //     getFlowCache(flowcache3,core[i]);  
    //     flowcache1.set_keylen(5);
    //     vector<int> parasVec;
	//     parasVec.push_back(3);	
    //     parasVec.push_back(128);	
    //     parasVec.push_back(13);	
	//     parasVec.push_back(15);	
    //     parasVec.push_back(4);
    //     vector<FlowCache> fcVec;
	//     fcVec.push_back(flowcache1);
    //     fcVec.push_back(flowcache2);
    //     fcVec.push_back(flowcache3);
    //     vector<pair<unordered_map<string, vector<MyItem> >, int> > AllRes;
    //     DecodeCaches(AllRes, fcVec, parasVec); 
    //     double tmp = 0;
    //     for(auto & v:AllRes[2].first)   
    //     {
    //         vector<MyItem> vec = v.second;
    //         for(int j = 0; j < vec.size(); ++j)
	//   		    tmp += vec[j].my_value_count;
    //     }
    //    // tmp = AllRes[2].first.size();
    //     std::cout<<tmp<<endl;
    //     estnum.push_back(tmp);
    // }
    // std::cout<<"True"<<sta_dev(truenum)<<endl;
    // std::cout<<"EST"<<sta_dev(estnum)<<endl;




/*###########################################*/
/*Packet Loss*/
/*###########################################*/
    // //k + p + len +full_len+layer

	// vector<int> parasVec;
	// parasVec.push_back(3);	
    // parasVec.push_back(128);	
    // parasVec.push_back(13);	
	// parasVec.push_back(15);	
    // parasVec.push_back(4);


	// vector<FlowCache> fcVec;
	// fcVec.push_back(flowcache1);
    // fcVec.push_back(flowcache2);
    // fcVec.push_back(flowcache3);

 
	// vector<pair<unordered_map<string, vector<MyItem> >, int> > AllRes;
    // DecodeCaches(AllRes, fcVec, parasVec); 

	// unordered_map<string, vector<MyItem> > sum1 = AllRes[0].first;
    // unordered_map<string, vector<MyItem> > sum2 = AllRes[1].first;

    
    // int dif[10000];
    // memset(dif,0,sizeof(dif));
    // int dif_esti[10000];
    // memset(dif_esti,0,sizeof(dif_esti));

    // int max = 0;
    // int c1 =0;
    // int c2 =0;
    // int countsem = 0;

    // int count  =0;
    // int total = 0;
    // vector<double> trueRate;
    // vector<double> estRate;
    // int vmax;

    // for(auto &v: intersect(flowcounts))
    // {
    //     int tmp = flowCount1[v] - flowCount2[v];
    //     vmax = flowCount1[v] >flowCount2[v]?flowCount1[v]:flowCount2[v];
    //     tmp = tmp<0?-tmp:tmp;
    //     if(tmp){
    //         count++;
    //         trueRate.push_back(double(tmp)/vmax);
           
    //         if(tmp>10)
    //         {
    //          std::cout <<tmp <<" "<< flowCount1[v] << " " << flowCount2[v]<<endl;
    //          printfKEY(v);
    //         }
    //     }
        
    //     max = tmp>max?tmp:max;
    //     c1++;
    //     dif[tmp]++;
    // }
 
    // printf("Same:%d Loss:%d %d\n",intersect(flowcounts).size(),count,max);
    // count  = 0 ;
    // max = 0;
    // for(auto it = sum1.begin(); it != sum1.end(); ++it)
    // {
    //     if(sum2.find(it->first)!=sum2.end())
    //     {
    //         vector<MyItem> vec = sum1[it->first];
    //         vector<MyItem> vec2 = sum2[it->first];
    //         int res1 =0;
    //         int res2 = 0;
	//  		for(int i = 0; i < vec.size(); ++i)
	//  			res1 += vec[i].my_value_count;
    //         for(int i = 0; i < vec2.size(); ++i)
	//  			res2 += vec2[i].my_value_count;   
    //         int tmp = res1 - res2;
    //         vmax = res1 > res2?res1:res2;
    //         tmp = tmp<0?-tmp:tmp;
    //         if(tmp)
    //         {
    //             count++;
    //             estRate.push_back(double(tmp)/vmax);
    //         }
    //         // if(tmp>2)
    //         // {   count++;
    //         //     std::cout <<tmp <<" "<< res1 <<" "<<res2 <<" "<<vec[0].my_value_time<<endl;
    //         //     estRate.push_back(double(tmp)/vmax);
    //         //     printfKEY(it->first);
    //         //     std::cout <<tmp <<" "<< flowCount1[it->first] << " "<<flowCount2[it->first]<<endl;

    //         // }
    //         max = tmp>max?tmp:max;
    //         dif_esti[tmp]++;
    //         c2++;
    //     }
    // }
    // printf("Flowcache Same %d Loss:%d %d\n",sum1.size(),count,max);
    // std::string fpath ="/home/zcxy/Desktop/ECres/DIF";
    // std::ofstream fout(fpath); 
    // int cdf1 = 0;
    // int cdf2 = 0;

    // sort(trueRate.begin(),trueRate.end());
    // sort(estRate.begin(),estRate.end());
    // for(int i=0;i<5;++i)
    // {
    //     printf("%.3f\n",trueRate[trueRate.size()/5 * i]);
    // }
    // for(int i=0;i<5;++i)
    // {
    //     printf("%.3f\n",estRate[estRate.size()/5 * i]);
    // }

    // for(int i =0;i<=max;++i)
    // {
    //     fout<<i<<","<<double(cdf1+dif[i])/c1<<","<<double(cdf2+dif_esti[i])/c2<<endl;
    //     cdf1+=dif[i];
    //     cdf2+=dif_esti[i];
    // }
    return 0;
}



