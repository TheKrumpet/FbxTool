#pragma once

#include <fbxsdk.h>
#include <iostream>
#include <format>

using std::cout;
using std::endl;

namespace FbxTool {
	static void FixUvs(FbxNode* node) {
		FbxMesh* mesh = node->GetMesh();

		if (mesh) {
			FbxStringList uvSetNames;
			mesh->GetUVSetNames(uvSetNames);
			const char* uvSetName = uvSetNames.GetStringAt(0);

			bool unmapped;
			bool identical = true;
			FbxVector2 firstUv;
			mesh->GetPolygonVertexUV(0, 0, uvSetName, firstUv, unmapped);

			int polyCount = mesh->GetPolygonCount();

			for (int pIdx = 0; pIdx < polyCount; pIdx++) {
				int polySize = mesh->GetPolygonSize(pIdx);

				for (int vIdx = 0; vIdx < polySize; vIdx++) {
					FbxVector2 uv;
					mesh->GetPolygonVertexUV(pIdx, vIdx, uvSetName, uv, unmapped);

					if (fabs(uv[0] - firstUv[0]) > 1e-6 || fabs(uv[1] - firstUv[1]) > 1e-6) {
						identical = false;
						break;
					}
				}

				if (!identical) break;
			}

			if (identical) {
				cout << "All uvs match - moving first" << endl;

				// Nudge the first UV slightly to make it distinct.
				firstUv[0] += 1e-6;
				FbxGeometryElementUV* uvElement = mesh->GetElementUV(uvSetName);
				uvElement->GetDirectArray().SetAt(0, firstUv);
			}
		}

		for (int childIdx = 0; childIdx < node->GetChildCount(); childIdx++) {
			FixUvs(node->GetChild(childIdx));
		}
	}
}