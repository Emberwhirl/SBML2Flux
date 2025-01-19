#pragma once

#include <filesystem>

struct config {
	bool parse(int argc, char** argv);
	std::filesystem::path m_out;
	std::filesystem::path m_model;
	bool m_show_help { false };
	bool m_trim_association { false };
};
