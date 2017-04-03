#include <iostream>

#include "../../reader/src/ConfigReader.h"


int main(void)
{
	std::cout << "Hello Andrew" << std::endl;
	ConfigReader reader("/home/admin/reader/config/config.ini");
	reader.display();
}


