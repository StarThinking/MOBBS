#include <iostream>
#include <stdlib.h>
#include "Analyzer.h"

using namespace monitor;
using namespace std;

int main(int argc, char** argv)
{
	ExtentQueue queue;
	ExtentDetail eds[1000];
	srand((unsigned int)time(0));
	for(int i = 0; i < 1000; i ++)
	{
		eds[i].m_weight = rand();
		queue.insert(&eds[i]);
	}
	//cout << queue.m_length << endl;
	/*
	ExtentNode* p = queue.m_head;
	
	for(int i = 0; i < 10; i ++)
	{
		cout << p << "   next: " << p->m_next << "  prev: " << p->m_prev << endl;
		p = p->m_next;
	}
	cout << "head: " << queue.m_head << "tail: " << queue.m_tail << endl;
	*/
	/*
	for(ExtentNode* p = queue.m_head; p != NULL; p = p->m_next)
	{
		cout << p->m_ed->m_weight << endl;
	}
	*/
	
	while(queue.m_length != 0)
	{
		cout << queue.pop_tail()->m_weight << endl;
	}
	
	return 1;
}
