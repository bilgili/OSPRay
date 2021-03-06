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

#pragma once

#include <ospray/ospray.h>

#include "SliceWidget.h"
#include <QtGui>
#include <vector>
#include <string>

class SliceEditor : public QWidget
{
Q_OBJECT

public:

  SliceEditor(osp::box3f boundingBox);

signals:

  void slicesChanged(std::vector<SliceParameters> sliceParameters);

public slots:

  void addSlice(std::string filename = std::string());
  void apply();
  void deleteSlice(SliceWidget *sliceWidget);

protected:

  //! Bounding box of the volume.
  osp::box3f boundingBox;

  //! UI elements.
  QVBoxLayout layout;
  std::vector<SliceWidget *> sliceWidgets;

};
