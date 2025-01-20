#include "SBML_objective.h"
#include <cstdlib>
#include <string>

bool SBML::objective::read(rapidxml::xml_node<>* node, reaction* react) {
	rapidxml::xml_node<>* obj_node = node->first_node("fbc:objective");
	while (obj_node) {
		auto flux_list_node = obj_node->first_node("fbc:listOfFluxObjectives");
		if (!flux_list_node) { return false; }
		auto flux_node = flux_list_node->first_node("fbc:fluxObjective");
		while (flux_node) {
			auto react_attr = flux_node->first_attribute("fbc:reaction");
			if (!react_attr) { return false; }
			auto itr = react->m_index.find(react_attr->value());
			if (itr == react->m_index.end()) { return false; }

			auto coef_attr = flux_node->first_attribute("fbc:coefficient");
			if (!coef_attr) { return false; }

			m_objective.push_back({ itr->second, std::stod(coef_attr->value()) });

			flux_node = flux_node->next_sibling();
		}
		obj_node = obj_node->next_sibling();
	}
	return true;
}
