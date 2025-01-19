#pragma once

#include "SBML_gene.h"
#include "SBML_objective.h"
#include "SBML_parameter.h"
#include "SBML_pathway.h"
#include "SBML_reaction.h"
#include "SBML_species.h"
#include <filesystem>

namespace SBML {
	struct GEM {
		bool read(std::filesystem::path filename);
		reaction m_reaction;
		pathway m_pathway;
		species m_species;
		parameter m_param;
		objective m_objective;
		gene m_gene;
	};
}
