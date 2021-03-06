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

#include "Model.h"

extern int  yyparse();
extern void yyerror(const char* msg);
extern FILE *yyin;
extern int yydebug;

namespace ospray {
  namespace particle {
    
    inline vec3f makeRandomColor(const int i)
    {
      const int mx = 13*17*43;
      const int my = 11*29;
      const int mz = 7*23*63;
      const uint32 g = (i * (3*5*127)+12312314);
      return vec3f((g % mx)*(1.f/(mx-1)),
                   (g % my)*(1.f/(my-1)),
                   (g % mz)*(1.f/(mz-1)));
    }

    int Model::getAtomType(const std::string &name)
    {
      if (atomTypeByName.find(name) == atomTypeByName.end()) {
        std::cout << "Found atom type '"+name+"'" << std::endl;
        AtomType *a = new AtomType(name);
        a->color = makeRandomColor(atomType.size());
        atomTypeByName[name] = atomType.size();
        atomType.push_back(a);
      }
      return atomTypeByName[name];
    }

    void Model::loadXYZ(const std::string &fileName)
    {
      FILE *file = fopen(fileName.c_str(),"r");
      if (!file) 
        throw std::runtime_error("could not open input file "+fileName);
      int numAtoms;

      // int rc = sscanf(line,"%i",&numAtoms);
      int rc = fscanf(file,"%i\n",&numAtoms);
      PRINT(numAtoms);
      if (rc != 1)
        throw std::runtime_error("could not parse .dat.xyz header in input file "+fileName);
      
      char line[10000]; 
      fgets(line,10000,file); // description line

      std::cout << "#" << fileName << " (.dat.xyz format): expecting " << numAtoms << " atoms" << std::endl;
      for (int i=0;i<numAtoms;i++) {
        char atomName[110];
        Atom a;
        vec3f n;
        if (!fgets(line,10000,file)) {
          std::stringstream ss;
          ss << "in " << fileName << " (line " << (i+2) << "): "
             << "unexpected end of file!?" << std::endl;
          throw std::runtime_error(ss.str());
        }

        rc = sscanf(line,"%100s %f %f %f %f %f %f\n",atomName,
                    &a.position.x,&a.position.y,&a.position.z,
                    &n.x,&n.y,&n.z
                    );
        // rc = fscanf(file,"%100s %f %f %f %f %f %f\n",atomName,
        //             &a.position.x,&a.position.y,&a.position.z,
        //             &n.x,&n.y,&n.z
        //             );
        if (rc != 7 && rc != 4) {
          std::stringstream ss;
          PRINT(rc);
          PRINT(line);
          ss << "in " << fileName << " (line " << (i+2) << "): "
             << "could not parse .dat.xyz data line" << std::endl;
          throw std::runtime_error(ss.str());
        }
        a.type = getAtomType(atomName);
        atom.push_back(a);
      }
    }

    /*! load xyz files in which there is *no* atom count, but just a
        list of "type x y z" lines */
    void Model::loadXYZ2(const std::string &fileName)
    {
      FILE *file = fopen(fileName.c_str(),"r");
      if (!file) 
        throw std::runtime_error("could not open input file "+fileName);
      int numAtoms;

      int rc = 0;
      char atomType[1000];
      vec3f pos;
      while ((rc = fscanf(file,"%s %f %f %f\n",atomType,&pos.x,&pos.y,&pos.z)) == 4) {
        Atom a;
        a.type = getAtomType(atomType);
        a.position = pos;
        atom.push_back(a);
      }
      if (rc != 4)
        std::cout << "#" << fileName << " (.xyz format): file may be truncated" << std::endl;
    }
  
    box3f Model::getBBox() const 
    {
      box3f bbox = embree::empty;
      for (int i=0;i<atom.size();i++)
        bbox.extend(atom[i].position);
      return bbox;
    }

  } // ::ospray::particle
} // ::ospray


