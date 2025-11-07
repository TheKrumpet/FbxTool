# FBX Tool

A small tool to get some assets to behave with the MonoGame content pipeline.

Requires the [Autodesk FBX SDK](https://aps.autodesk.com/developer/overview/fbx-sdk) (and probably some DLL/linker configuration because I couldn't be bothered to set up a build system.)

# WARNING
This is something I hacked together to fix some models; it's only here because it may be useful to someone else (probably myself in future). If you're using this, do not overwrite your source FBX files until you're absolutely _sure_ that this tool has done what you want it to. **There will be no help forthcoming if you ignore this advice**.

## Usage
`.\FbxTool.exe fix -i <input_path> -o <output_path> <fix_flags>`

Input path can be a single FBX file or a directory (if it's a directory, it should have a trailing slash.)  
Output path can be a single FBX file if the input is a single file, or a directory.

### Fix flags
#### -st - Strip textures
Removes all textures from the FBX file.

#### -sm - Strip materials
Removes all materials from the FBX file

#### -rdf - Remove degenerate faces
Removes all faces with zero area. This can cause issues with generating tangent frames within monogame.

This flag is *EXTREMELY EXPERIMENTAL*. This has to rebuild the model, skipping degenerate faces, so some data may (and probably will) be lost. It at the very least copies UVs and Normals, as that's what I needed.

#### -duv - Fix duplicate UVs
If all UVs for a face match, then they get ignored by Assimp as it assumes it's invalid data (which it might not be, for example in the case of a colour palette texture). This will just bump the first UV slightly so it's distinct from the others, which causes Assimp to return them again.

#### -fn - Fix Normals
If it comes across a zero normal, it will attempt to regenerate that normal from the face. If it comes across a normal with a length of less than 1, it'll normalize it to 1.

# License
Licensed under the terms of the GNU General Public License V3