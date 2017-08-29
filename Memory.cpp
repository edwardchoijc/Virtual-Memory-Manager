#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <cstdio>
#include <ctime>
#include <iomanip>

#include "Memory.h"
//#include "Const.h"

using namespace std;



/********************************************************************************************/
/************************************Class PageTable*****************************************/
/********************************************************************************************/

//initialize for static variables
int PageTable::proc_frame[NUM_PageFrame] = { UNUSED };
int PageTable::pos_LRU = 0;

PageTable::PageTable()//由于virtual address有32bits，所以默认虚页数为2^19=524288
{
	//initialize
	/*for (int i = 0; i < NUM_PageFrame; i++)
		proc_frame[i] = UNUSED;*/
	for (int i = 0; i < SIZE_PTfirst; i++)
		flag_PTfirst[i] = UNUSED;

	seekFrame = 0;
}

/*PageTable::~PageTable()
{
	delete[] PT;
	delete[] *PT;
}*/

int seek(int &curpos)//ONLY FOR findFrame
{
	int num = 0;
	
	while (curpos < NUM_PageFrame && PageTable::proc_frame[curpos] != UNUSED)
	{
		num++;
		curpos = ++curpos % NUM_PageFrame;
		if (num == NUM_PageFrame)//找了一个轮回也没有可分配page frame则结束查找，返回失败信号
			return INVALID;
	}

	return curpos;
}
ll PageTable::findFrame(ll page)
{
	int PT_first = page / SIZE_PTsecond;//映射在一级PT上
	int PT_second = page % SIZE_PTsecond;//算出在二级PT上的位置，即相对于一级PT的offset
	pageTOframe[0] = page;//记录虚页号

	int validFrame = INVALID;
	if (flag_PTfirst[PT_first] == UNUSED)
	{
		PT[PT_first] = new ll[SIZE_PTsecond];
		for (int i = 0; i < SIZE_PTsecond; i++)
			PT[PT_first][i] = UNUSED;//initialize

		//allocate
		flag_PTfirst[PT_first] = USED;
		validFrame = seek(seekFrame);
		if (validFrame != INVALID)//可分配
		{
			proc_frame[validFrame] = USED;
			pageTOframe[1] = validFrame;
			PT[PT_first][PT_second] = validFrame;
			seekFrame++;
		}
		else
		{
			//LRU
			pageTOframe[1] = pos_LRU;
			PT[PT_first][PT_second] = pos_LRU;
			pos_LRU = ++pos_LRU % NUM_PageFrame;
		}

		return INVALID;//PT miss occur
	}
	else
	{
		if (PT[PT_first][PT_second] == UNUSED)
		{
			validFrame = seek(seekFrame);
			if (validFrame != INVALID)//可分配
			{
				proc_frame[validFrame] = USED;
				pageTOframe[1] = validFrame;
				PT[PT_first][PT_second] = validFrame;
				seekFrame++;
			}
			else
			{
				//LRU
				pageTOframe[1] = pos_LRU;
				PT[PT_first][PT_second] = pos_LRU;
				pos_LRU = ++pos_LRU % NUM_PageFrame;
			}

			return INVALID;//PT miss occur
		}
		else
		{
			pageTOframe[1] = PT[PT_first][PT_second];

			return PT[PT_first][PT_second];
		}
	}
}



/********************************************************************************************/
/***************************************Class TLB********************************************/
/********************************************************************************************/

TLB::TLB(int s = SIZE_TLB)
{
	size = s;
	for (int i = 0; i < s; i++)
	{
		entry_page[i] = INVALID;
		entry_frame[i] = INVALID;
		time[i] = INVALID;
	}
}

TLB::TLB(TLB &temp)
{
	this->size = temp.size;
	for (int i = 0; i < temp.size; i++)
	{
		this->entry_page[i] = temp.entry_page[i];
		this->entry_frame[i] = temp.entry_frame[i];
		this->time[i] = temp.time[i];
	}
}

ll TLB::searchEntry(ll page, int curtime)
{
	for (int i = 0; i < getSize(); i++)
	{
		if (entry_page[i] == page)
		{
			time[i] = curtime;
			return entry_frame[i];
		}
	}

	//else--->TLB miss occur
	return INVALID;
}

void TLB::addEntry(ll page, ll frame, int curtime)
{
	//LRU
	int min = 0;
	for (int i = getSize()-1; i > 0; i--)
	{
		if (time[min] > time[i])
			min = i;
	}
	//cout << "min:" << min << endl;
	entry_page[min] = page;
	entry_frame[min] = frame;
	time[min] = curtime;//重置访问时间
}

void TLB::refreshTLB()
{
	for (int i = 0; i < getSize(); i++)
	{
		entry_page[i] = INVALID;
		entry_frame[i] = INVALID;
		time[i] = INVALID;
	}
}

/*void TLB::loadTLB(TLB &tlb)
{
	if (this->size != tlb.size)
	{
		cout << "Load TLB failed !!!!!  ------Their size is different." << endl;
		return;
	}

	//success
	for (int i = 0; i < this->size; i++)
	{
		this->entry_page[i] = tlb.entry_page[i];
		this->entry_frame[i] = tlb.entry_frame[i];
		this->time[i] = tlb.time[i];
	}
}

void TLB::saveTLB(string name_file)
{
	ofstream out(name_file);
	if (out.is_open())
	{
		cout << "open file succeed !!" << endl;
		out << "TLB Size:" << getSize() << endl;
		out << "NO.\tPage Number\tFrame Number\tTime\n";
		for (int i = 0; i < this->size; i++)
			out << i << "\t" << entry_page[i] << "\t\t" << entry_frame[i] << "\t\t" << time[i] << endl;

		out.close();
	}
	return;
}
*/


/********************************************************************************************/
/**************************************Class Process*****************************************/
/********************************************************************************************/

//initialize for static variables
TLB Process::tlb = TLB(SIZE_TLB);
int Process::num_totalproc = 0;

Process::Process(string n, int mb)
{
	num_totalproc++;

	name = n;
	procnum = num_totalproc;
	size = mb;
	times_run = 0;
	times_totalaccess = 0;
	times_TLBmiss = 0;
	times_pagefault = 0;
	pt = PageTable();

	ll b = (ll)mb*pow(2, 20);
	addr_high[0] = (ll)pow(2, 32) - b / 2;
	addr_high[1] = (ll)pow(2, 32) - 1;
	addr_low[0] = 0;
	addr_low[1] = b / 2 - 1;

	for (int i = 0; i < NUM_PageFrame; i++)
		proc_frame[i] = UNUSED;
}

void Process::action(string name_file)
{
	ofstream out(name_file, ios::app);
	if (out.is_open())
	{
		cout << "open file " << name_file << " succeed !!" << endl;

		setTimes_Run();
		updatePT();
		tlb.refreshTLB();
		cout << "NO:" << getProcnum() << "\tsize:" << getSize() << "\truntime:" << getTimes_Run() << endl;
		//cout << "LOW:" << "0x" << setw(8) << setfill('0') << setiosflags(ios::uppercase) << hex << addr_low[0] << "\t0x" << setw(8) << setfill('0') << addr_low[1] << endl;
		//cout << "HIGH: " << "0x" << setw(8) << setfill('0') << setiosflags(ios::uppercase) << hex << addr_high[0] << "\t0x" << addr_high[1] << endl;
		out << "--------------------->>>>>>>ROUND " << getTimes_Run() << "<<<<<<<---------------------\n";
		out << "VP number\tPF number\tTLB hit/no hit\tPT hit/no hit\n";
		int temp = RANDOMROUND*NUM_ADDR_PR;
		string status_TLB = "hit";
		string status_PT = "hit";
		for (int i = 0; i < temp; i++)
		{
			int curtime = temp * getTimes_Run() + i;
			ll temp_page = addr_access[i] / pow(2, 13);//除去offset，即右移13位(8K = 2^13的offset)
			ll temp_frame;
			int res_tlb = tlb.searchEntry(temp_page, curtime);
			int res_PT;
			if (res_tlb == INVALID)//TLB miss
			{
				status_TLB = "no hit";
				setTimes_TLBmiss();
				res_PT = pt.findFrame(temp_page);
				if (res_PT == INVALID)//PT miss
				{
					status_PT = "no hit";
					setTimes_Pagefault();
					temp_frame = pt.getFrame();
					pt.proc_frame[temp_frame] = getProcnum();
					proc_frame[temp_frame] = getProcnum();
				}
				else
				{
					temp_frame = res_PT;
				}
				tlb.addEntry(temp_page, temp_frame, curtime);
			}
			else
			{
				temp_frame = res_tlb;
			}

			//Output
			out << temp_page << "\t\t" << temp_frame << "\t\t" << status_TLB << "\t\t" << status_PT << endl;
		}
		out << ">>>>>>>>>>>>>>>>Page Fault Rate is " << (double)getTimes_Pagefault() / (double)getTimes_Totalaccess() * 100.0 << "% <<<<<<<<<<<<<<<<\n";
		out << endl;

		cout << "save file " << name_file << " succeed !!\n" << endl;
		out.close();
	}
}

ll random()//generate random number
{
	srand((unsigned)time(0));
	ll rand1 = (ll)rand() + (ll)rand();
	ll rand2 = (ll)rand() + (ll)rand();
	return rand1 * rand2;
}
void Process::saveAccessAddr(int num, string name_file)
{
	setTimes_Totalaccess(num);
	//Generate
	int num_addr = 0;
	int times = num / (2 * NUM_ADDR_PR);//单独高（低）位需要产生的地址数
	//srand((unsigned)time(0));
	for (int i = 0; i < times; i++)
	{

		addr_access[num_addr++] = random() % addr_low[1];//低位
		for (int j = 0; j < NUM_ADDR_PR-1; j++)//连续多个
			addr_access[num_addr++] = addr_access[num_addr - 1] + 1;
		addr_access[num_addr++] = random() % (addr_high[1] - addr_high[0]) + addr_high[0];//高位
		for(int j=0;j<NUM_ADDR_PR-1; j++)
			addr_access[num_addr++] = addr_access[num_addr - 1] + 1;
	}

	//Output
	ofstream out(name_file, ios::app);
	if (out.is_open())
	{
		cout << "open file " << name_file << " succeed !!" << endl;
		out << "------------->>>ROUND " << getTimes_Run()+1 << "<<<-------------\n";
		out << "NO.\t\tAddress\n";
		for (int i = 0; i < num; i++)
		{
			out << dec << i + 1 << "\t\t" << "0x" << setw(8) << setfill('0') << setiosflags(ios::uppercase) << hex << addr_access[i] << endl;
		}
		out << endl;

		cout << "save file " << name_file << " succeed !!\n" << endl;
		out.close();
	}
}

void Process::updatePT()
{
	for (int i = 0; i < NUM_PageFrame; i++)
		if (proc_frame[i] == getProcnum() && pt.proc_frame[i] != getProcnum())//记录信息有不同
			for (int j = 0; j < SIZE_PTfirst; j++)//一级页表
				if (pt.flag_PTfirst[j] == USED)//被使用了的才更新
					for (int k = 0; k < SIZE_PTsecond; k++)//二级
						if (pt.PT[j][k] == i)
							pt.PT[j][k] = UNUSED;//回收
}

void Process::savePT(string name_file)
{
	ofstream out(name_file, ios::trunc);
	if (out.is_open())
	{
		cout << "open file " << name_file << " succeed !!" << endl;
		for (int i = 0; i < SIZE_PTfirst; i++)
		{
			if (pt.flag_PTfirst[i] == USED)//只读取已被用的，不然会访问越界
			{
				out << "------------>>一级页表<<------------>ENTRY  " << i << endl;
				out << "\t---------------->二级页表<----------------" << endl;
				out << "\t\tPage Number\tFrame Number\n";
				for (int j = 0; j < SIZE_PTsecond; j++)
				{
					if (pt.PT[i][j] != UNUSED)
						out << "\t\t" << i*SIZE_PTsecond + j << "\t\t" << pt.PT[i][j] << endl;
					//else
						//out << "\t\t" << i*SIZE_PTsecond + j << "\t\t" << "Unused" << endl;
				}
				out << endl;
			}
		}

		cout << "save file " << name_file << " succeed !!\n" << endl;
		out.close();
	}
	return;
}