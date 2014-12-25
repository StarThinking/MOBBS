#include <iostream>
#include <stdio.h>
#include <pthread.h>
#include "Client.h"

using namespace experiment;

void* apply_storage0(void* argv)
{
	return NULL;
}

void* apply_storage1(void* argv)
{
	return NULL;
}

int main(int argc, char** argv)
{
	Client client;
	client.apply_migration("auto-generated-0", 0, 1, "10.0.0.20");
	return 0;
}
