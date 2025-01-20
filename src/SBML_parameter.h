#pragma once

#include "rapidxml/rapidxml.hpp"
#include <map>
#include <string>

namespace SBML {
	struct parameter {
		bool read(rapidxml::xml_node<>* node);
		std::map<std::string, double> m_param;
	};
}
