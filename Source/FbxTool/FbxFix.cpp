#include <format>
#include <string>
#include <iostream>
#include <filesystem>
#include <fbxsdk.h>

#include "FbxToolResult.cpp"
#include "GetFbxPaths.cpp"
#include "StripResources.cpp"
#include "FbxArgs.cpp"
#include "RemoveDegenerateFaces.cpp"
#include "FixUVs.cpp"
#include "FixNormals.cpp"

namespace fs = std::filesystem;
using std::count;
using std::endl;

namespace FbxTool {
	static std::error_code FbxFix(FbxFixArgs& args, FbxManager* fbxsdk) {
		args.PrintArgs();

		std::vector<fs::path> fbxPaths;
		bool isFile;

		std::error_code pathError = ValidateAndGetFbxPaths(args.InPath, args.OutPath, fbxPaths, isFile);

		if (pathError) return pathError;

		FbxScene* scene = FbxScene::Create(fbxsdk, "StripTexture");
		FbxImporter* importer = FbxImporter::Create(fbxsdk, "");
		FbxExporter* exporter = FbxExporter::Create(fbxsdk, "");

		for (fs::path const& fbxPath : fbxPaths) {
			const fs::path relative = fbxPath == args.InPath
				? fbxPath.filename()
				: fs::relative(fbxPath, args.InPath);

			const fs::path output = args.OutPath / relative;

			cout << format("Processing file {}", relative.string()) << endl;

			scene->Clear();

			if (!importer->Initialize(fbxPath.string().c_str(), -1, fbxsdk->GetIOSettings())) {
				cout << "Could not process file; import failed." << endl;
				continue;
			}

			importer->Import(scene);

			if (args.StripTextures) {
				cout << "Stripping textures" << endl;
				StripTextures(scene);
			}

			if (args.StripMaterials) {
				cout << "Stripping materials" << endl;
				StripMaterials(scene);
			}

			if (args.RemoveDegenerateFaces) {
				cout << "Removing degenerate faces" << endl;
				RemoveDegenerateFaces(scene->GetRootNode(), fbxsdk);
			}

			if (args.FixDuplicateUVs) {
				cout << "Fixing duplicate UVs" << endl;
				FixUvs(scene->GetRootNode());
			}

			if (args.FixNormals) {
				cout << "Fixing normals" << endl;
				FixNormals(scene->GetRootNode());
			}

			if (!exporter->Initialize(output.string().c_str(), -1, fbxsdk->GetIOSettings())) {
				cout << format("Could not process file; export to path {} failed.", output.string()) << endl;
				continue;
			}

			cout << format("Writing to path {}", output.string()) << endl << endl;
			exporter->SetFileExportVersion(FBX_2013_00_COMPATIBLE);
			exporter->Export(scene);
		}

		importer->Destroy();
		exporter->Destroy();
		scene->Destroy();

		return FbxToolResult::SUCCESS;
	}
}