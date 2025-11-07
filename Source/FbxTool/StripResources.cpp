#pragma once

#include <fbxsdk.h>
#include <iostream>
#include <format>

using std::cout;
using std::endl;

namespace FbxTool {
	static void StripTextures(FbxScene* scene) {
		while (scene->GetTextureCount() > 0) {
			FbxTexture* texture = scene->GetTexture(0);
			cout << std::format("Removing texture {}", texture->GetName()) << endl;
			scene->RemoveTexture(texture);
		}
	}

	static void StripMaterials(FbxScene* scene) {
		while (scene->GetMaterialCount() > 0) {
			FbxSurfaceMaterial* mat = scene->GetMaterial(0);
			cout << std::format("Removing material {}", mat->GetName()) << endl;
			scene->RemoveMaterial(mat);
		}
	}
}