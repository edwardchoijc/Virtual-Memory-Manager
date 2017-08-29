#ifndef MEMORY_H
#define MEMORY_H

#include <string>

#include "Const.h"

using namespace std;

class PageTable
{
public:
	//Variables
	static int proc_frame[NUM_PageFrame];//��Ǳ��ĸ�����ռ��
	static int pos_LRU;//���LRU�㷨�����ȱ��û���page frame
	int flag_PTfirst[SIZE_PTfirst];//���һ��ҳ���Ƿ�������
	ll **PT = new ll*[SIZE_PTfirst];//����ҳ��ṹ

	//Functions
	PageTable();
	ll findFrame(ll page);
	ll getFrame() const { return pageTOframe[1]; }
	//~PageTable();
private:
	int seekFrame;
	ll pageTOframe[2];//[0]��page��[1]��pageframe
};


class TLB
{
public:
	TLB(int s);
	TLB(TLB &temp);//���ƹ��캯��
	ll searchEntry(ll page, int curtime);//��page�ҵ���Ӧentry��page frame
	void addEntry(ll page, ll frame, int curtime);
	int getSize() const { return size; }
	void refreshTLB();//�ڽ����л�ʱ�� ��Ҫ��� TLB Ϊ�½��̵�������׼��
	//void saveTLB(string name_file); //ÿ��ִ����ϳ��ֽ����л�ʱ������ҳ�����ݱ��浽�ⲿ�ļ�
private:
	ll entry_page[SIZE_TLB];//ÿ��entry��Ӧ��page��
	ll entry_frame[SIZE_TLB];//ÿ��entry��Ӧ��page frame��
	int time[SIZE_TLB];//ÿ��entry����ʱ��
	int size;//TLB����
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
	ll getHighAddr() const { return addr_high[0]; }//��λ��ַĬ����0xFFFFFFFF����
	ll getLowAddr() const { return addr_low[1]; }//��λ��ַĬ����0x00000000��ʼ
	void saveAccessAddr(int num, string name_file);
	void updatePT();
	void savePT(string name_file);
private:
	static int num_totalproc;//������¼Ŀǰ���ܽ��������Ա���
	string name;
	int procnum;//��ǰ�������
	int size;//��λMB
	int times_run;//��¼�������д���
	int times_totalaccess;//��¼�ܵķ��ʵ�ַ����
	int times_TLBmiss;//��¼TLB miss�Ĵ���
	int times_pagefault;//��¼page fault�Ĵ���
	ll addr_high[2];//����Ϊ����ַ��Χ��addr_high[0]��addr_high[1]
	ll addr_low[2];//��ͬ��
	ll addr_access[RANDOMROUND*NUM_ADDR_PR];//��¼50������ķ��ʵ�500����ַ
	int proc_frame[NUM_PageFrame];//��Ǳ�����ʹ��page frame�����,��page table�е����Ӧ
};

/*class System
{
public:

private:

};*/



#endif