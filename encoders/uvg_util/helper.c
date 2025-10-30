#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "common.h"
#include "helper.h"

static void apply_preset_settings(H266Config* config, const char* preset_name) {
    if (!config || !preset_name) return;
    
    // Browse all presets
    for (int i = 0; i < 11 && preset_values[i][0] != NULL; i++) {
        // Compare the preset name with strcmp
        if (strcmp(preset_values[i][0], preset_name) == 0) {
            printf("Applying preset: %s\n", preset_name);
            
            // Browse all key-value pairs in the preset
            for (int j = 1; preset_values[i][j] != NULL && preset_values[i][j+1] != NULL; j += 2) {
                const char* key = preset_values[i][j];
                const char* value = preset_values[i][j+1];
                
                // Apply settings according to key
                if (strcmp(key, "sao") == 0) {
                    if (strcmp(value, "off") == 0) config->sao = 0;
                    else if (strcmp(value, "band") == 0) config->sao = 1;
                    else if (strcmp(value, "edge") == 0) config->sao = 2;
                    else if (strcmp(value, "full") == 0) config->sao = 3;
                }
                else if (strcmp(key, "alf") == 0) {
                    if (strcmp(value, "off") == 0) config->alf = 0;
                    else if (strcmp(value, "nocc") == 0) config->alf = 1;
                    else if (strcmp(value, "full") == 0) config->alf = 2;
                }
                else if (strcmp(key, "subme") == 0) {
                    config->subme = atoi(value);
                }
                else if (strcmp(key, "rd") == 0) {
                    config->rdo = atoi(value);
                }
                else if (strcmp(key, "rdoq") == 0) {
                    config->rdoq_enable = atoi(value);
                }
                else if (strcmp(key, "ref") == 0) {
                    config->ref_frames = atoi(value);
                }
                else if (strcmp(key, "gop") == 0) {
                    config->gop_len = atoi(value);
                }
                else if (strcmp(key, "max-merge") == 0) {
                    config->max_merge = atoi(value);
                }
                else if (strcmp(key, "me") == 0) {
                    if (strcmp(value, "hexbs") == 0) config->ime_algorithm = 0;
                    else if (strcmp(value, "tz") == 0) config->ime_algorithm = 1;
                    else if (strcmp(value, "full") == 0) config->ime_algorithm = 2;
                }
                else if (strcmp(key, "deblock") == 0) {
                    // Manage the “0:0” format
                    sscanf(value, "%d:%d", &config->deblock_beta, &config->deblock_tc);
                    config->deblock_enable = 1;

                }
                else if (strcmp(key, "signhide") == 0) {
                    config->signhide_enable = atoi(value);
                }
                else if (strcmp(key, "rdoq-skip") == 0) {
                    config->rdoq_skip = atoi(value);
                    config->rdoq_enable = 1;
                }
                else if (strcmp(key, "transform-skip") == 0) {
                    config->trskip_enable = atoi(value);
                }
                else if (strcmp(key, "mv-rdo") == 0) {
                    config->mv_rdo = atoi(value);
                }
                else if (strcmp(key, "full-intra-search") == 0) {
                    config->full_intra_search = atoi(value);
                }
                else if (strcmp(key, "early-skip") == 0) {
                    config->early_skip = atoi(value);
                }
                else if (strcmp(key, "fast-residual-cost") == 0) {
                    config->fast_residual_cost_limit = atoi(value);
                }
                else if (strcmp(key, "cclm") == 0) {
                    config->cclm = atoi(value);
                }
                else if (strcmp(key, "dual-tree") == 0) {
                    config->dual_tree = atoi(value);
                }
                else if (strcmp(key, "jccr") == 0) {
                    config->jccr = atoi(value);
                }
                else if (strcmp(key, "mip") == 0) {
                    config->mip = atoi(value);
                }
                else if (strcmp(key, "mrl") == 0) {
                    config->mrl = atoi(value);
                }
                else if (strcmp(key, "dep-quant") == 0) {
                    config->dep_quant = atoi(value);
                }
                
                printf("  %s = %s\n", key, value);
            }
            return; // Preset found and applied
        }
    }
    
    printf("Warning: preset '%s' not found, using default settings\n", preset_name);
}


