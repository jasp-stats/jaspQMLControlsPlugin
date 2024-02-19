//
// Copyright (C) 2013-2021 University of Amsterdam
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

#include "listmodelgridinput.h"
#include "controls/tableviewbase.h"

ListModelGridInput::ListModelGridInput(TableViewBase *parent) : ListModelTableViewBase(parent)
{

}

void ListModelGridInput::setup()
{
	ListModelTableViewBase::setup();
	_readSource();
}

void ListModelGridInput::initTableTerms(const ListModelTableViewBase::TableTerms &terms)
{
	_rowCount = 0;
	for (const auto & column : terms.values)
		if (column.length() > _rowCount)		_rowCount = column.length();

	ListModelTableViewBase::initTableTerms(terms);
}

void ListModelGridInput::sourceTermsReset()
{
	_readSource();
}

void ListModelGridInput::_readSource()
{
	Terms terms = getSourceTerms();
	int nbColumns	= _tableView->minColumn(),
		nbRows		= _tableView->minRow();

	for (const Term& term : terms)
	{
		const QStringList& row = term.components();
		if (row.length() > nbColumns)											nbColumns = row.length();
	}
	if (int(terms.size()) > nbRows)												nbRows = int(terms.size());
	if (_tableView->maxRow() >= 0 && nbRows > _tableView->maxRow())				nbRows = _tableView->maxRow();
	if (_tableView->maxColumn() >= 0 && nbColumns > _tableView->maxColumn())	nbColumns = _tableView->maxColumn();

	if (nbRows == _rowCount && nbColumns == _tableTerms.colNames.length())
	{
		// Apparently only some valuee has been changed. Just emit a dataChanged in this case
		for (int colNb = 0; colNb < nbColumns; colNb++)
		{
			QVector<QVariant> column;
			for (int rowNb = 0; rowNb < nbRows; rowNb++)
			{
				const Term& term = int(terms.size()) > rowNb ? terms.at(size_t(rowNb)) : Term(QStringList());
				const QStringList& rowValues = term.components();
				QVariant value = rowValues.length() > colNb ? rowValues.at(colNb) : _tableView->defaultValue();
				if (value != _tableTerms.values[colNb][rowNb])
				{
					_tableTerms.values[colNb][rowNb] = value;
					QModelIndex ind = index(rowNb, colNb);
					emit dataChanged(ind, ind, {Qt::DisplayRole});
				}
			}
		}
	}
	else
	{
		beginResetModel();

		_tableTerms.clear();

		for (int colNb = 0; colNb < nbColumns; colNb++)
		{
			QVector<QVariant> column;
			for (size_t rowNb = 0; rowNb < size_t(nbRows); rowNb++)
			{
				const Term& term = terms.size() > rowNb ? terms.at(rowNb) : Term(QStringList());
				const QStringList& rowValues = term.components();
				column.append(rowValues.length() > colNb ? rowValues.at(colNb) : _tableView->defaultValue());
			}
			_tableTerms.values.append(column);
			_tableTerms.colNames.append(QString::number(colNb));
		}

		for (int i = 0; i < _rowCount; i++)
			_tableTerms.rowNames.append(_tableView->rowNames().count() > i ? _tableView->rowNames()[i] : QString::number(i));
		_rowCount = nbRows;

		endResetModel();
	}
}
