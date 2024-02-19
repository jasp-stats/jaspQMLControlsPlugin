//
// Copyright (C) 2013-2018 University of Amsterdam
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

#include "log.h"
#include <QSize>
#include <fstream>
#include "listmodeltableviewbase.h"
#include "analysisform.h"
#include "qutils.h"
#include "controls/tableviewbase.h"
#include "controls/textinputbase.h"
#include "utilities/desktopcommunicator.h"

using namespace std;

ListModelTableViewBase::ListModelTableViewBase(TableViewBase * tableView)
	: ListModel(tableView), _tableView(tableView)
{
	connect(DesktopCommunicator::singleton(),	&DesktopCommunicator::uiScaleChanged,	this,	&ListModelTableViewBase::refresh);
}

QVariant ListModelTableViewBase::data(const QModelIndex &index, int role) const
{
	int		column	= index.column(),
			row		= index.row();

	if (column < 0 || column >= columnCount() || row < 0 || row >= rowCount())
		return QVariant();

	switch(role)
	{
	case int(specialRoles::lines):
	{
		bool	belowMeIsActive = row < rowCount() - 1,
				up				= true,
				left			= true,
				down			= !belowMeIsActive,
				right			= column == columnCount() - 1; //always draw left line and right line only if last col

		return	(left ?		1 : 0) +
				(right ?	2 : 0) +
				(up ?		4 : 0) +
				(down ?		8 : 0);
	}		
	case int(specialRoles::itemInputType):	return getItemInputType(index);
	case Qt::DisplayRole:					return QVariant(_tableTerms.values[column][row]);
	default:								return ListModel::data(index, role);
	}
}


int ListModelTableViewBase::getMaximumColumnWidthInCharacters(size_t columnIndex) const
{
	int maxL = 3;
	int column = int(columnIndex);

	if (column < _tableTerms.values.size())
		for (QVariant val : _tableTerms.values[column])
            maxL = std::max(static_cast<int>(val.toString().size()), maxL);

	return maxL + 3;
}


QString ListModelTableViewBase::getMaximumRowHeaderString() const
{
	int maxL = 7;

	for (QString val : _tableTerms.rowNames)
            maxL = std::max(static_cast<int>(val.size() + 2), maxL);

	QString dummyText;
	while (maxL > dummyText.length())
		dummyText += "X";

	return dummyText;
}

void ListModelTableViewBase::addColumn(bool emitStuff)
{
	if (emitStuff)
		beginResetModel();

	size_t count = size_t(columnCount());

	if (count < _maxColumn)
	{
		_tableTerms.colNames.push_back(getDefaultColName(count));
		QVector<QVariant> values;
		for (int rowIndex = 0; rowIndex < _tableTerms.rowNames.length(); rowIndex++)
			values.push_back(_tableView->defaultValue(count, rowIndex));
		_tableTerms.values.push_back(values);
	}

	if (emitStuff)
	{
		endResetModel();

		emit columnCountChanged();
	}
}

void ListModelTableViewBase::removeColumn(size_t col, bool emitStuff)
{
	if (emitStuff)
		beginResetModel();
	int colIndex = int(col);

	if (colIndex < columnCount())
	{
		_tableTerms.values.removeAt(colIndex);
		_tableTerms.colNames.pop_back();
	}

	if (emitStuff)
	{
		endResetModel();

		emit columnCountChanged();
	}
}

void ListModelTableViewBase::addRow(bool emitStuff)
{
	if (emitStuff)
		beginResetModel();

	if (rowCount() < int(_maxRow))
	{
		_tableTerms.rowNames.push_back(getDefaultRowName(rowCount()));
		int colIndex = 0;
		for (QVector<QVariant> & value : _tableTerms.values)
		{
			while (value.size() < _tableTerms.rowNames.size()) //Lets make sure the data is rectangular!
				value.push_back(_tableView->defaultValue(colIndex, value.length()));
			colIndex++;
		}
	}

	if (emitStuff)
	{
		endResetModel();

		emit rowCountChanged();
	}
}

void ListModelTableViewBase::removeRow(size_t row, bool emitStuff)
{
	if (emitStuff)
		beginResetModel();

	if (row < rowCount())
	{
		for (QVector<QVariant> & value : _tableTerms.values)
			value.removeAt(int(row));
		_tableTerms.rowNames.pop_back(); //Should we remove the exact right rowName? Or I guess there just generated row for row in the base..
	}

	if (emitStuff)
	{
		endResetModel();

		emit rowCountChanged();
	}
}

void ListModelTableViewBase::setSize(int rows, int columns, bool emitStuff)
{
	if (emitStuff)
		beginResetModel();

	bool rowsChanged = false;
	if (rows > -1)
	{
		if (rows < rowCount())
		{
			for (QVector<QVariant> & value : _tableTerms.values)
				value.erase(value.begin() + rows, value.end());
			_tableTerms.rowNames.erase(_tableTerms.rowNames.begin() + rows, _tableTerms.rowNames.end());

			rowsChanged = true;
		}
		else if (rows > rowCount())
		{
			size_t oldRowCount = rowCount();
			for (int i = 0; i < rows - oldRowCount; i++)
			{
				_tableTerms.rowNames.push_back(getDefaultRowName(rowCount()));
				int colIndex = 0;
				for (QVector<QVariant> & value : _tableTerms.values)
				{
					while (value.size() < _tableTerms.rowNames.size())
						value.push_back(_tableView->defaultValue(colIndex, value.length()));
					colIndex++;
				}
			}

			rowsChanged = true;
		}
	}

	bool columnsChanged = false;
	if (columns > -1)
	{
		if (columns < columnCount())
		{
			_tableTerms.values.erase(_tableTerms.values.begin() + columns, _tableTerms.values.end());
			_tableTerms.colNames.erase(_tableTerms.colNames.begin() + columns, _tableTerms.colNames.end());

			columnsChanged = true;
		}
		else if (columns > columnCount())
		{
			size_t oldColumnCount = columnCount();
			for (int i = 0; i < columns - oldColumnCount; i++)
			{
				_tableTerms.colNames.push_back(getDefaultColName(columnCount()));
				QVector<QVariant> values;
				for (int rowIndex = 0; rowIndex < _tableTerms.rowNames.length(); rowIndex++)
					values.push_back(_tableView->defaultValue(columnCount(), rowIndex));
				_tableTerms.values.push_back(values);
			}

			columnsChanged = true;
		}
	}

	if (emitStuff)
	{
		endResetModel();

		if (columnsChanged)
			emit columnCountChanged();

		if (rowsChanged)
			emit rowCountChanged();
	}
}

void ListModelTableViewBase::reset()
{
	beginResetModel();

	if (!_keepColsOnReset)	_tableTerms.colNames.clear();
	if (!_keepRowsOnReset)	_tableTerms.rowNames.clear();

	_tableTerms.values.clear();

	if (!_keepColsOnReset)
		for (int col=0; col < _tableView->initialColumnCount(); col++)
			addColumn(false);

    int rows = std::max(static_cast<int>(_tableTerms.rowNames.length()), _tableView->initialRowCount());

	if (!_keepRowsOnReset)
		for (int row=0; row < rows; row++)
			addRow(false);

	endResetModel();

	emit columnCountChanged();
	emit rowCountChanged();
}

void ListModelTableViewBase::itemChanged(int column, int row, QVariant value, QString type)
{
	//If you change this function, also take a look at ListModelFilteredDataEntry::itemChanged
	if (column > -1 && column < columnCount() && row > -1 && row < rowCount())
	{
		if (_tableTerms.values[column][row] != value)
		{
			JASP::ItemType itemType = _tableView->itemTypePerItem(column, row);
			_tableTerms.values[column][row] = itemType == JASP::ItemType::Integer ? value.toInt() : itemType == JASP::ItemType::Double ? value.toDouble() : value;

		if (type != "formula") // For formula type, wait for the formulaCheckSucceeded signal before emitting modelChanged
			emit termsChanged();

			// Here we should *actually* check if specialRoles::maxColString changes and in that case: (so that the view can recalculate stuff)
			//	emit headerDataChanged(Qt::Orientation::Horizontal, column, column);
		}
	}
}

Terms ListModelTableViewBase::filterTerms(const Terms& terms, const QStringList& filters)
{
	Terms tempTerms;
	QStringList otherFilters;

	std::set<int> colNbs;
	if (filters.empty() && _tableTerms.values.length() == 1)
		colNbs.insert(0);

	for (const QString& filter : filters)
	{
		int colNb = _tableTerms.colNames.indexOf(filter);
		if (colNb == -1 && filter.startsWith("column"))
		{
			QString tempWhat = filter;
			QString colNbStr = tempWhat.remove("column");
			bool ok = false;
			colNb = colNbStr.toInt(&ok);
			if (!ok) colNb = -1;
			if (colNb > 0) colNb--;
		}

		if (colNb != -1)	colNbs.insert(colNb);
		else				otherFilters.append(filter);
	}

	for (int colNb : colNbs)
	{
		if (_tableTerms.values.length() > colNb)
		{
			const QVector<QVariant>& values = _tableTerms.values[colNb];
			for (const QVariant& val : values)
			{
				QString value = val.toString();
				if (!value.isEmpty() && value != "...")
					tempTerms.add(val.toString());
			}
		}
		else
			addControlError(tr("Column number in source is bigger than the number of columns of %1").arg(name()));
	}

	return ListModel::filterTerms(tempTerms, otherFilters);
}

QVariant ListModelTableViewBase::headerData( int section, Qt::Orientation orientation, int role) const
{
	if (role == int(specialRoles::maxRowHeaderString))	return getMaximumRowHeaderString();

	if (section < 0 || section >= (orientation == Qt::Horizontal ? _tableTerms.colNames.length() : _tableTerms.rowNames.length()))
		return QVariant();

	switch(role)
	{
	case int(specialRoles::maxColString): //A query from DataSetView for the maximumlength string to be expected! This to accomodate columnwidth
	{
		QString dummyText	= headerData(section, orientation, Qt::DisplayRole).toString() + "XXXXX";
		int colWidth		= getMaximumColumnWidthInCharacters(size_t(section));

		while (colWidth > dummyText.length())
			dummyText += "X";

		return dummyText;
	}
	case Qt::DisplayRole:						return QVariant(orientation == Qt::Horizontal ? _tableTerms.colNames[section] : _tableTerms.rowNames[section]);
	case Qt::TextAlignmentRole:					return QVariant(Qt::AlignCenter);
	default:									return QVariant();
	}
}

void ListModelTableViewBase::sourceTermsReset()
{
	beginResetModel();

	QMap<QString, QVector<QVariant>> tempStore;

	for (int row = 0; row < rowCount(); row++)
		for (int col = 0; col < columnCount(); col++)
			tempStore[_tableTerms.rowNames[row]].push_back(_tableTerms.values[col][row]);

	_tableTerms.values.clear();
	_tableTerms.rowNames = getSourceTerms().asQList();
	if (_tableTerms.colNames.size() == 0)
		_tableTerms.colNames.push_back(getDefaultColName(0));

	for (int col = 0; col < columnCount(); col++)
	{
		QVector<QVariant> newValues;
		for (int row = 0; row < rowCount(); row++)
			newValues.push_back(_tableView->defaultValue(col, row));
		_tableTerms.values.push_back(newValues);

		for (int row = 0; row < rowCount(); row++)
			if (tempStore.contains(_tableTerms.rowNames[row]) && tempStore[_tableTerms.rowNames[row]].size() > col)
				_tableTerms.values[col][row] = tempStore[_tableTerms.rowNames[row]][col];
	}

	endResetModel();

	emit columnCountChanged();
	emit rowCountChanged();
}

QHash<int, QByteArray> ListModelTableViewBase::roleNames() const
{
	static QHash<int, QByteArray> roles = ListModel::roleNames();

	static bool addRoles = true;

	if (addRoles)
	{
		roles[int(specialRoles::active)]				= QString("active").toUtf8();
		roles[int(specialRoles::lines)]					= QString("lines").toUtf8();
		roles[int(specialRoles::maxColString)]			= QString("maxColString").toUtf8();
		roles[int(specialRoles::maxRowHeaderString)]	= QString("maxRowHeaderString").toUtf8();
		roles[int(specialRoles::itemInputType)]			= QString("itemInputType").toUtf8();
		addRoles = false;
	}

	return roles;
}

Qt::ItemFlags ListModelTableViewBase::flags(const QModelIndex &index) const
{
	Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;

	if (isEditable(index))
		flags |= Qt::ItemIsEditable;

	return flags;
}

void ListModelTableViewBase::runRScript(const QString & script)
{
	_tableView->runRScript(script);
}

bool ListModelTableViewBase::valueOk(QVariant value, int col, int row)
{
	bool	ok	= true;
	JASP::ItemType itemType = _tableView->itemTypePerItem(col, row);

	if		(itemType == JASP::ItemType::Double)		value.toDouble(&ok);
	else if	(itemType == JASP::ItemType::Integer)	value.toInt(&ok);

	return ok;
}

JASPControl *ListModelTableViewBase::getRowControl(const QString &key, const QString &name) const
{
	if (_itemControls.contains(key))	return _itemControls[key][name];
	else								return nullptr;
}

bool ListModelTableViewBase::addRowControl(const QString &key, JASPControl *control)
{
	_itemControls[key][control->name()] = control;

	if (control->controlType() == JASPControl::ControlType::TextField)
	{
		TextInputBase* textInput = dynamic_cast<TextInputBase*>(control);
		if (textInput && textInput->inputType() == TextInputBase::TextInputType::FormulaType)
			connect(textInput, &TextInputBase::formulaCheckSucceeded, this, &ListModelTableViewBase::formulaCheckSucceededSlot);
	}

	return true;
}

void ListModelTableViewBase::formulaCheckSucceededSlot()
{
	_tableView->resetBoundValue();
}


void ListModelTableViewBase::initTableTerms(const TableTerms& terms)
{
	beginResetModel();

	_tableTerms = terms;

	for (auto & col : _tableTerms.values)
		if(_tableTerms.rowNames.size() < col.size())
		{
			Log::log() << "Too many rows in a column of OptionsTable for ListModelTableViewBase! Shrinking column to fit." << std::endl;
			col.resize(_tableTerms.rowNames.size());
		}
		else
			for (int row = col.size(); row < _tableTerms.rowNames.size(); row++)
				col.push_back(1);

	endResetModel();

	emit columnCountChanged();
	emit rowCountChanged();
}

QString ListModelTableViewBase::getDefaultColName(size_t index) const
{
	return listView()->property("colName").toString() + " " + QString::number(index + 1);
}

QString ListModelTableViewBase::getItemInputType(const QModelIndex &index) const
{
	JASP::ItemType itemType = _tableView->itemTypePerItem(index.column(), index.row());

	if (itemType == JASP::ItemType::Double)			return "double";
	else if (itemType == JASP::ItemType::Integer)	return "integer";
	else													return "string";
}
