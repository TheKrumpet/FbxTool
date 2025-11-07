#pragma once

#include <fbxsdk.h>
#include <iostream>
#include <format>
#include <vector>

using std::cout;
using std::endl;

namespace FbxTool {
	static bool PolyHasArea(FbxMesh* mesh, int pIdx) {
		int vCount = mesh->GetPolygonSize(pIdx);

		bool hasArea = false;

		for (int vStart = 0; vStart <= vCount - 3; vStart++) {
			int i0 = mesh->GetPolygonVertex(pIdx, vStart);
			int i1 = mesh->GetPolygonVertex(pIdx, vStart + 1);
			int i2 = mesh->GetPolygonVertex(pIdx, vStart + 2);

			FbxVector4 v0 = mesh->GetControlPointAt(i0);
			FbxVector4 v1 = mesh->GetControlPointAt(i1);
			FbxVector4 v2 = mesh->GetControlPointAt(i2);

			FbxVector4 e1 = v1 - v0;
			FbxVector4 e2 = v2 - v0;

			FbxVector4 cross = e1.CrossProduct(e2);
			double area = cross.Length() * 0.5;

			hasArea = area > 1e-8;

			if (hasArea) break;
		}

		return hasArea;
	}

	static FbxMesh* RebuildMesh(FbxMesh* mesh, FbxManager* fbxsdk) {
		cout << std::format("Rebuilding {}", mesh->GetNode()->GetName()) << endl;

		FbxMesh* clean = FbxMesh::Create(fbxsdk, mesh->GetName());

		int vCount = mesh->GetControlPointsCount();
		clean->InitControlPoints(vCount);

		for (int vIdx = 0; vIdx < vCount; vIdx++) {
			clean->SetControlPointAt(mesh->GetControlPointAt(vIdx), vIdx);
		}

		FbxLayerElementUV* uvs = mesh->GetLayer(0)->GetUVs();
		FbxLayerElementNormal* normals = mesh->GetLayer(0)->GetNormals();

		if (uvs->GetMappingMode() != FbxLayerElement::eByPolygonVertex) {
			cout << "Cannot regenerate mesh: wrong UV mapping" << endl;
			return nullptr;
		}
		if (uvs->GetReferenceMode() != FbxLayerElement::eIndexToDirect) {
			cout << "Cannot regenerate mesh: wrong UV referencing mode" << endl;
			return nullptr;
		}

		if (normals->GetMappingMode() != FbxLayerElement::eByPolygonVertex) {
			cout << "Cannot regenerate mesh: wrong normal mapping" << endl;
			return nullptr;
		}
		if (normals->GetReferenceMode() != FbxLayerElement::eDirect) {
			cout << "Cannot regenerate mesh: wrong normal referencing mode" << endl;
			return nullptr;
		}

		FbxLayerElementUV* cleanUVs = FbxLayerElementUV::Create(clean, uvs->GetName());
		cleanUVs->SetMappingMode(FbxLayerElement::eByPolygonVertex);
		cleanUVs->SetReferenceMode(FbxLayerElement::eIndexToDirect);

		FbxLayerElementNormal* cleanNormals = FbxLayerElementNormal::Create(clean, normals->GetName());
		cleanNormals->SetMappingMode(FbxLayerElement::eByPolygonVertex);
		cleanNormals->SetReferenceMode(FbxLayerElement::eDirect);

		int pCount = mesh->GetPolygonCount();
		bool unmapped;

		for (int pIdx = 0; pIdx < pCount; pIdx++) {
			int pvCount = mesh->GetPolygonSize(pIdx);

			std::vector<FbxVector4> points;

			if (!PolyHasArea(mesh, pIdx)) {
				cout << std::format("Face {} has zero area; skipping this face.", pIdx) << endl;
			}
			else {
				clean->BeginPolygon();

				for (int pvIdx = 0; pvIdx < pvCount; pvIdx++) {
					int cpIdx = mesh->GetPolygonVertex(pIdx, pvIdx);

					clean->AddPolygon(cpIdx);

					FbxVector2 uv;
					mesh->GetPolygonVertexUV(pIdx, pvIdx, uvs->GetName(), uv, unmapped);
					int uvIdx = cleanUVs->GetDirectArray().Add(uv);
					cleanUVs->GetIndexArray().Add(uvIdx);

					FbxVector4 normal;
					mesh->GetPolygonVertexNormal(pIdx, pvIdx, normal);
					cleanNormals->GetDirectArray().Add(normal);
				}

				clean->EndPolygon();
			}
		}

		clean->CreateLayer();
		FbxLayer* cleanLayer = clean->GetLayer(0);

		cleanLayer->SetUVs(cleanUVs, FbxLayerElement::eTextureDiffuse);
		cleanLayer->SetNormals(cleanNormals);

		return clean;
	}

	static void RemoveDegenerateFaces(FbxNode* node, FbxManager* fbxsdk) {
		FbxMesh* mesh = node->GetMesh();

		if (mesh) {
			FbxMesh* clean = RebuildMesh(mesh, fbxsdk);

			if (clean) {
				node->SetNodeAttribute(clean);
				mesh->Destroy();
			}
		}

		for (int cIdx = 0; cIdx < node->GetChildCount(); cIdx++) {
			RemoveDegenerateFaces(node->GetChild(cIdx), fbxsdk);
		}
	}
}