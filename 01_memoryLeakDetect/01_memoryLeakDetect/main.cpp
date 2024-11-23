#include <iostream>

#include "myNew.h"

class CTest
{
public:
	CTest()
	{
		_x = 1;
		_y = 1;
	}

	~CTest()
	{
	}

	void Test()
	{
		_x = 0;
		_y = 0;
	}

private:
	int _x;
	int _y;
};

int main()
{
	CTest* pTest = new CTest;
	pTest->Test();
	delete pTest;

	CTest* pTest2 = new CTest[10];
	CTest* pTest3 = new CTest;
}
