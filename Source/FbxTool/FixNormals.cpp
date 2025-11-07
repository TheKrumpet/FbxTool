#pragma once

#include <fbxsdk.h>
#include <iostream>
#include <format>
#include <vector>

using std::cout;
using std::endl;

namespace FbxTool {

	static void PopulateWithUniqueControlPoints(FbxMesh* mesh, int pIdx, std::vector<FbxVector4>& points) {
		int vCount = mesh->GetPolygonSize(pIdx);

		// Pull out the unique positions.
		for (int vIdx = 0; vIdx < vCount; vIdx++) {
			int cpIdx = mesh->GetPolygonVertex(pIdx, vIdx);
			FbxVector4 cPoint = mesh->GetControlPointAt(cpIdx);
			bool duplicate = false;

			for (int pointIdx = 0; pointIdx < points.size(); pointIdx++) {
				FbxVector4 compare = points[pointIdx];

				if (compare.Distance(cPoint) < 1e-6) {
					duplicate = true;
					break;
				}
			}

			if (!duplicate) points.push_back(cPoint);
		}
	}

	static void FixNormals(FbxNode* node) {
		FbxMesh* mesh = node->GetMesh();

		if (mesh) {
			for (int lIdx = 0; lIdx < mesh->GetLayerCount(); lIdx++) {
				FbxLayer* layer = mesh->GetLayer(lIdx);
				FbxLayerElementNormal* normals = layer->GetNormals();

				int pCount = mesh->GetPolygonCount();
				int nIdx = -1;

				for (int pIdx = 0; pIdx < pCount; pIdx++) {
					int vCount = mesh->GetPolygonSize(pIdx);

					for (int vIdx = 0; vIdx < vCount; vIdx++) {
						nIdx++;

						FbxVector4 normal;
						mesh->GetPolygonVertexNormal(pIdx, vIdx, normal);
						int vertexIdx = mesh->GetPolygonVertex(pIdx, vIdx);

						if (normal.Length() < 0.001) {
							cout << std::format("Attempting to regenerate normal for mesh {} at face {}, vtx {}, normal {}", node->GetName(), pIdx, vertexIdx, nIdx) << endl;

							std::vector<FbxVector4> points;

							PopulateWithUniqueControlPoints(mesh, pIdx, points);

							if (points.size() < 3) {
								cout << "Face has less than 3 unique points; cannot generate normals for this face." << endl;
								continue;
							}

							FbxVector4 e1 = points[1] - points[0];
							FbxVector4 e2 = points[2] - points[0];
							FbxVector4 newNormal = e1.CrossProduct(e2);
							newNormal.Normalize();

							if (newNormal.Length() < 0.999) {
								cout << "Generating normal failed." << endl;
							}

							normals->GetDirectArray().SetAt(nIdx, newNormal);
						}
						else if (normal.Length() < 0.999) {
							// Normalise the normal
							cout << std::format("Normalising normal at idx {}", nIdx) << endl;
							normal.Normalize();
							normals->GetDirectArray().SetAt(nIdx, normal);
						}
					}
				}
			}
		}

		for (int childIdx = 0; childIdx < node->GetChildCount(); childIdx++) {
			FixNormals(node->GetChild(childIdx));
		}
	}
}