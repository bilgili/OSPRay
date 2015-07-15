// ======================================================================== //
// Copyright 2009-2015 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#include "Volume.h"
#include "sg/common/World.h"
#include "sg/common/Integrator.h"

namespace ospray {
  namespace sg {

    // =======================================================
    // base volume class
    // =======================================================

    /*! \brief returns a std::string with the c++ name of this class */
    std::string Volume::toString() const
    { return "ospray::sg::Volume"; }
    
    // =======================================================
    // structured volume class
    // =======================================================

    //! constructor
    StructuredVolume::StructuredVolume()
      : dimensions(-1), fileName(""), voxelType("<undefined>"), volume(NULL), mappedPointer(NULL)
    {}

    /*! \brief returns a std::string with the c++ name of this class */
    std::string StructuredVolume::toString() const
    { return "ospray::sg::StructuredVolume"; }
    
    //! return bounding box of all primitives
    box3f StructuredVolume::getBounds()
    { return box3f(vec3f(0.f),vec3f(getDimensions())); }

      //! \brief Initialize this node's value from given XML node 
    void StructuredVolume::setFromXML(const xml::Node *const node, const unsigned char *binBasePtr)
    {
      voxelType = node->getProp("voxelType");
      if (node->hasProp("ofs"))
        mappedPointer = binBasePtr + node->getPropl("ofs");
      dimensions = parseVec3i(node->getProp("dimensions"));
      fileName = node->getProp("fileName");
      if (fileName != "") fileNameOfCorrespondingXmlDoc = node->doc->fileName;
      PRINT(fileName);

      if (voxelType != "float") 
        throw std::runtime_error("unkonwn StructuredVolume.voxelType (currently only supporting 'float')");
          
      if (!transferFunction) 
        setTransferFunction(new TransferFunction);

      std::cout << "#osp:sg: created StructuredVolume from XML file, dimensions = " 
                << getDimensions() << std::endl;
    }
    
    /*! \brief 'render' the object to ospray */
    void StructuredVolume::render(RenderContext &ctx)
    {
      if (volume) return;

      PRINT(dimensions);
      PRINT(fileName);

      if (dimensions.x <= 0 || dimensions.y <= 0 || dimensions.z <= 0)
        throw std::runtime_error("StructuredVolume::render(): invalid volume dimensions");
      
      volume = ospNewVolume("block_bricked_volume");
      ospSetString(volume,"voxelType",voxelType.c_str());
      ospSetVec3i(volume,"dimensions",dimensions);
      size_t nPerSlice = dimensions.x*dimensions.y;
      if (fileName != "") {
        PRINT(fileNameOfCorrespondingXmlDoc);
        FileName realFileName = fileNameOfCorrespondingXmlDoc.path()+fileName;
        PRINT(realFileName);
        FILE *file = fopen(realFileName.c_str(),"rb");
        if (!file) 
          throw std::runtime_error("StructuredVolume::render(): could not open file '"
                                   +realFileName.str()+"' (expanded from xml file '"
                                   +fileNameOfCorrespondingXmlDoc.str()
                                   +"' and file name '"+fileName+"')");
        float *slice = new float[nPerSlice];
        for (int z=0;z<dimensions.z;z++) {
          size_t nRead = fread(slice,sizeof(float),nPerSlice,file);
          if (nRead != nPerSlice)
            throw std::runtime_error("StructuredVolume::render(): read incomplete slice data ... partial file or wrong format!?");
          ospSetRegion(volume,slice,vec3i(0,0,z),vec3i(dimensions.x,dimensions.y,1));
        }
        fclose(file);
        delete[] slice;
      } else {
        assert(mappedPointer != NULL);
        float *slice = new float[nPerSlice];
        for (int z=0;z<dimensions.z;z++) {
          memcpy(slice,((unsigned char *)mappedPointer)+z*nPerSlice*sizeof(float),nPerSlice*sizeof(float));
          ospSetRegion(volume,slice,vec3i(0,0,z),vec3i(dimensions.x,dimensions.y,1));
        }
        delete[] slice;
      }
      
      transferFunction->render(ctx);

      // volume = ospNewVolume("block_bricked_volume");
      // ospSetString(volume,"voxelType","float");
      ospSetObject(volume,"transferFunction",transferFunction->getOSPHandle());
      ospCommit(volume);
      ospAddVolume(ctx.world->ospModel,volume);
      // PRINT(volume);
    }

    OSP_REGISTER_SG_NODE(StructuredVolume);





    // =======================================================
    // structured volume class
    // =======================================================

    //! constructor
    StackedRawSlices::StackedRawSlices()
      : dimensions(-1), baseName(""), voxelType("uint8"), volume(NULL)
    {}

    /*! \brief returns a std::string with the c++ name of this class */
    std::string StackedRawSlices::toString() const
    { return "ospray::sg::StackedRawSlices"; }
    
    //! return bounding box of all primitives
    box3f StackedRawSlices::getBounds()
    { return box3f(vec3f(0.f),vec3f(getDimensions())); }

      //! \brief Initialize this node's value from given XML node 
    void StackedRawSlices::setFromXML(const xml::Node *const node, const unsigned char *binBasePtr)
    {
      voxelType = node->getProp("voxelType");
      sliceResolution = parseVec2i(node->getProp("sliceResolution"));
      baseName = node->getProp("baseName");
      firstSliceID = node->getPropl("firstSliceID");
      numSlices = node->getPropl("numSlices");
      if (voxelType != "uint8") 
        throw std::runtime_error("unkonwn StackedRawSlices.voxelType (currently only supporting 'uint8')");
          
      if (!transferFunction) 
        setTransferFunction(new TransferFunction);
    }
    
    /*! \brief 'render' the object to ospray */
    void StackedRawSlices::render(RenderContext &ctx)
    {
      if (volume) return;

      dimensions.x = sliceResolution.x;
      dimensions.y = sliceResolution.y;
      dimensions.z = numSlices;

      if (dimensions.x <= 0 || dimensions.y <= 0 || dimensions.z <= 0)
        throw std::runtime_error("StackedRawSlices::render(): invalid volume dimensions");
      
      volume = ospNewVolume("block_bricked_volume");
      ospSetString(volume,"voxelType",voxelType.c_str());
      ospSetVec3i(volume,"dimensions",dimensions);
      size_t nPerSlice = dimensions.x*dimensions.y;
      uint8 *slice = new uint8[nPerSlice];
      for (int sliceID=0;sliceID<numSlices;sliceID++) {
        char sliceName[strlen(baseName.c_str())+20];
        sprintf(sliceName,baseName.c_str(),firstSliceID+sliceID);
        PRINT(sliceName);
        FILE *file = fopen(sliceName,"rb");
        if (!file) 
          throw std::runtime_error("StackedRawSlices::render(): could not open file '"
                                   +std::string(sliceName)+"'");
        size_t nRead = fread(slice,sizeof(float),nPerSlice,file);
        if (nRead != nPerSlice)
          throw std::runtime_error("StackedRawSlices::render(): read incomplete slice data ... partial file or wrong format!?");
        ospSetRegion(volume,slice,vec3i(0,0,sliceID),vec3i(dimensions.x,dimensions.y,1));
        fclose(file);
      }
      delete[] slice;
      
      transferFunction->render(ctx);
      
      ospSetObject(volume,"transferFunction",transferFunction->getOSPHandle());
      ospCommit(volume);
      ospAddVolume(ctx.world->ospModel,volume);
    }

    OSP_REGISTER_SG_NODE(StackedRawSlices);

  } // ::ospray::sg
} // ::ospray

