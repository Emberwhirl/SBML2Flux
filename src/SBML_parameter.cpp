#include "SBML_parameter.h"
#include <string>

bool SBML::parameter::read(rapidxml::xml_node<>* node) {
	rapidxml::xml_node<>* param_node = node->first_node();
	while (param_node) {
		auto id_attr = param_node->first_attribute("id");
		if (!id_attr) { return false; }
		auto value_attr = param_node->first_attribute("value");
		if (!value_attr) { return false; }
		m_param.emplace(id_attr->value(), std::stod(value_attr->value()));
		param_node = param_node->next_sibling();
	}
	return true;
}

