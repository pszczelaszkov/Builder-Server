#ifndef __TOOLBOX__
#define __TOOLBOX__

#include <vector>
#include <string>
#include <stdint.h>
#include <sstream>

typedef std::vector<std::string> stringVector;

class Toolbox
{
	public:
		Toolbox();
		stringVector stringExplode(const std::string string,const char separator);
		static std::string intToString(int32_t source);
		static int32_t stringToInt(std::string source);
};
#endif
