#include "SBML.h"
#include "config.h"
#include "flux_writer.h"
#include <filesystem>
#include <iostream>

int work(config* conf);
void help();

int main(int argc, char** argv) {
	config conf;
	if (!conf.parse(argc, argv)) {
		std::cerr << "Bad cmdline arguments. Stop.\n" <<
			"Try SBML2Flux -h for help." << std::endl;
		return -1;
	}
	if (conf.m_show_help) {
		help();
		return 0;
	}
	if (conf.m_model.empty()) {
		std::cerr << "No input file. Nothing to do.\n" <<
			"Try SBML2Flux -h for help." << std::endl;
		return -1;
	}
	if (conf.m_out.empty()) {
		conf.m_out = std::filesystem::current_path();
	}
	return work(&conf);
}

int work(config* conf) {
	int res = 0;

	auto try_doing = [&](bool b){
		if (b) { std::cerr << "Succeeded" << std::endl; }
		else {
			std::cerr << "Failed" << std::endl;
			res = -1;
		}
		return b;
	};

	SBML::GEM gem;
	std::cerr << "Reading GEM ... ";
	bool b = try_doing(gem.read(conf->m_model));
	if (!b) {
		std::cerr << "Failed to read GEM" << std::endl;
		return -1;
	}

	std::filesystem::path out_root(conf->m_out);
	flux_writer writer(&gem, conf);
	
	std::cerr << "Writing iso, multi_comp and simple_comp ... ";
	try_doing(writer.write_association(out_root / "iso.txt", out_root / "multi_comp.txt", out_root / "simple_comp.txt"));

	std::cerr << "Writing reactions ... ";
	try_doing(writer.write_reaction(out_root / "S.txt", out_root / "annotation.txt", out_root / "lookup.txt"));

	std::cerr << "Writing gene list ... ";
	try_doing(writer.write_gene(out_root / "gene_list.txt"));

	std::cerr << "Writing builder ... ";
	try_doing(writer.write_builder(out_root / "builder.R"));

    return res;
}

void help() {
	std::cerr <<
		"\nSBML2Flux\n"
		"\n"
		"This program is designed to convert Genome-Scale Metabolic Models (GEMs)\n"
		"provided by Metabolic Atlas into the internal GEM model format used by\n"
		"METAFlux. This tool enables the extension of METAFlux, originally\n"
		"tailored for human models, to other species.\n"
		"\n"
		"Note: The SBML parsing module in this program is specifically designed for\n"
		"SBML models from Metabolic Atlas. Due to the presence of non-standard\n"
		"elements in Metabolic Atlas models and the limited scope of the parser\n"
		"(which extracts only essential information), this tool does not support SBML\n"
		"models from other sources.\n"
		"\n"
		"Usage\n"
		"\n"
		"SBML2Flux <model_file.xml> -O <output_directory>\n"
		"\n"
		"    <model_file.xml>: Path to the SBML model file from Metabolic Atlas.\n"
		"\n"
		"    -O, --out-dir <output_directory>: Path to the directory where output files\n"
		"    will be saved.\n"
		"\n"
        "    -t, --trim-association: Enable removing of gene associations with depth\n"
		"    greater than 2.\n"
        "    By default, this option is disabled. When enabled, gene associations with\n"
        "    depth greater than 2 will be automatically removed.\n"
        "    This option is provided to adapt to a known limitation in METAFlux, which\n"
        "    can only process gene associations with depth less than or equal to 2.\n"
        "    Enabling this option ensures compatibility with METAFlux but may result in\n"
        "    loss of information for complex gene associations.\n"
		"\n"
		"    -h, --help: Show this help and exit.\n"
		"\n"
		"Output Files\n"
		"\n"
		"The program generates the following files in the specified output directory:\n"
		"\n"
		"    iso.txt: Gene-reaction associations for OR relationships.\n"
		"\n"
		"    multi_comp.txt: Complex gene-reaction associations with multiple nodes.\n"
		"\n"
		"    simple_comp.txt: Simple gene-reaction associations (non-OR).\n"
		"\n"
		"    S.txt: Stoichiometric coefficients of reactants and products.\n"
		"\n"
		"    annotation.txt: Reaction metadata including bounds, objectives, pathways,\n"
		"    etc.\n"
		"\n"
		"    gene_list.txt: Gene list.\n"
		"\n"
		"    lookup.txt: a file contains all Exchange/demand reactions, which is designed\n"
		"    to construct the medium file for METAFlux. \n"
		"\n"
		"    builder.R: R script for model construction.\n" << std::endl;
}
