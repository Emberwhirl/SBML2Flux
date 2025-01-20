#pragma once

#include "SBML_reaction.h"
#include "rapidxml/rapidxml.hpp"
#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

namespace SBML {
	struct pathway {
		bool read(rapidxml::xml_node<>* node, reaction* react);
		std::vector<std::string> m_name; // pathway:index => pathway:name
		std::unordered_map<std::string, size_t> m_index; // pathway:name => pathway:index
		std::vector<size_t> m_map; // reaction:name <=> reaction:index => pathway:index
	};
}
