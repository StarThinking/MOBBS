#include <iostream>
#include "ObjectGenerater.h"
#include "Client.h"

using namespace std;
using namespace experiment;

int main(int argc, char** argv)
{
	//experiment::ObjectGenerater::generate("hdd-pool", 8192);
	Client client;
	//client.all_migration(1, 0, 8192);

	int count = 20;
	int total;
	for(int i = 0; i < count; i++)
	{
		total += client.read_one_object("auto-generated-0", 0);
	}
	long a = total / count;
	cout << a << endl;
	return 0;
}
