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

#include "listmodelassignedinterface.h"
#include "controls/variableslistbase.h"
#include "analysisform.h"

ListModelAssignedInterface::ListModelAssignedInterface(JASPListControl* listView)
	: ListModelDraggable(listView)
  , _availableModel(nullptr)
{
	_needsSource = false;
}

void ListModelAssignedInterface::refresh()
{
	bool doRefresh = true;
	QList<int> toRemove;
	for (int i = 0; i < rowCount(); i++)
	{
		QString term = data(index(i, 0)).toString();
		if (!isAllowed(term))
			toRemove.push_back(i);
	}

	if (toRemove.count() > 0)
	{
		VariablesListBase* qmlListView = dynamic_cast<VariablesListBase*>(listView());
		if (qmlListView)
		{
			qmlListView->moveItems(toRemove, _availableModel);
			doRefresh = false;
		}
	}

	if (doRefresh)
		ListModelDraggable::refresh();
}

void ListModelAssignedInterface::setAvailableModel(ListModelAvailableInterface *source)
{
	_availableModel = source;
}

int ListModelAssignedInterface::sourceColumnTypeChanged(QString name)
{
	int index = ListModelDraggable::sourceColumnTypeChanged(name);
	VariablesListBase* qmlListView = dynamic_cast<VariablesListBase*>(listView());

	if (qmlListView && index >= 0 && index < int(terms().size()))
	{
		if (!isAllowed(terms().at(size_t(index))))
		{
			QList<int> indexes = {index};
			qmlListView->moveItems(indexes, _availableModel);
			ListModelDraggable::refresh();
		}
		// Force the analysis to be rerun
		emit qmlListView->boundValueChanged(qmlListView);
	}

	return index;
}

bool ListModelAssignedInterface::sourceLabelsChanged(QString columnName, QMap<QString, QString> changedLabels)
{
	bool change = ListModelDraggable::sourceLabelsChanged(columnName, changedLabels);

	if (change && listView() && listView()->form())
		listView()->form()->refreshAnalysis();

	return change;
}

bool ListModelAssignedInterface::sourceLabelsReordered(QString columnName)
{
	bool change = ListModelDraggable::sourceLabelsReordered(columnName);

	if (change && listView() && listView()->form())
		listView()->form()->refreshAnalysis();

	return change;
}

bool ListModelAssignedInterface::checkAllowedTerms(Terms& terms)
{
	if (!_availableModel) return true;

	Terms notAllowedTerms, allowedTerms;
	for (const Term& term : terms)
	{
		if (isAllowed(term))
			allowedTerms.add(term);
		else
			notAllowedTerms.add(term);
	}
	if (notAllowedTerms.size() == 0) return true;

	terms.set(allowedTerms);
	if (!_availableModel->copyTermsWhenDropped())
		_availableModel->addTerms(notAllowedTerms);

	QString notAllowedTermsStr = notAllowedTerms.asQList().join(", ");
	if (notAllowedTerms.size() == 1)
		listView()->addControlWarningTemporary(tr("This variable has been removed, because it is not allowed: %1").arg(notAllowedTermsStr));
	else
		listView()->addControlWarningTemporary(tr("These variables have been removed, because they are not allowed: %1").arg(notAllowedTermsStr));

	return false;
}
