# SBML2Flux
SBML2Flux is a tool designed to convert Genome-Scale Metabolic Models (GEMs) provided by [Metabolic Atlas](https://metabolicatlas.org/) into the internal GEM model format used by METAFlux. It extends the functionality of METAFlux, enabling support for non-human species' metabolic models.

**Warning**: This repository is currently intended for **discussion and demonstration purposes only**. The code and tools provided here are **not stable** and should not be used in production or for critical analyses.

## Conversion Process Overview
This tool converts an SBML file into a format compatible with METAflux by mapping SBML attributes to METAflux variables. Below is a summary of the mapping process:

|METAflux Variable|Description|SBML Source|
|------|-------|-------|
|`gene_num`| Maps gene IDs to indices.| Extracted from `sbml-model-listOfReactions-reaction-fbc:listOfGeneProducts`.|
|`iso`| Gene associations connected by OR logic.| Derived from `sbml-model-listOfReactions-reaction-fbc:geneProductAssociation` nodes.|
|`simple_comp`| Gene associations connected by AND logic.| Derived from `sbml-model-listOfReactions-reaction-fbc:geneProductAssociation` nodes.|
|`multi_comp`| Gene associations with mixed OR/AND logic.| Derived from `sbml-model-listOfReactions-reaction-fbc:geneProductAssociation` nodes.|
|`Hgem$LB`| Lower bounds for reactions.| Extracted from `sbml-model-listOfReactions-reaction-fbc:lowerFluxBound` (values found in `sbml-model-listOfParameters`).|
|`Hgem$UB`| Upper bounds for reactions.| Extracted from `sbml-model-listOfReactions-reaction-fbc:upperFluxBound` (values found in `sbml-model-listOfParameters`).|
|`Hgem$Reaction`| Reaction IDs.| Uses `sbml-model-listOfReactions-reaction` node `id` attribute.|
|`Hgem$pathway`| Reaction type/category.| Extracted from `sbml-model-groups:listOfGroups-groups:group` nodes.|
|`Hgem$rev`| Reversibility of reactions (1 = reversible, 0 = irreversible).| Extracted from `sbml-model-listOfReactions-reaction` node `reversible` attribute.|
|`Hgem$S`| Stoichiometric matrix (rows = reactions, columns = metabolites).| Derived from `sbml-model-listOfReactions-reaction-listOfReactants` and `listOfProducts` nodes (`stoichiometry` attribute).|
|`Hgem$Obj`| Objective function coefficients.| Extracted from `sbml-model-fbc:listOfObjectives-fbc:objective-fbc:listOfFluxObjectives` nodes (`fbc:coefficient` attribute).|

For more details, refer to the [SBML documentation](http://sbml.org) and the METAflux source code.

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
SBML2Flux <model_file.xml>
```

+ `<model_file.xml>`: Path to the input SBML file.
+ `-O, --out-dir`: Path to the output directory.
+ `-t, --trim-association`: Enable trimming of gene associations with depth > 2. This option is provided due to a limitation in METAFlux, which cannot handle gene associations that mix AND and OR operators with a depth greater than 2. By default, this option is disabled. When enabled, it automatically removes gene associations with a depth exceeding 2 to ensure compatibility with METAFlux.
+ `-h, --help`: Display help information and exit.

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
	* `lookup.txt`: A file contains all Exchange/demand reactions, which is designed to construct the medium file for METAFlux. 
+ Model construction script
	* `builder.R`

## Step-by-Step Guide for Using SBML2Flux and METAFlux
1. **Download the SBML Model**:
	* Visit [Metabolic Atlas GEM Repository](https://metabolicatlas.org/gems/repository) and locate the GitHub repository for your species of interest (e.g., Mouse-GEM).
	* Download the repository and navigate to the `model` directory to find the SBML file (e.g., `Mouse-GEM.xml`).

2. **Convert SBML to METAFlux Format**:
	* Run SBML2Flux to convert the SBML file:

    ```bash
    mkdir out
    ./SBML2Flux Mouse-GEM.xml -t -O out
    ```

3. **Build the Model in R**:
	* In R, source the `builder.R` script generated by SBML2Flux, passing the output directory as an argument:

    ```R
    source("out/builder.R")
    model <- build.model("./out/")
    ```

	* This will construct the metabolic model in R.

4. **Prepare the Medium Object**:
	* Use the `lookup.txt` file to create a medium object for your species. For example, to create a medium object for _Mus musculus_ based on human blood metabolites:

    ```R
    lookup <- read.table("./out/lookup.txt", sep = "\t", header = TRUE, quote = "")
    mouse_blood <- lookup[match(tolower(METAFlux::human_blood[, 1]), tolower(lookup[, "METABOLITE"])), ] %>%
        na.omit() %>%
        dplyr::select(reaction_name = ID, metabolite = METABOLITE)
    ```

5. **Clone and Modify METAFlux**:
	* Clone the METAFlux repository and source the necessary scripts:

    ```R
    source("calculate_score.R")
    source("optimization.R")
    ```

	* Replace the `calculate_multi_comp`, `calculate_reaction_score` and `compute_flux` functions with the provided custom implementations.

    ```R
    # This code snippet is adapted from the METAFlux project (https://github.com/KChen-lab/METAFlux),
    # and has been modified for use in this project. Original code is copyrighted by the METAFlux authors.
    # METAFlux is licensed under the MIT License: https://github.com/KChen-lab/METAFlux/blob/main/LICENSE.md
    #
    # Modified code snippet follows:
    calculate_multi_comp <- function(x, data, list, gene_num) {
        reaction <- list[[x]]
        # determine whether it is or and? within the bracket
        com <- stri_detect_fixed(str_extract_all(reaction, "\\([^()]+\\)")[[1]], "or")
        c <- gsub("\\)", "", gsub("\\(", "", str_extract_all(reaction, "\\([^()]+\\)")[[1]]))
        if (unique(com) == T) {
            newiso <- lapply(lapply(c, function(x) { unlist(strsplit(x, "or")) }), function(x) { trimws(x) })
            sum_score <- do.call(rbind, lapply(1:length(newiso), calculate_iso_score, data = data, list = newiso, gene_num = gene_num))
            feature <- trimws(unlist(strsplit(reaction, "and"))[!stri_detect_fixed(unlist(strsplit(reaction, "and")), "or")])
            data_feature <- feature[feature %in% rownames(data)]
            vec <- gene_num[data_feature, ]$V1
            expr <- as.matrix(data[data_feature, , drop = F])
            whole_score <- rbind((1/vec) * expr, sum_score)
            norm <- apply(whole_score, 2, min, na.rm = T)
        } else if (unique(com) == FALSE) {
            # trim the white space for iso enzyme
            newiso <- lapply(lapply(c, function(x) { unlist(strsplit(x, "and")) }), function(x) { trimws(x) })
            sum_score <- do.call(rbind, lapply(1:length(newiso), calculate_simple_comeplex_score,
                                               data = data, list = newiso, gene_num = gene_num))
            feature <- trimws(unlist(strsplit(reaction, "or"))[!stri_detect_fixed(unlist(strsplit(reaction, "or")), "and")])
            data_feature <- feature[feature %in% rownames(data)]
            vec <- gene_num[data_feature, ]$V1
            expr <- as.matrix(data[data_feature, , drop = F])
            upper_score <- (1/vec) * expr
            whole_score <- rbind(upper_score, sum_score)
            norm <- colSums(whole_score, na.rm = T)
        }
    }

    calculate_reaction_score <- function(data, model) {
        if (sum(data < 0) > 0)
            stop("Expression data needs to be all positive")
        # make sure features are present
        features <- rownames(data)
        if (sum(features %in% rownames(model$gene_num)) == 0)
            stop("Requested gene names cannot be found. Rownames of input data should be human gene names.Please check the rownames of input data.")  #change
        message(paste0(round(sum(features %in% rownames(model$gene_num))/3625 * 100, 3), "% metabolic related genes were found......"))
        gene_num <- model$gene_num
        Hgem <- model$gem
        iso <- model$iso
        multi_comp <- model$multi_comp
        simple_comp <- model$simple_comp
        message("Computing metabolic reaction activity scores......")
        core <- do.call(rbind, lapply(1:length(iso), calculate_iso_score, data = data, list = iso, gene_num = gene_num))
        core2 <- do.call(rbind, lapply(1:length(simple_comp), calculate_simple_comeplex_score, data, list = simple_comp, gene_num = gene_num))
        core3 <- do.call(rbind, lapply(1:length(multi_comp), calculate_multi_comp, data = data, list = multi_comp, gene_num = gene_num))
        message("Preparing for score matrix......")
        rownames(core) <- names(iso)
        rownames(core2) <- names(simple_comp)
        rownames(core3) <- names(multi_comp)
        big_score_matrix <- rbind(core, core2, core3)
        big_score_matrix <- apply(big_score_matrix, 2, stdize, na.rm = T)
        # handle NAN
        big_score_matrix[is.na(big_score_matrix)] <- 0
        empty_helper <- as.data.frame(Hgem$Reaction)
        colnames(empty_helper) <- "reaction"
        Final_df <- merge(empty_helper, big_score_matrix, all.x = T, by.x = 1, by.y = 0)
        Final_df[is.na(Final_df)] <- 1
        rownames(Final_df) <- Final_df$reaction
        Final_df$reaction <- NULL
        Final_df <- Final_df[Hgem$Reaction, , drop = F]
        if (all.equal(rownames(Final_df), Hgem$Reaction)) {
            message("Metabolic reaction activity scores successfully calculated \n")
        } else {
            message("Calculation not reliable Check input data format \n")
        }
        Final_df[which(Hgem$LB == 0 & Hgem$UB == 0), ] <- 0  #this is biomass reaction, do not use
        return(Final_df)
    }

    compute_flux <- function(mras, gem.model, medium) {
        message("Setting up for optimization.....")
        Hgem <- gem.model$gem
        P <- diag(1, ncol(Hgem$S), ncol(Hgem$S))
        q <- rep(0, ncol(Hgem$S))
        q[which(Hgem$Obj == 1)] <- -10000
        A <- as.matrix(rbind(Hgem$S, P))
        resu <- list()
        flux_vector <- list()
        message("Computing bulk RNA-seq flux.....")
        Seq <- seq(1, ncol(mras))
        pb <- txtProgressBar(0, length(Seq), style = 3)
        for (i in Seq) {
            setTxtProgressBar(pb, i)
            origlb <- Hgem$LB
            origlb[Hgem$rev == 1] <- -mras[, i][Hgem$rev == 1]
            origlb[Hgem$rev == 0] <- 0
            origlb <- origlb[, 1]
            origub <- mras[, i]
            origlb[Hgem$Reaction %in% Hgem$Reaction[which(Hgem$pathway == "Exchange/demand reactions")]] <- 0
            origlb[Hgem$Reaction %in% medium$reaction_name] <- -1
            l <- c(rep(0, nrow(Hgem$S)), origlb)
            u <- c(rep(0, nrow(Hgem$S)), origub)
            settings <- osqpSettings(max_iter = 1000000L, eps_abs = 1e-04,
                                     eps_rel = 1e-04, adaptive_rho_interval = 50, verbose = FALSE)
            model <- osqp(P, q, A, l, u, settings)
            # Solve problem
            res <- model$Solve()
            resu[[i]] <- res
            flux_vector[[i]] <- res$x
        }
        close(pb)
        flux_vector <- do.call(cbind, flux_vector)
        colnames(flux_vector) <- colnames(mras)
        rownames(flux_vector) <- Hgem$Reaction
        return(flux_vector)
    }
    ```

6. **Calculate Reaction Scores and Fluxes**:
	* Use the modified functions to calculate reaction scores and compute metabolic fluxes:

    ```R
    scores <- calculate_reaction_score(data, model)
    res <- compute_flux(scores, model, mouse_blood)
    ```

	* Here, `data` is your gene expression data, and `model` is the metabolic model constructed from the SBML file.

### **Notes**:
+ Ensure your gene expression data is properly formatted, with gene names matching those in the metabolic model.
+ The process involves several steps and custom modifications, so carefully follow the instructions to ensure accurate results.

This workflow allows you to convert, build, and analyze metabolic models for non-human species using SBML2Flux and METAFlux.

## License
This project is licensed under the [GPL-3 License](LICENSE).

## Acknowledgments
+ [Metabolic Atlas](https://metabolicatlas.org/) for providing SBML models.
+ The METAFlux team for their elegant algorithm.

