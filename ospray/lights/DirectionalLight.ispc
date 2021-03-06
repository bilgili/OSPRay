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

#include "Light.ih"
#include "ospray/math/vec.ih"

struct DirectionalLight {
  uniform Light super;      //!< inherited light fields

  uniform vec3f direction;  //!< direction of emitted light rays
  uniform vec3f radiance;   //!< RGB color and intensity of light
};


// Implementation
//////////////////////////////////////////////////////////////////////////////

varying vec3f DirectionalLight_computeRadiance(const uniform Light *uniform _self,
                                               const varying vec3f &coordinates, 
                                               varying vec3f &direction, 
                                               varying float &distance) 
{
  const DirectionalLight *uniform self = (DirectionalLight *uniform)_self;

  direction = neg(self->direction);
  distance = inf;

  return self->radiance;
}

void DirectionalLight_Constructor(uniform DirectionalLight *uniform self,
                                  void *uniform cppEquivalent,
                                  const uniform vec3f &direction,
                                  const uniform vec3f &radiance)
{
  Light_Constructor(&self->super, cppEquivalent);
  self->direction = direction;
  self->radiance = radiance;
  self->super.computeRadiance = DirectionalLight_computeRadiance;
}


// Exports (called from C++)
//////////////////////////////////////////////////////////////////////////////

//! Create an ispc-side DirectionalLight object
export void *uniform DirectionalLight_create(void *uniform cppEquivalent)
{
  uniform DirectionalLight *uniform self = uniform new uniform DirectionalLight;
  DirectionalLight_Constructor(self, cppEquivalent, make_vec3f(0.f, 0.f, 1.f), make_vec3f(1.f));
  return self;
}

//! Set the parameters of an ispc-side DirectionalLight object
export void DirectionalLight_set(void *uniform _self,
                                 const uniform vec3f &direction,
                                 const uniform vec3f &radiance)
{
  uniform DirectionalLight *uniform self = (uniform DirectionalLight *uniform)_self;
  self->direction = direction;
  self->radiance = radiance;
}
