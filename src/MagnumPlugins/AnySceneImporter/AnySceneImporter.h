#ifndef Magnum_Trade_AnySceneImporter_h
#define Magnum_Trade_AnySceneImporter_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
              Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

/** @file
 * @brief Class @ref Magnum::Trade::AnySceneImporter
 */

#include <Magnum/Trade/AbstractImporter.h>

#include "MagnumPlugins/AnySceneImporter/configure.h"

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_ANYSCENEIMPORTER_BUILD_STATIC
    #if defined(AnySceneImporter_EXPORTS) || defined(AnySceneImporterObjects_EXPORTS)
        #define MAGNUM_ANYSCENEIMPORTER_EXPORT CORRADE_VISIBILITY_EXPORT
    #else
        #define MAGNUM_ANYSCENEIMPORTER_EXPORT CORRADE_VISIBILITY_IMPORT
    #endif
#else
    #define MAGNUM_ANYSCENEIMPORTER_EXPORT CORRADE_VISIBILITY_STATIC
#endif
#define MAGNUM_ANYSCENEIMPORTER_LOCAL CORRADE_VISIBILITY_LOCAL
#else
#define MAGNUM_ANYSCENEIMPORTER_EXPORT
#define MAGNUM_ANYSCENEIMPORTER_LOCAL
#endif

namespace Magnum { namespace Trade {

/**
@brief Any scene importer plugin

Detects file type based on file extension, loads corresponding plugin and then
tries to open the file with it.

This plugin depends on the @ref Trade library and is built if
`WITH_ANYSCENEIMPORTER` is enabled when building Magnum Plugins. To use as a
dynamic plugin, you need to load the @cpp "AnySceneImporter" @ce plugin from
`MAGNUM_PLUGINS_IMPORTER_DIR`. To use as a static plugin or as a dependency of
another plugin with CMake, you need to request the `AnySceneImporter` component
of the `MagnumPlugins` package in CMake and link to the
`MagnumPlugins::AnySceneImporter` target. See @ref building-plugins,
@ref cmake-plugins and @ref plugins for more information.

Supported formats:

-   3ds Max 3DS and ASE (`*.3ds`, `*.ase`), loaded with any plugin that
    provides `3dsImporter`
-   AC3D (`*.ac`), loaded with any plugin that provides `Ac3dImporter`
-   Blender 3D (`*.blend`), loaded with any plugin that provides
    `BlenderImporter`
-   Biovision BVH (`*.bvh`), loaded with any plugin that provides `BvhImporter`
-   CharacterStudio Motion (`*.csm`), loaded with any plugin that provides
    `CsmImporter`
-   COLLADA (`*.dae`), loaded with @ref ColladaImporter or any other plugin
    that provides it
-   DirectX X (`*.x`), loaded with any plugin that provides `DirectXImporter`
-   AutoCAD DXF (`*.dxf`), loaded with any plugin that provides `DxfImporter`
-   Autodesk FBX (`*.fbx`), loaded with any plugin that provides `FbxImporter`
-   glTF (`*.gltf`, `*.glb`), loaded with any plugin that provides
    `GltfImporter`
-   Industry Foundation Classes (IFC/Step) (`*.ifc`), loaded with any plugin
    that provides `IfcImporter`
-   Irrlicht Mesh and Scene (`*.irrmesh`, `*.irr`), loaded with any plugin that
    provides `IrrlichtImporter`
-   LightWave, LightWave Scene (`*.lwo`, `*.lws`), loaded with any plugin that
    provides `LightWaveImporter`
-   Modo (`*.lxo`), loaded with any plugin that provides `ModoImporter`
-   Milkshape 3D (`*.ms3d`), loaded with any plugin that provides
    `MilkshapeImporter`
-   Wavefront OBJ (`*.obj`), loaded with @ref ObjImporter or any other plugin
    that provides it
-   Ogre XML (`*.xml`), loaded with any plugin that provides `OgreImporter`
-   OpenGEX (`*.ogex`), loaded with @ref OpenGexImporter or any other plugin
    that provides it
-   Stanford (`*.ply`), loaded with @ref StanfordImporter or any other plugin
    that provides it
-   Stereolitography (`*.stl`), loaded with any plugin that provides
    `StlImporter`
-   TrueSpace (`*.cob`, `*.scn`), loaded with any plugin that provides
    `TrueSpaceImporter`
-   Unreal (`*.3d`), loaded with any plugin that provides `UnrealImporter`
-   Valve Model (`*.smd`, `*.vta`), loaded with any plugin that provides
    `ValveImporter`
-   XGL (`*.xgl`, `*.zgl`), loaded with any plugin that provides `XglImporter`

Only loading from files is supported.
*/
class MAGNUM_ANYSCENEIMPORTER_EXPORT AnySceneImporter: public AbstractImporter {
    public:
        /** @brief Constructor with access to plugin manager */
        explicit AnySceneImporter(PluginManager::Manager<AbstractImporter>& manager);

        /** @brief Plugin manager constructor */
        explicit AnySceneImporter(PluginManager::AbstractManager& manager, const std::string& plugin);

        ~AnySceneImporter();

    private:
        MAGNUM_ANYSCENEIMPORTER_LOCAL Features doFeatures() const override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL bool doIsOpened() const override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL void doClose() override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL void doOpenFile(const std::string& filename) override;

        MAGNUM_ANYSCENEIMPORTER_LOCAL Int doDefaultScene() override;

        MAGNUM_ANYSCENEIMPORTER_LOCAL UnsignedInt doSceneCount() const override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Int doSceneForName(const std::string& name) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL std::string doSceneName(UnsignedInt id) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Containers::Optional<SceneData> doScene(UnsignedInt id) override;

        MAGNUM_ANYSCENEIMPORTER_LOCAL UnsignedInt doLightCount() const override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Int doLightForName(const std::string& name) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL std::string doLightName(UnsignedInt id) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Containers::Optional<LightData> doLight(UnsignedInt id) override;

        MAGNUM_ANYSCENEIMPORTER_LOCAL UnsignedInt doCameraCount() const override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Int doCameraForName(const std::string& name) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL std::string doCameraName(UnsignedInt id) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Containers::Optional<CameraData> doCamera(UnsignedInt id) override;

        MAGNUM_ANYSCENEIMPORTER_LOCAL UnsignedInt doObject2DCount() const override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Int doObject2DForName(const std::string& name) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL std::string doObject2DName(UnsignedInt id) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL std::unique_ptr<ObjectData2D> doObject2D(UnsignedInt id) override;

        MAGNUM_ANYSCENEIMPORTER_LOCAL UnsignedInt doObject3DCount() const override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Int doObject3DForName(const std::string& name) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL std::string doObject3DName(UnsignedInt id) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL std::unique_ptr<ObjectData3D> doObject3D(UnsignedInt id) override;

        MAGNUM_ANYSCENEIMPORTER_LOCAL UnsignedInt doMesh2DCount() const override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Int doMesh2DForName(const std::string& name) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL std::string doMesh2DName(UnsignedInt id) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Containers::Optional<MeshData2D> doMesh2D(UnsignedInt id) override;

        MAGNUM_ANYSCENEIMPORTER_LOCAL UnsignedInt doMesh3DCount() const override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Int doMesh3DForName(const std::string& name) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL std::string doMesh3DName(UnsignedInt id) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Containers::Optional<MeshData3D> doMesh3D(UnsignedInt id) override;

        MAGNUM_ANYSCENEIMPORTER_LOCAL UnsignedInt doMaterialCount() const override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Int doMaterialForName(const std::string& name) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL std::string doMaterialName(UnsignedInt id) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL std::unique_ptr<AbstractMaterialData> doMaterial(UnsignedInt id) override;

        MAGNUM_ANYSCENEIMPORTER_LOCAL UnsignedInt doTextureCount() const override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Int doTextureForName(const std::string& name) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL std::string doTextureName(UnsignedInt id) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Containers::Optional<TextureData> doTexture(UnsignedInt id) override;

        MAGNUM_ANYSCENEIMPORTER_LOCAL UnsignedInt doImage1DCount() const override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Int doImage1DForName(const std::string& name) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL std::string doImage1DName(UnsignedInt id) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Containers::Optional<ImageData1D> doImage1D(UnsignedInt id) override;

        MAGNUM_ANYSCENEIMPORTER_LOCAL UnsignedInt doImage2DCount() const override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Int doImage2DForName(const std::string& name) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL std::string doImage2DName(UnsignedInt id) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Containers::Optional<ImageData2D> doImage2D(UnsignedInt id) override;

        MAGNUM_ANYSCENEIMPORTER_LOCAL UnsignedInt doImage3DCount() const override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Int doImage3DForName(const std::string& name) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL std::string doImage3DName(UnsignedInt id) override;
        MAGNUM_ANYSCENEIMPORTER_LOCAL Containers::Optional<ImageData3D> doImage3D(UnsignedInt id) override;

        std::unique_ptr<AbstractImporter> _in;
};

}}

#endif
