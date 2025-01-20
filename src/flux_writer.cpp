#include "flux_writer.h"
#include "SBML_association.h"
#include <cstddef>
#include <fstream>
#include <vector>

flux_writer::flux_writer(SBML::GEM* gem, config* conf) :
	m_GEM(gem), m_config(conf) {
}

bool flux_writer::write_association(const std::filesystem::path& iso, const std::filesystem::path& multi_comp, const std::filesystem::path& simple_comp) {
	std::ofstream ofs_iso, ofs_multi, ofs_simple;
	ofs_iso.open(iso, std::ios::out); if (!ofs_iso.is_open()) { return false; }
	ofs_multi.open(multi_comp, std::ios::out); if (!ofs_multi.is_open()) { return false; }
	ofs_simple.open(simple_comp, std::ios::out); if (!ofs_simple.is_open()) { return false; }

	for (auto& i : m_GEM->m_reaction.m_entries) {
		auto& root = i.m_association.m_root;
		int depth = root.depth();
		if (depth == 1) {
			if (root.m_relation == SBML::association_node::OR) {
				for (const auto& j : root.m_gene) { ofs_iso << i.m_id << "\t" << m_GEM->m_gene.m_name[j] << "\n"; }
			}
			else {
				for (const auto& j : root.m_gene) { ofs_simple << i.m_id << "\t" << m_GEM->m_gene.m_name[j] << "\n"; }
			}
		}
		else if (!m_config->m_trim_association || depth == 2) {
			ofs_multi << i.m_id << "\t" << i.m_association.fmt(&m_GEM->m_gene) << "\n";
		}
	}
	return true;
}

bool flux_writer::write_reaction(const std::filesystem::path& stoichiometry, const std::filesystem::path& annotation, const std::filesystem::path& lookup) {
	std::ofstream ofs_S, ofs_ann, ofs_lookup;
	ofs_S.open(stoichiometry, std::ios::out); if (!ofs_S.is_open()) { return false; }
	ofs_ann.open(annotation, std::ios::out); if (!ofs_ann.is_open()) { return false; }
	ofs_lookup.open(lookup, std::ios::out); if (!ofs_lookup.is_open()) { return false; }
	ofs_ann << "LB\tUB\tObj\tReaction\tpathway\trev\n";
	ofs_lookup << "ID\tNAME\tMETABOLITE\n";

	std::vector<int> objective(m_GEM->m_reaction.m_entries.size(), 0);
	for (const auto& i : m_GEM->m_objective.m_objective) {
		objective[i.first] = i.second;
	}

	const auto& entries = m_GEM->m_reaction.m_entries;
	size_t n_entries = entries.size();
	for (size_t i = 0; i < n_entries; ++i) {
		for (const auto& j : entries[i].m_reactant) {
			ofs_S << j.first << "\t" << entries[i].m_index << "\t" << -1 * j.second << "\n";
		}
		for (const auto& j : entries[i].m_product) {
			ofs_S << j.first << "\t" << entries[i].m_index << "\t" << j.second << "\n";
		}
		
		auto& pathway_name = m_GEM->m_pathway.m_name[m_GEM->m_pathway.m_map[i]];
		ofs_ann << entries[i].m_lower_bound << "\t"
			<< entries[i].m_upper_bound << "\t"
			<< objective[i] << "\t"
			<< entries[i].m_id << "\t\""
			<< pathway_name << "\"\t"
			<< (entries[i].m_reversible ? 1 : 0) << "\n";

		if (pathway_name == "Exchange/demand reactions") {
			for (const auto& j : entries[i].m_reactant) {
				ofs_lookup << entries[i].m_id << "\t"
					<< entries[i].m_name << "\t"
					<< m_GEM->m_species.m_name[j.first] << "\n";
			}
			for (const auto& j : entries[i].m_product) {
				ofs_lookup << entries[i].m_id << "\t"
					<< entries[i].m_name << "\t"
					<< m_GEM->m_species.m_name[j.first] << "\n";
			}
		}
	}
	return true;
}

bool flux_writer::write_gene(const std::filesystem::path& filename) {
	std::ofstream ofs;
	ofs.open(filename, std::ios::out);
	if (!ofs.is_open()) { return false; }

	for (const auto& i : m_GEM->m_gene.m_name) {
		ofs << i << "\n";
	}
	return true;
}

bool flux_writer::write_builder(const std::filesystem::path& filename) {
	std::ofstream ofs;
	ofs.open(filename, std::ios::out);
	if (!ofs.is_open()) { return false; }
	
	std::string builder = 
		"library(Matrix)\n"
		"\n"
		"build.model <- function(dir = getwd()) {\n"
		"    dir <- normalizePath(dir)\n"
		"\n"
		"    S.dat <- read.table(file.path(dir, \"S.txt\"), sep = \"\\t\", header = FALSE)\n"
		"    S.mat <- sparseMatrix(i = S.dat[, 1] + 1, j = S.dat[, 2] + 1, x = S.dat[, 3])\n"
		"\n"
		"    annotation <- read.table(file.path(dir, \"annotation.txt\"), sep = \"\\t\", header = TRUE, quote = \"\\\"\")\n"
		"    gem <- list(S = S.mat,\n"
		"                 LB = matrix(annotation$LB, ncol = 1),\n"
		"                 UB = matrix(annotation$UB, ncol = 1),\n"
		"                 Obj = matrix(annotation$Obj, ncol = 1),\n"
		"                 Reaction = annotation$Reaction,\n"
		"                 pathway = annotation$pathway,\n"
		"                 rev = matrix(annotation$rev, ncol = 1))\n"
		"\n"
		"    iso.dat <- read.table(file.path(dir, \"iso.txt\"), sep = \"\\t\", header = FALSE)\n"
		"    iso <- split(iso.dat[, 2], iso.dat[, 1])\n"
		"    iso <- lapply(iso, unlist)\n"
		"    \n"
		"    simple.dat <- read.table(file.path(dir, \"simple_comp.txt\"), sep = \"\\t\", header = FALSE)\n"
		"    simple <- split(simple.dat[, 2], simple.dat[, 1])\n"
		"    simple <- lapply(simple, unlist)\n"
		"    \n"
		"    multi.dat <- read.table(file.path(dir, \"multi_comp.txt\"), sep = \"\\t\", header = FALSE)\n"
		"    multi <- split(multi.dat[, 2], multi.dat[, 1])\n"
		"    multi <- lapply(multi, unlist)\n"
		"\n"
		"    gene.list <- read.table(file.path(dir, \"gene_list.txt\"), sep = \"\\t\", header = FALSE)\n"
		"    gene_num <- data.frame(V1 = 1:nrow(gene.list), raw.id = gene.list[, 1])\n"
		"    rownames(gene_num) <- gene.list[, 1]\n"
		"\n"
		"    return(list(gene_num = gene_num, gem = gem, iso = iso, simple_comp = simple, multi_comp = multi))\n"
		"}\n";
	ofs << builder;
	return true;
}

