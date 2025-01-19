#pragma once

#include "SBML.h"
#include "config.h"
#include <filesystem>

class flux_writer {
	public:
		flux_writer(SBML::GEM* gem, config* conf);
		bool write_association(const std::filesystem::path& iso, const std::filesystem::path& multi_comp, const std::filesystem::path& simple_comp);
		bool write_reaction(const std::filesystem::path& stoichiometry, const std::filesystem::path& annotation);
		bool write_gene(const std::filesystem::path& filename);
		bool write_builder(const std::filesystem::path& filename);
	private:
		SBML::GEM* m_GEM;
		config* m_config;
};
