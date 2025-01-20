#include "SBML_reaction.h"
#include <cstring>
#include <string>

bool SBML::reaction::read(rapidxml::xml_node<>* node, parameter* param, species* spe, gene* gene_list) {
	rapidxml::xml_node<>* reaction_node = node->first_node();
	while (reaction_node) {
		reaction_entry entry;
		
		auto attr = reaction_node->first_attribute("id");
		if (!attr) { return false; }
		entry.m_id = attr->value();

		attr = reaction_node->first_attribute("fbc:lowerFluxBound");
		if (!attr) { return false; }
		auto itr = param->m_param.find(attr->value());
		if (itr == param->m_param.end()) { return false; }
		entry.m_lower_bound = itr->second;
		
		attr = reaction_node->first_attribute("fbc:upperFluxBound");
		if (!attr) { return false; }
		itr = param->m_param.find(attr->value());
		if (itr == param->m_param.end()) { return false; }
		entry.m_upper_bound = itr->second;
		
		attr = reaction_node->first_attribute("name");
		if (attr) { entry.m_name = attr->value(); }

		attr = reaction_node->first_attribute("reversible");
		if (!attr) { return false; }
		entry.m_reversible = strcmp(attr->value(), "false") ? true : false;

		auto reactant_list_node = reaction_node->first_node("listOfReactants");
		if (reactant_list_node) {
			auto reactant_node = reactant_list_node->first_node();
			while (reactant_node) {
				auto reactant_attr = reactant_node->first_attribute("species");
				if (!reactant_attr) { return false; }
				auto itr = spe->m_index.find(reactant_attr->value());
				if (itr == spe->m_index.end()) { return false; }

				reactant_attr = reactant_node->first_attribute("stoichiometry");
				if (!reactant_attr) { return false; }
				
				entry.m_reactant.push_back({ itr->second, std::stod(reactant_attr->value()) });
				reactant_node = reactant_node->next_sibling();
			}
		}
		
		auto product_list_node = reaction_node->first_node("listOfProducts");
		if (product_list_node) {
			auto product_node = product_list_node->first_node();
			while (product_node) {
				auto product_attr = product_node->first_attribute("species");
				if (!product_attr) { return false; }
				auto itr = spe->m_index.find(product_attr->value());
				if (itr == spe->m_index.end()) { return false; }
				
				product_attr = product_node->first_attribute("stoichiometry");
				if (!product_attr) { return false; }
				
				entry.m_product.push_back({ itr->second, std::stod(product_attr->value()) });
				product_node = product_node->next_sibling();
			}
		}
		
		auto association_node = reaction_node->first_node("fbc:geneProductAssociation");
		if (association_node) {
			if (!entry.m_association.read(association_node, gene_list)) { return false; }
		}
		
		entry.m_index = m_entries.size();
		m_entries.push_back(entry);
		m_index.emplace(entry.m_id, entry.m_index);
		
		reaction_node = reaction_node->next_sibling();
	}
	return true;
}
