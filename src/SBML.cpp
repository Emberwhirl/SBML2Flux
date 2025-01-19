#include "SBML.h"
#include <cassert>
#include <fstream>

bool SBML::GEM::read(std::filesystem::path filename) {
	std::ifstream ifs;
	ifs.open(filename, std::ios::in);
	if (!ifs.is_open()) { return false; }

	std::string xml((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

	rapidxml::xml_document<> doc;
	doc.parse<0>(xml.data());

	auto root = doc.first_node();
	if (!root) { return false; }
	auto model = root->first_node("model");
	if (!model) { return false; }

	auto node = model->first_node("listOfSpecies");
	if (!node) { return false; }
	if (!m_species.read(node)) { return false; }
	
	node = model->first_node("listOfParameters");
	if (!node) { return false; }
	if (!m_param.read(node)) { return false; }
	
	node = model->first_node("fbc:listOfGeneProducts");
	if (!node) { return false; }
	if (!m_gene.read(node)) { return false; }

	node = model->first_node("listOfReactions");
	if (!node) { return false; }
	if (!m_reaction.read(node, &m_param, &m_species, &m_gene)) { return false; }
	
	node = model->first_node("groups:listOfGroups");
	if (!node) { return false; }
	if (!m_pathway.read(node, &m_reaction)) { return false; }

	node = model->first_node("fbc:listOfObjectives");
	if (!node) { return false; }
	if (!m_objective.read(node, &m_reaction)) { return false; }

	return true;
}


