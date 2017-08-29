#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

#include "Memory.h"
#include "Const.h"

using namespace std;

int main()
{
	//Generate files' name
	stringstream file_addr[NUM_PROCESS];
	stringstream file_visit[NUM_PROCESS];
	stringstream file_pt[NUM_PROCESS];
	for (int i = 0; i < NUM_PROCESS; i++)
	{
		file_addr[i] << FILE_ADDR << i + 1 << FILE_SUFFIX;
		file_visit[i] << FILE_VISIT << i + 1 << FILE_SUFFIX;
		file_pt[i] << FILE_PT << i + 1 << FILE_SUFFIX;
		//cout << file_addr[i].str() << endl << file_visit[i].str() << endl << file_pt[i].str() << endl;
	}

	//Start
	Process p[NUM_PROCESS] = { {"p1",1},{ "p2",32 },{ "p3",64 },{ "p4",128 },{ "p5",256 } };
	/*Process p1("p1", 1);
	Process p2("p2", 32);
	Process p3("p3", 64);
	Process p4("p4", 128);
	Process p5("p5", 256);*/

	int excute = EXECUTIMES;
	int addrnum = RANDOMROUND * NUM_ADDR_PR;
	while (excute--)
		for (int i = 0; i < NUM_PROCESS; i++)
		{
			p[i].saveAccessAddr(addrnum, file_addr[i].str());
			p[i].action(file_visit[i].str());
		}

	for (int i = 0; i < NUM_PROCESS; i++)
		p[i].savePT(file_pt[i].str());

	system("pause");
	return 0;
}