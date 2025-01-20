# SBML2Flux
SBML2Flux is a tool designed to convert Genome-Scale Metabolic Models (GEMs) provided by [Metabolic Atlas](https://metabolicatlas.org/) into the internal GEM model format used by METAFlux. It extends the functionality of METAFlux, enabling support for non-human species' metabolic models.

## Installation
### Dependencies
+ C++ 17
+ xmake
+ RapidXML

### Build Instructions
SBML2Flux only depends on the C++17 standard library and RapidXML. Since RapidXML is a header-only library and no additional installation or linking is required, which should be handled by xmake automatically.

1. Clone the repository:

```bash
git clone https://github.com/RestlessTail/SBML2Flux.git
cd SBML2Flux
```

2. Build SBML2Flux

```bash
xmake
```

## Usage
```bash
SBML2Flux <model_file>
```

+ `<model_file>`: Path to the input SBML file.
+ `-O, --out-dir`: Path to the output directory.
+ `-t, --trim-association`: Enable trimming of gene associations with depth > 2. This option is provided due to a limitation in METAFlux, which cannot handle gene associations that mix AND and OR operators with a depth greater than 2. By default, this option is disabled. When enabled, it automatically removes gene associations with a depth exceeding 2 to ensure compatibility with METAFlux.
+ `-h, --help`: Display help information and exit.

## Example
```bash
./SBML2Flux model.xml -O ./output
```

## Output Files
+ Gene reactions:
	* `iso.txt`: Gene-reaction associations for OR relationships.
	* `multi_comp.txt`: Complex gene-reaction associations with multiple nodes.
	* `simple_comp.txt`: Simple gene-reaction associations (non-OR).
	* `S.txt`: Stoichiometric matrices
	* `annotation.txt`: Reaction metadata
+ Gene list:
	* `gene_list.txt`
+ Medium:
	* lookup.txt`: A file contains all Exchange/demand reactions, which is designed to construct the medium file for METAFlux. 
+ Model construction script
	* `builder.R`

## License
This project is licensed under the [GPL-3 License](LICENSE).

## Acknowledgments
+ [Metabolic Atlas](https://metabolicatlas.org/) for providing SBML models.
+ The METAFlux team for their elegant algorithm.

