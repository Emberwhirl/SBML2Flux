#include "SBML_gene.h"

bool SBML::gene::read(rapidxml::xml_node<>* node) {
	auto gene_node = node->first_node();
	if (!gene_node) { return false; }
	while (gene_node) {
		auto attr = gene_node->first_attribute("metaid");
		if (!attr) { return false; }
		
		std::string name(attr->value());
		m_name.push_back(name);
		m_index.emplace(name, m_name.size() - 1);

		gene_node = gene_node->next_sibling();
	}
	return true;
}

