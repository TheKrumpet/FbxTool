#include <format>
#include <string>
#include <iostream>
#include <fbxsdk.h>

#include "FbxPrint.cpp"
#include "FbxFix.cpp"
#include "FbxArgs.cpp"

using std::cout;
using std::endl;

int main(int argc, const char** argv) {
    cout << "FBX Tool v0.1" << endl << endl;

    if (argc == 1) {
        const std::string indent = "  ";

        cout << "Commands:" << endl;
        cout << "info <file_name>" << endl;
        cout << indent << "prints some random info about the specified FBX file" << endl << endl;
        cout << "fix -i <input_path> -o <output_path>" << endl;
        cout << indent << "applies fixes to an FBX file, or a directory of FBX files specified by input_path, and copies them to output_path" << endl;
        cout << indent << "Available fixes: " << endl;
        cout << indent << indent << "-st: Strips all textures from the file" << endl;
        cout << indent << indent << "-sm: Strips all materials from the file" << endl;
        cout << indent << indent << "-rdf: Removes all zero-area faces. This is experimental, as this command rebuilds the model, so other data may be lost." << endl;
        cout << indent << indent << "-duv: Fixes duplicate UVs" << endl;
        cout << indent << indent << "-fn: Fixes normals" << endl;

        exit(0);
    }

    FbxManager* fbxsdk = FbxManager::Create();
    FbxIOSettings* ioSettings = FbxIOSettings::Create(fbxsdk, IOSROOT);
    int exit_code = 0;

    if (strcmp(argv[1], "info") == 0) {
        FbxTool::PrintFbxInfo(argv[2], fbxsdk);
    }
    else if (strcmp(argv[1], "fix") == 0) {
        FbxTool::FbxFixArgs args = FbxTool::FbxFixArgs(argv, argc);

        if (std::error_code err = FbxTool::FbxFix(args, fbxsdk); err) {
            cout << err.message() << endl;
            exit_code = err.value();
        }
    }
    else {
        cout << "No command specified." << endl;
    }

    fbxsdk->Destroy();
    exit(exit_code);
}