#include <iostream>
#include "ObjectGenerater.h"
#include "Client.h"

using namespace std;
using namespace experiment;

int main(int argc, char** argv)
{
	//experiment::ObjectGenerater::generate("hdd-pool", 8192);
	Client client;
	client.all_migration(0, 1, 8192);
	return 0;
}
