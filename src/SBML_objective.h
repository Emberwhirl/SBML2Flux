#pragma once

#include "SBML_reaction.h"
#include "rapidxml/rapidxml.hpp"
#include <cstddef>
#include <utility>
#include <vector>

namespace SBML {
	struct objective {
		bool read(rapidxml::xml_node<>* node, reaction* react);
		std::vector<std::pair<size_t, double>> m_objective;
	};
}
