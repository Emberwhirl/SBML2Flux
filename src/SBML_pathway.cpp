#include "SBML_pathway.h"
#include <cstddef>

bool SBML::pathway::read(rapidxml::xml_node<>* node, reaction* react) {
	m_map.resize(react->m_entries.size());
	
	rapidxml::xml_node<>* group_node = node->first_node();
	while (group_node) {
		auto name_attr = group_node->first_attribute("groups:name");
		if (!name_attr) { return false; }
		std::string name = name_attr->value();
		m_name.push_back(name);
		size_t name_index = m_name.size() - 1;
		m_index.emplace(name, name_index);
		auto member_list_node = group_node->first_node("groups:listOfMembers");
		if (!member_list_node) { return false; }
		auto member_node = member_list_node->first_node();
		while (member_node) {
			auto reaction_attr = member_node->first_attribute("groups:idRef");
			if (!reaction_attr) { return false; }
			auto itr = react->m_index.find(reaction_attr->value());
			if (itr == react->m_index.end()) { return false; }
			m_map[itr->second] = name_index;
			member_node = member_node->next_sibling();
		}
		group_node = group_node->next_sibling();
	}
	return true;
}

