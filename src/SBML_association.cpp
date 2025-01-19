#include "SBML_association.h"
#include "SBML_gene.h"
#include <algorithm>
#include <cstring>
#include <sstream>

bool SBML::association::read(rapidxml::xml_node<>* node, gene* gene_list) {
	auto n = node->first_node();
	if (!n) { return false; }
	if (strcmp(n->name(), "fbc:or") == 0) {
		return m_root.read(node->first_node(), association_node::OR, gene_list);
	}
	else if (strcmp(n->name(), "fbc:and") == 0){
		return m_root.read(node->first_node(), association_node::AND, gene_list);
	}
	else {
		auto attr = n->first_attribute("fbc:geneProduct");
		if (!attr) { return false; }
		auto itr = gene_list->m_index.find(attr->value());
		if (itr == gene_list->m_index.end()) { return false; }
		m_root.m_gene.push_back(itr->second);
		return n->next_sibling() == nullptr;
	}
}

bool SBML::association_node::read(rapidxml::xml_node<>* node, relation rel, gene* gene_list) {
	m_relation = rel;
	
	auto n = node->first_node();
	while (n) {
		if (strcmp(n->name(), "fbc:or") == 0) {
			association_node an;
			if (!an.read(n, association_node::OR, gene_list)) { return false; }
			m_children.push_back(an);
		}
		else if (strcmp(n->name(), "fbc:and") == 0){
			association_node an;
			if (!an.read(n, association_node::AND, gene_list)) { return false; }
			m_children.push_back(an);
		}
		else {
			auto attr = n->first_attribute("fbc:geneProduct");
			if (!attr) { return false; }
			auto itr = gene_list->m_index.find(attr->value());
			if (itr == gene_list->m_index.end()) { return false; }
			m_gene.push_back(itr->second);
		}
		n = n->next_sibling();
	}
	return true;
}

std::string SBML::association_node::fmt(SBML::gene* gene_list, bool quoted) {
	std::string op = m_relation == association_node::OR ? " or " : " and ";
	std::stringstream ss;
	if (quoted) { ss << "("; }
	bool first_elem = true;
	for (const auto& i : m_gene) {
		if (!first_elem) { ss << op; }
		ss << gene_list->m_name[i];
		first_elem = false;
	}
	for (auto& i : m_children) {
		if (!first_elem) { ss << op; }
		ss << i.fmt(gene_list, true);
		first_elem = false;
	}
	if (quoted) { ss << ")"; }
	return ss.str();
}

std::string SBML::association::fmt(SBML::gene* gene_list) {
	return m_root.fmt(gene_list, false);
}

int SBML::association_node::depth() {
	if (m_children.empty()) {
		return 1;
	}
	else {
		int max_depth = m_children.front().depth();
		for (auto itr = m_children.begin() + 1; itr != m_children.end(); ++itr) {
			int d = itr->depth();
			if (max_depth < d) { max_depth = d; }
		}
		return max_depth + 1;
	}
}

int SBML::association::depth() {
	return m_root.depth();
}
