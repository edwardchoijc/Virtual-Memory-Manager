#ifndef MEMORY_H
#define MEMORY_H

#include <string>

#include "Const.h"

using namespace std;

class PageTable
{
public:
	//Variables
	static int proc_frame[NUM_PageFrame];//标记被哪个进程占用
	static int pos_LRU;//标记LRU算法中最先被置换的page frame
	int flag_PTfirst[SIZE_PTfirst];//标记一级页表是否已载入
	ll **PT = new ll*[SIZE_PTfirst];//二级页表结构

	//Functions
	PageTable();
	ll findFrame(ll page);
	ll getFrame() const { return pageTOframe[1]; }
	//~PageTable();
private:
	int seekFrame;
	ll pageTOframe[2];//[0]是page，[1]是pageframe
};


class TLB
{
public:
	TLB(int s);
	TLB(TLB &temp);//复制构造函数
	ll searchEntry(ll page, int curtime);//由page找到对应entry的page frame
	void addEntry(ll page, ll frame, int curtime);
	int getSize() const { return size; }
	void refreshTLB();//在进程切换时， 需要清空 TLB 为新进程的运行做准备
	//void saveTLB(string name_file); //每次执行完毕出现进程切换时，将其页表内容保存到外部文件
private:
	ll entry_page[SIZE_TLB];//每个entry对应的page项
	ll entry_frame[SIZE_TLB];//每个entry对应的page frame项
	int time[SIZE_TLB];//每个entry访问时间
	int size;//TLB项数
};


class Process
{
public:
	//Variables
	static TLB tlb;
	PageTable pt;
	//Functions
	Process(string n, int mb);
	void action(string name_file);
	string getName() const { return name; }
	int getProcnum() const { return procnum; }
	int getSize() const { return size; }
	int getTimes_Run() const { return times_run; }
	void setTimes_Run() { times_run++; }
	int getTimes_Totalaccess() const { return times_totalaccess; }
	void setTimes_Totalaccess(int num) { times_totalaccess += num; }
	int getTimes_TLBmiss() const { return times_TLBmiss; }
	void setTimes_TLBmiss() { times_TLBmiss++; }
	int getTimes_Pagefault() const { return times_pagefault; }
	void setTimes_Pagefault() { times_pagefault++; }
	ll getHighAddr() const { return addr_high[0]; }//高位地址默认以0xFFFFFFFF结束
	ll getLowAddr() const { return addr_low[1]; }//地位地址默认以0x00000000开始
	void saveAccessAddr(int num, string name_file);
	void updatePT();
	void savePT(string name_file);
private:
	static int num_totalproc;//用来记录目前的总进程数，以便编号
	string name;
	int procnum;//当前进程序号
	int size;//单位MB
	int times_run;//记录程序运行次数
	int times_totalaccess;//记录总的访问地址次数
	int times_TLBmiss;//记录TLB miss的次数
	int times_pagefault;//记录page fault的次数
	ll addr_high[2];//含义为：地址范围由addr_high[0]到addr_high[1]
	ll addr_low[2];//形同上
	ll addr_access[RANDOMROUND*NUM_ADDR_PR];//记录50轮随机的访问的500个地址
	int proc_frame[NUM_PageFrame];//标记本进程使用page frame的情况,与page table中的相呼应
};

/*class System
{
public:

private:

};*/



#endif