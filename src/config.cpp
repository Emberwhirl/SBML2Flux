#include "config.h"
#include <cstring>

bool config::parse(int argc, char** argv) {
	for (int i = 1; i < argc; ++i) {
		if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
			m_show_help = true;
		}
		else if (strcmp(argv[i], "-O") == 0 || strcmp(argv[i], "--out-dir") == 0) {
			if (++i >= argc) { return false; }
			if (!m_out.empty()) { return false; }
			m_out = argv[i];
		}
		else if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--trim-association") == 0) {
			m_trim_association = true;
		}
		else {
			if (!m_model.empty()) { return false; }
			m_model = argv[i];
		}
	}
	return true;
}
