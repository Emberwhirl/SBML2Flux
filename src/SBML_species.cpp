#include "SBML_species.h"

bool SBML::species::read(rapidxml::xml_node<>* node) {
	rapidxml::xml_node<>* species_node = node->first_node();
	while (species_node) {
		auto attr = species_node->first_attribute("id");
		if (!attr) { return false; }
		std::string id = attr->value();
		m_id.push_back(id);
		m_index.emplace(id, m_id.size() - 1);
		
		attr = species_node->first_attribute("name");
		if (!attr) {
			m_name.push_back(std::string());
		}
		else {
			m_name.push_back(attr->value());
		}
		
		species_node = species_node->next_sibling();
	}
	return true;
}
