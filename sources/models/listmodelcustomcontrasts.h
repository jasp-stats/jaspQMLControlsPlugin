//
// Copyright (C) 2013-2020 University of Amsterdam
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public
// License along with this program.  If not, see
// <http://www.gnu.org/licenses/>.
//

#ifndef LISTMODELCUSTOMCONTRASTS_H
#define LISTMODELCUSTOMCONTRASTS_H

#include "listmodeltableviewbase.h"

class ListModelFactorLevels;

class ListModelCustomContrasts : public ListModelTableViewBase
{
	Q_OBJECT
	Q_PROPERTY(QString colName		READ colName	WRITE setColName	NOTIFY colNameChanged	)

public:
	explicit ListModelCustomContrasts(TableViewBase * parent);

	int getMaximumColumnWidthInCharacters(size_t columnIndex)	const	override;

	QString			getDefaultColName(size_t index)				const	override;
	QString			getDefaultRowName(size_t index)				const	override { return QString::number(index + 1); }

	void			reset()												override;
	void			setup()												override;
	bool			isEditable(const QModelIndex& index)		const	override	{ return index.column() >= _tableTerms.variables.length(); }
	QString			getItemInputType(const QModelIndex& index)	const	override;
	QString			colName()									const				{ return _colName;	}

	void			getVariablesAndLabels(QStringList& variables, QVector<QVector<QVariant> >& allLabels);

public slots:
	void sourceTermsReset()														override;
	bool sourceLabelsChanged(QString columnName, QMap<QString, QString> = {})	override;
	bool sourceLabelsReordered(QString columnName)								override;
	void sourceColumnsChanged(QStringList columns)								override;
	void scaleFactorChanged();
	void setColName(QString colName);
	void factorsSourceChanged();

signals:
	void colNameChanged(QString colName);

protected:
	int									_variableCount	= 0;
	double								_scaleFactor	= 1;
	ListModelFactorLevels*	_factorsSourceModel;
	QString								_colName;
	QMap<QString, QList<QString> >		_factors;


private:
	void		_resetValuesEtc();
	bool		_labelChanged(const QString& columnName, const QString& originalLabel, const QString& newLabel);
	void		_setFactorsSource(ListModelFactorLevels* factorsSourceModel);
	void		_setFactors();
	void		_loadColumnInfo();
	QStringList	_getVariables();


};

#endif // LISTMODELCUSTOMCONTRASTS_H
