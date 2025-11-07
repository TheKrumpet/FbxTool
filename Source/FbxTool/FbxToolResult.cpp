#pragma once

#include <string>
#include <system_error>

namespace FbxTool {
	enum class FbxToolResult {
		SUCCESS = 0,
		INPUT_NOT_EXISTS = 1,
		INPUT_FILE_NOT_FBX = 2,
		FILE_OUTPUT_FOR_DIR_INPUT = 3,
		OUTPUT_FILE_NOT_FBX = 4
	};

	struct FbxToolResultCategory : public std::error_category {
		const char* name() const noexcept override {
			return "StripTextureError";
		}
		std::string message(const int condition) const override {
			const FbxToolResult error = static_cast<FbxToolResult>(condition);

			switch (error) {
			case FbxToolResult::SUCCESS: return "The operation completed successfully.";
			case FbxToolResult::INPUT_NOT_EXISTS: return "The specified input path does not exist.";
			case FbxToolResult::INPUT_FILE_NOT_FBX: return "The specified input file is not an .fbx file.";
			case FbxToolResult::FILE_OUTPUT_FOR_DIR_INPUT: return "Specified a single output file for a directory input; a directory as input must have a directory as output.";
			case FbxToolResult::OUTPUT_FILE_NOT_FBX: return "The specified output file is not an .fbx file.";
			default: return "Unknown error.";
			}
		}
	};

	static std::error_code make_error_code(const FbxToolResult& r) {
		return { static_cast<int>(r), FbxToolResultCategory() };
	}
}

template<> struct std::is_error_code_enum<FbxTool::FbxToolResult> : public std::true_type {};
