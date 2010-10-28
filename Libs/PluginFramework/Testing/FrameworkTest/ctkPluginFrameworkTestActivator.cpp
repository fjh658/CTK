/*=============================================================================

  Library: CTK

  Copyright (c) German Cancer Research Center,
    Division of Medical and Biological Informatics

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=============================================================================*/

#include "ctkPluginFrameworkTestActivator_p.h"

#include "ctkPluginFrameworkTestSuite_p.h"

#include <ctkPluginContext.h>
#include <ctkPluginConstants.h>

#include <QtPlugin>
#include <QStringList>

void ctkPluginFrameworkTestActivator::start(ctkPluginContext* context)
{
  ServiceProperties props;

  frameworkTestSuite = new ctkPluginFrameworkTestSuite(context);
  props.clear();
  props.insert(ctkPluginConstants::SERVICE_PID, frameworkTestSuite->metaObject()->className());
  context->registerService(QStringList("ctkTestSuiteInterface"), frameworkTestSuite, props);
}

void ctkPluginFrameworkTestActivator::stop(ctkPluginContext* context)
{
  Q_UNUSED(context);

  delete frameworkTestSuite;
}

Q_EXPORT_PLUGIN2(org_commontk_pluginfwtest, ctkPluginFrameworkTestActivator)
