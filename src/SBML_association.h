#pragma once

#include "SBML_gene.h"
#include "rapidxml/rapidxml.hpp"
#include <cstddef>
#include <vector>

namespace SBML {
	struct association_node {
		enum relation { OR, AND };
		bool read(rapidxml::xml_node<>* node, relation rel, gene* gene_list);
		std::string fmt(gene* gene_list, bool quoted);
		int depth();

		std::vector<size_t> m_gene;
		relation m_relation;
		std::vector<association_node> m_children;
	};
	struct association {
		bool read(rapidxml::xml_node<>* node, gene* gene_list);
		int depth();
		std::string fmt(gene* gene_list);
		association_node m_root;
	};
}
