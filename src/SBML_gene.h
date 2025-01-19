#pragma once

#include "rapidxml/rapidxml.hpp"
#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

namespace SBML {
	struct gene {
		bool read(rapidxml::xml_node<>* node);
		std::vector<std::string> m_name;
		std::unordered_map<std::string, size_t> m_index;
	};
}
