#ifndef SIMPLEDATASETMODEL_H
#define SIMPLEDATASETMODEL_H

#include <QAbstractTableModel>
#include "variableinfo.h"
#include "json/json.h"

class SimpleDataSetModel : public QAbstractTableModel, public VariableInfoProvider
{
	Q_OBJECT

public:
	explicit SimpleDataSetModel(QObject* parent = nullptr);

	static SimpleDataSetModel* singleton()	{ return _singleton; }


	int							rowCount(const QModelIndex &parent = QModelIndex())										const override		{ return _rowCount;			}
	int							columnCount(const QModelIndex &parent = QModelIndex())									const override		{ return _columns.size();	}
	QVariant					data(const QModelIndex &index, int role = Qt::DisplayRole)								const override		{ return QVariant();		}

	QVariant					provideInfo(VariableInfo::InfoType info, const QString& colName = "", int row = 0)		const	override;
	QAbstractItemModel*			providerModel()																					override	{ return this;	}

	void						setData(const Json::Value& value);

private:
	static SimpleDataSetModel* _singleton;

	struct SimpleColumnType
	{
		QVector<QVariant>	data;
		QVector<QString>	labels;
		columnType			type;

		SimpleColumnType(const QVector<QVariant>& _data, const QVector<QString>& _labels, const columnType _type) : data(_data), labels(_labels), type(_type) {}
		SimpleColumnType(const QVector<QString>& _labels, const columnType _type) : labels(_labels), type(_type) {}
		SimpleColumnType() {}
	};

	QVector<QString> getNames() const;

	int									_rowCount = 0;
	QMap<QString,  SimpleColumnType>	_columns;
};


#endif //SIMPLEDATASETMODEL_H
