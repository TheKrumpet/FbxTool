#pragma once

#include <iostream>
#include <filesystem>

#include "FbxToolResult.cpp"

namespace fs = std::filesystem;

namespace FbxTool {
	static std::error_code ValidateAndGetFbxPaths(const fs::path inPath, const fs::path outPath, std::vector<fs::path>& fbxPaths, bool& isFile) {
		if (!fs::exists(inPath)) return FbxToolResult::INPUT_NOT_EXISTS;

		isFile = inPath.has_filename();
		if (isFile && inPath.extension() != ".fbx") return FbxToolResult::INPUT_FILE_NOT_FBX;

		if (fs::exists(outPath)) {
			if (outPath.has_filename()) {
				if (!isFile) return FbxToolResult::FILE_OUTPUT_FOR_DIR_INPUT;
				if (outPath.extension() != ".fbx") return FbxToolResult::OUTPUT_FILE_NOT_FBX;
			}
		}

		if (isFile) {
			fbxPaths.push_back(inPath);
		}
		else {
			for (fs::directory_entry const& dir_entry : fs::recursive_directory_iterator(inPath)) {
				if (!dir_entry.is_regular_file() || dir_entry.path().extension() != ".fbx") continue;
				fbxPaths.push_back(dir_entry.path());
			}
		}

		return FbxToolResult::SUCCESS;
	}
}