#pragma once
#include <sstream>
class SimpleReader
{
		void fromSimpleText(const char* str)
		{
			std::istringstream s(str);
			std::string line;

			while(std::getline(s, line))
			{
				std::istringstream l(line);

				std::string type;
				l >> type;
				if(type == "place")
				{
					std::string placeName;
					l >> placeName;

					// addPlace(placeName)
				}
				else if(type == "trans")
				{
					std::string transName;
					int cost;
					l >> transName >> cost;

					// addTrans(transName, cost)

				}
				else if(type == "arc")
				{
					std::string orig, dest;
					l >> orig >> dest;

					// addArc(orig, dest);
				}
			}
		}

};
