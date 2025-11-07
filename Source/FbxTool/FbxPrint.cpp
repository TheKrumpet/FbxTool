#include <format>
#include <string>
#include <iostream>
#include <filesystem>
#include <fbxsdk.h>

#include "FbxToolResult.cpp"
#include "GetFbxPaths.cpp"

using std::cout;
using std::endl;
using std::string;
using std::format;

namespace FbxTool {
	class TreePrinter {
	public:
		TreePrinter(int indentWidth) {
			_indent = 0;
			_indentWidth = indentWidth;
		}

		void PrintLine(std::string toPrint) {
			cout << format("{}{}", string(_indent, ' '), toPrint) << endl;
		}

		void Indent() {
			_indent += _indentWidth;
		}

		void Unindent() {
			_indent -= _indentWidth;
		}
	private:
		int _indent;
		int _indentWidth;
	};

	static string MappingModeToString(const FbxLayerElement::EMappingMode mappingMode) {
		switch (mappingMode) {
		case FbxLayerElement::EMappingMode::eNone: return "None";
		case FbxLayerElement::EMappingMode::eByControlPoint: return "ControlPoint";
		case FbxLayerElement::EMappingMode::eByPolygonVertex: return "PolygonVertex";
		case FbxLayerElement::EMappingMode::eByPolygon: return "Polygon";
		case FbxLayerElement::EMappingMode::eByEdge: return "Edge";
		case FbxLayerElement::EMappingMode::eAllSame: return "AllSame";
		}
	}

	static string ReferenceModeToString(const FbxLayerElement::EReferenceMode refMode) {
		switch (refMode) {
		case FbxLayerElement::EReferenceMode::eDirect: return "Direct";
		case FbxLayerElement::EReferenceMode::eIndex: return "Index";
		case FbxLayerElement::EReferenceMode::eIndexToDirect: return "IndexToDirect";
		}
	}

	static void PrintNodeInfo(FbxNode* node, TreePrinter& printer) {
		printer.PrintLine(format("Node: {}", node->GetName()));

		FbxMesh* mesh = node->GetMesh();
		printer.Indent();

		if (mesh) {
			printer.PrintLine(format("Vertex count: {}", mesh->GetPolygonVertexCount()));
			printer.PrintLine(format("Layer count: {}", mesh->GetLayerCount()));

			for (int layerIdx = 0; layerIdx < mesh->GetLayerCount(); layerIdx++) {
				FbxLayer* layer = mesh->GetLayer(layerIdx);
				printer.PrintLine(format("Layer {}", layerIdx));

				printer.Indent();

				FbxLayerElementUV* uvs = layer->GetUVs();

				if (uvs) {
					printer.PrintLine(format("UV layer: {}", uvs->GetName()));
					printer.Indent();
					printer.PrintLine(format("Mapping: {}", MappingModeToString(uvs->GetMappingMode())));
					printer.PrintLine(format("Reference: {}", ReferenceModeToString(uvs->GetReferenceMode())));
					printer.Unindent();
				}

				FbxLayerElementNormal* normals = layer->GetNormals();
				if (normals) {
					printer.PrintLine(format("Normal layer: {}", normals->GetName()));
					printer.Indent();
					printer.PrintLine(format("Mapping: {}", MappingModeToString(normals->GetMappingMode())));
					printer.PrintLine(format("Reference: {}", ReferenceModeToString(normals->GetReferenceMode())));
					printer.Unindent();
				}

				printer.Unindent();
			}

		}
		else {
			printer.PrintLine("No mesh");
		}

		cout << endl;

		for (int childIdx = 0; childIdx < node->GetChildCount(); childIdx++) {
			PrintNodeInfo(node->GetChild(childIdx), printer);
		}

		printer.Unindent();
	}

	static std::error_code PrintFbxInfo(const std::string input, FbxManager* fbxsdk) {
		const fs::path inPath = fs::path(input);
		const fs::path outPath = fs::path();

		std::vector<fs::path> fbxPaths;
		bool isFile;

		std::error_code pathError = ValidateAndGetFbxPaths(inPath, outPath, fbxPaths, isFile);

		if (pathError) return pathError;

		FbxScene* scene = FbxScene::Create(fbxsdk, "PrintInfo");
		FbxImporter* importer = FbxImporter::Create(fbxsdk, "");

		for (fs::path const& fbxPath : fbxPaths) {
			scene->Clear();

			if (!importer->Initialize(fbxPath.string().c_str(), -1, fbxsdk->GetIOSettings())) {
				cout << "Could not process file; import failed." << endl;
				continue;
			}

			cout << "Model: " << fbxPath.string() << endl;

			importer->Import(scene);

			TreePrinter printer = TreePrinter(2);
			FbxNode* root = scene->GetRootNode();

			PrintNodeInfo(root, printer);
		}

		importer->Destroy();
		scene->Destroy();

		return FbxToolResult::SUCCESS;
	}
}