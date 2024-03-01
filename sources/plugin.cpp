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
#include <qdebug.h>
#include "jaspdoublevalidator.h"
#include "formulabase.h"
#include "knownissues.h"


//![plugin]
class JASPQmlPlugin : public QQmlEngineExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlEngineExtensionInterface_iid)


	void initializeEngine(QQmlEngine *engine, const char *uri) override
	{
		QQmlEngineExtensionPlugin::initializeEngine(engine, uri);

		QLocale::setDefault(QLocale(QLocale::English)); // make decimal points == .

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

		if (engine->rootContext()->contextProperty("preferencesModel").isNull())
			engine->rootContext()->setContextProperty("preferencesModel",		new PreferencesModelBase()	);
		if (engine->rootContext()->contextProperty("jaspTheme").isNull())
			engine->rootContext()->setContextProperty("jaspTheme",				new JaspTheme()	);

		qmlRegisterUncreatableMetaObject(JASP::staticMetaObject, // static meta object
										 "JASP.Controls",        // import statement
										 0, 1,                   // major and minor version of the import
										 "JASP",                 // name in QML
										 "Error: only enums");

		ALTNavigation::registerQMLTypes("JASP.Controls");
		// TODO: ALTNavControl::ctrl()->enableAlTNavigation(_preferences->ALTNavModeActive());

		qmlRegisterType<JASPDoubleValidator>						("JASP.Controls",		1, 0, "JASPDoubleValidator"				);
		qmlRegisterType<FormulaBase>								("JASP.Controls",		1, 0, "Formula"							);

		if (!KnownIssues::issues())
			new KnownIssues(this);
	}
};
//![plugin]

#include "plugin.moc"


