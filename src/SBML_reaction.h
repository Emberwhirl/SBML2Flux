#pragma once

#include <cstddef>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <rapidxml/rapidxml.hpp>
#include "SBML_association.h"
#include "SBML_gene.h"
#include "SBML_parameter.h"
#include "SBML_species.h"

namespace SBML {
	struct reaction_entry {
		double m_lower_bound;
		double m_upper_bound;
		std::vector<std::pair<size_t, double>> m_reactant;
		std::vector<std::pair<size_t, double>> m_product;
		bool m_reversible;
		std::string m_id;
		size_t m_index;
		std::string m_name;
		association m_association;
	};
	
	struct reaction {
		bool read(rapidxml::xml_node<>* node, parameter* param, species* spe, gene* gene_list);
		std::vector<reaction_entry> m_entries; // reaction:index => reaction:entry
		std::unordered_map<std::string, size_t> m_index; // reaction:name => reaction:index
	};
}
