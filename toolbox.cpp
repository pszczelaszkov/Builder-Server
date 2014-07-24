#include "toolbox.h"

Toolbox::Toolbox()
{
}

stringVector Toolbox::stringExplode(const std::string string,const char separator)
{
        stringVector result;
        size_t start = 0, end = 0;
        while((end = string.find(separator, start)) != std::string::npos)
        {
                result.push_back(string.substr(start, end - start));
                start = end + 1;
        }

        result.push_back(string.substr(start));
        return result;
}

std::string Toolbox::intToString(int32_t source)
{
	std::stringstream convert;
	std::string result;

	convert << source;
	convert >> result;

	return result;
}

int32_t Toolbox::stringToInt(std::string source)
{
        std::stringstream convert;
        int32_t result;

        convert << source;
        convert >> result;

        return result;
}

