// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QtQml/QQmlEngineExtensionPlugin>
#include <QQmlEngine>
#include <QQmlContext>
#include "preferencesmodelbase.h"
#include "jasptheme.h"
#include "models/term.h"
#include "jaspcontrol.h"
#include "ALTNavigation/altnavigation.h"
#include "knownissues.h"
#include "simpleDataSetModel.h"
#include <qdebug.h>

//![plugin]
class JASPQMLComponents : public QQmlEngineExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlEngineExtensionInterface_iid)

	void initializeEngine(QQmlEngine *engine, const char *uri) override
	{
		QQmlEngineExtensionPlugin::initializeEngine(engine, uri);

		bool	debug	= false,
				isMac	= false,
				isLinux = false;

	#ifdef JASP_DEBUG
		debug = true;
	#endif

	#ifdef __APPLE__
		isMac = true;
	#endif

	#ifdef __linux__
		isLinux = true;
	#endif

		bool isWindows = !isMac && !isLinux;

		engine->rootContext()->setContextProperty("DEBUG_MODE",				debug);
		engine->rootContext()->setContextProperty("MACOS",					isMac);
		engine->rootContext()->setContextProperty("LINUX",					isLinux);
		engine->rootContext()->setContextProperty("WINDOWS",				isWindows);
		engine->rootContext()->setContextProperty("INTERACTION_SEPARATOR",	Term::separator);

		engine->rootContext()->setContextProperty("preferencesModel",		new PreferencesModelBase()	);
		engine->rootContext()->setContextProperty("jaspTheme",				new JaspTheme()	);


		qmlRegisterUncreatableMetaObject(JASP::staticMetaObject, // static meta object
										 "JASP.Controls",          // import statement
										 0, 1,                         // major and minor version of the import
										 "JASP",                 // name in QML
										 "Error: only enums");
		ALTNavigation::registerQMLTypes("JASP.Controls");

		new KnownIssues();
		new SimpleDataSetModel();
	}
};
//![plugin]

#include "plugin.moc"


