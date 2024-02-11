#include "simpleDataSetModel.h"
#include "qutils.h"
#include <QQmlEngine>

SimpleDataSetModel* SimpleDataSetModel::_singleton = nullptr;

SimpleDataSetModel::SimpleDataSetModel(QObject *parent) : QAbstractTableModel(parent)
{
	_engine = qobject_cast<QQmlEngine*>(parent);
	new VariableInfo(this);
	_singleton = this;
}

QVariant SimpleDataSetModel::provideInfo(VariableInfo::InfoType info, const QString& colName, int row) const
{
	try
	{
		switch(info)
		{
		case VariableInfo::VariableType:				return	int(_columns[colName].type);
		case VariableInfo::VariableTypeName:			return	"";
		case VariableInfo::VariableTypeIcon:			return	VariableInfo::getIconFile(_columns[colName].type, VariableInfo::DefaultIconType);
		case VariableInfo::VariableTypeDisabledIcon:	return	VariableInfo::getIconFile(_columns[colName].type, VariableInfo::DisabledIconType);
		case VariableInfo::VariableTypeInactiveIcon:	return	VariableInfo::getIconFile(_columns[colName].type, VariableInfo::InactiveIconType);
		case VariableInfo::Labels:						return	_columns[colName].labels;
		case VariableInfo::DoubleValues:				return	_columns[colName].data;
		case VariableInfo::NameRole:					return	Qt::DisplayRole;
		case VariableInfo::DataSetRowCount:				return  _rowCount;
		case VariableInfo::DataSetValue:				return	"";
		case VariableInfo::MaxWidth:					return	100;
		case VariableInfo::SignalsBlocked:				return	false;
		case VariableInfo::VariableNames:				return	_columns.keys();
		case VariableInfo::DataAvailable:				return	true;

		default: break;
		}
	}
	catch(std::exception & e)
	{
		throw e;
	}
	return QVariant("");
}

QQmlContext *SimpleDataSetModel::providerQMLContext() const
{
	return _engine->rootContext();
}

void SimpleDataSetModel::setData(const Json::Value &value)
{
	if (!value.isArray()) return;

	for (const Json::Value& column : value)
	{
		if (!column.isObject() && !column.isMember("name")) continue;

		QString name = tq(column["name"].asString());
		columnType type = columnType::unknown;
		if (column.isMember("type"))
			type = columnTypeFromString(column["type"].asString());

		QVector<QString> labels;
		if (column.isMember("labels"))
			for (const Json::Value& label : column["labels"])
				labels.push_back(tq(label.asString()));

		if (column.isMember("count"))
			_rowCount = column["count"].asInt();

		_columns.insert(name, SimpleColumnType(labels, type));
	}
}
