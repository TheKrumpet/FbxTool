#pragma once

#include <filesystem>
#include <iostream>
#include <format>

namespace fs = std::filesystem;
using std::cout;
using std::endl;

namespace FbxTool {
	class FbxFixArgs {
	public:
		bool StripTextures;
		bool StripMaterials;
		bool RemoveDegenerateFaces;
		bool FixDuplicateUVs;
		bool FixNormals;
		fs::path InPath;
		fs::path OutPath;

		FbxFixArgs(const char** args, int argc) {
			StripTextures = false;
			StripMaterials = false;
			RemoveDegenerateFaces = false;
			FixDuplicateUVs = false;
			FixNormals = false;

			for (int argIdx = 1; argIdx < argc; argIdx++) {
				const char* arg = args[argIdx];

				if (strcmp(arg, "-st") == 0) {
					StripTextures = true;
				}
				else if (strcmp(arg, "-sm") == 0) {
					StripMaterials = true;
				}
				else if (strcmp(arg, "-rdf") == 0) {
					RemoveDegenerateFaces = true;
				}
				else if (strcmp(arg, "-duv") == 0) {
					FixDuplicateUVs = true;
				}
				else if (strcmp(arg, "-fn") == 0) {
					FixNormals = true;
				}
				else if (strcmp(arg, "-i") == 0) {
					// Next arg is input path
					InPath = fs::path(args[argIdx + 1]);
					argIdx++;
				}
				else if (strcmp(arg, "-o") == 0) {
					// Next arg is output path
					OutPath = fs::path(args[argIdx + 1]);
					argIdx++;
				}
			}

			if (InPath.empty() || OutPath.empty()) {
				throw std::runtime_error("Either input or output path is missing.");
			}
		}

		void PrintArgs() const {
			cout << "Running fix with args:" << endl;

			cout << std::format("Input path: {}", InPath.string()) << endl;
			cout << std::format("Output path: {}", OutPath.string()) << endl << endl;
			cout << "Fixes to apply:" << endl;

			if (StripTextures) {
				cout << "- Strip textures" << endl;
			}

			if (StripMaterials) {
				cout << "- Strip materials" << endl;
			}

			if (RemoveDegenerateFaces) {
				cout << "- Remove degenerate faces" << endl;
			}

			if (FixDuplicateUVs) {
				cout << "- Fix duplicate UVs" << endl;
			}

			if (FixNormals) {
				cout << "- Fix normals" << endl;
			}

			cout << endl;
		}
	};
}