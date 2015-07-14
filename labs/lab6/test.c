#include <stdio.h>
using namespace std;

int main(int argc, char const *argv[])
{
	char oldname[] = "hello.log";
	char newname[] = "world.log";
	rename(oldname, newname);
	return 0;
}