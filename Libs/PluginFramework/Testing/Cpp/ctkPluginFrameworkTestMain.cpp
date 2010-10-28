/*=============================================================================

  Library: CTK

  Copyright (c) 2010 German Cancer Research Center,
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

#include <QCoreApplication>
#include <QDirIterator>
#include <QTest>
#include <QThread>

#include <ctkPluginConstants.h>
#include <ctkPluginFrameworkFactory.h>
#include <ctkPluginFramework.h>
#include <ctkPluginException.h>
#include <ctkServiceReference.h>

class TestRunner : public QThread
{
public:

  TestRunner(ctkPluginContext* context, int argc, char** argv)
    : context(context), argc(argc), argv(argv)
  {

  }

  void run()
  {
    QList<ctkServiceReference> refs = context->getServiceReferences("ctkTestSuiteInterface");

    int result = 0;
    foreach(ctkServiceReference ref, refs)
    {
      result = QTest::qExec(context->getService(ref), argc, argv);
    }

    QCoreApplication::exit(result);
  }

private:

  ctkPluginContext* context;
  int argc;
  char** argv;
};

int main(int argc, char** argv)
{
  QCoreApplication app(argc, argv);

  app.setOrganizationName("CTK");
  app.setOrganizationDomain("commontk.org");
  app.setApplicationName("ctkPluginFrameworkCppTests");

  QString pluginDir;
#ifdef CMAKE_INTDIR
  pluginDir = qApp->applicationDirPath() + "/../test_plugins/" CMAKE_INTDIR "/";
#else
  pluginDir = qApp->applicationDirPath() + "/test_plugins/";
#endif

  QApplication::addLibraryPath(pluginDir);

  ctkProperties fwProps;
  fwProps.insert(ctkPluginConstants::FRAMEWORK_STORAGE_CLEAN, ctkPluginConstants::FRAMEWORK_STORAGE_CLEAN_ONFIRSTINIT);
  fwProps.insert("pluginfw.testDir", pluginDir);
  ctkPluginFrameworkFactory fwFactory(fwProps);
  ctkPluginFramework* framework = fwFactory.getFramework();
  framework->start();

  ctkPluginContext* context = framework->getPluginContext();

  ctkPlugin* fwTest = 0;
  QStringList libFilter;
  libFilter << "*.dll" << "*.so" << "*.dylib";
  QDirIterator dirIter(pluginDir, libFilter, QDir::Files);
  while(dirIter.hasNext())
  {
    dirIter.next();
    if (dirIter.fileName().contains("org_commontk_pluginfwtest"))
    {
      try
      {
        fwTest = context->installPlugin(QUrl::fromLocalFile(dirIter.filePath()).toString());
        break;
      }
      catch (const ctkPluginException& e)
      {
        qCritical() << e.what();
      }
    }
  }

  if (!fwTest)
  {
    qCritical() << "Could not find the plugin framework test plugin: org.commontk.pluginfwtest";
  }

  // start the main test plugin which registers the test suites (QObject classes)
  fwTest->start();

//  QList<ctkServiceReference> refs = context->getServiceReferences("ctkTestSuiteInterface");

//  int result = 0;
//  foreach(ctkServiceReference ref, refs)
//  {
//    result = QTest::qExec(context->getService(ref), argc, argv);
//  }

//  return result;

  TestRunner runner(context, argc,argv);
  runner.start();

  return app.exec();
}
