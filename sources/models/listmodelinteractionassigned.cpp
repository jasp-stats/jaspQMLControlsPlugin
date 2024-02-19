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

#include "listmodelinteractionassigned.h"
#include "qutils.h"
#include "listmodeltermsavailable.h"
#include "listmodeltermsassigned.h"
#include "controls/jasplistcontrol.h"

using namespace std;

ListModelInteractionAssigned::ListModelInteractionAssigned(JASPListControl* listView, bool mustContainLowerTerms, bool addInteractionsByDefault)
	: ListModelAssignedInterface(listView), InteractionModel ()
{
	_copyTermsWhenDropped		= true;
	_mustContainLowerTerms		= mustContainLowerTerms;
	_addInteractionsByDefault	= addInteractionsByDefault;
}

void ListModelInteractionAssigned::initTerms(const Terms &terms, const RowControlsValues& allValuesMap, bool reInit)
{
	// Initialization of the terms can be a re-initialization: in this case the interaction terms can be lost
	// So the interaction terms must be kept, and if their components are in the new terms, then add this interaction.
	Terms newTerms = terms;

	if (reInit)
	{
		Terms oldInteractions = interactionTerms();
		for (const Term& oldInteraction : oldInteractions)
		{
			if (oldInteraction.size() > 1)
			{
				bool add = true;
				for (const QString& comp : oldInteraction.components())
				{
					if (!terms.contains(Term(comp)))
						add = false;
				}
				if (add)
					newTerms.add(oldInteraction);
			}
		}
	}
	clearInteractions();
	_addTerms(newTerms, false);
	ListModelAssignedInterface::initTerms(interactionTerms(), allValuesMap, reInit);
}

Terms ListModelInteractionAssigned::filterTerms(const Terms& terms, const QStringList& filters)
{
	Terms result;
	if (filters.contains("noInteraction"))
	{
		result.add(_fixedFactors);
		result.add(_randomFactors);
		result.add(_covariates);
	}
	else
		result = terms;

	return ListModelAssignedInterface::filterTerms(result, filters);
}

void ListModelInteractionAssigned::removeTerms(const QList<int> &indices)
{
	Terms toRemove;

	for (int i : indices)
	{
		int index = i;
		if (index < rowCount())
			toRemove.add(terms().at(size_t(index)));
	}

	removeInteractionTerms(toRemove);

	setTerms();
}

Terms ListModelInteractionAssigned::termsFromIndexes(const QList<int> &indexes) const
{
	Terms result;
	for (int i : indexes)
	{
		int index = i;
		if (index < rowCount())
			result.add(terms().at(size_t(index)));
	}
	
	return result;
}

void ListModelInteractionAssigned::_addTerms(const Terms& terms, bool combineWithExistingTerms)
{
	Terms fixedFactors;
	Terms randomFactors;
	Terms covariates;
	Terms others;
	for (const Term& term : terms)
	{
		QString itemType = getItemType(term);
		if (itemType == "fixedFactors")
		{
			if (!_fixedFactors.contains(term))
				fixedFactors.add(term);
		}
		else if (itemType == "randomFactors")
		{
			if (!_randomFactors.contains(term))
				randomFactors.add(term);
		}
		else if (itemType == "covariates")
		{
			if (!_covariates.contains(term))
				covariates.add(term);
		}
		else
		{
			if (!_interactionTerms.contains(term))
				others.add(term);
		}
	}
			
	if (fixedFactors.size() > 0)
		addFixedFactors(fixedFactors, combineWithExistingTerms);
	
	if (randomFactors.size() > 0)
		addRandomFactors(randomFactors);
	
	if (covariates.size() > 0)
		addCovariates(covariates);
	
	if (others.size() > 0)
		addInteractionTerms(others);
	
}

void ListModelInteractionAssigned::availableTermsResetHandler(Terms termsAdded, Terms termsRemoved)
{
	if (termsAdded.size() > 0 && listView()->addAvailableVariablesToAssigned())
	{
		_addTerms(termsAdded, _addInteractionsByDefault);
		setTerms();
	}
	
	if (termsRemoved.size() > 0)
	{
		removeInteractionTerms(termsRemoved);
		setTerms();
	}
}

QString ListModelInteractionAssigned::getItemType(const Term &term) const
{
	QString type;
	ListModelTermsAvailable* _source = dynamic_cast<ListModelTermsAvailable*>(availableModel());
	if (_source)
	{
		ListModel* model = _source->getSourceModelOfTerm(term);
		if (model)
		{
			type = model->getItemType(term);
			if (type.isEmpty() || type == "variables")
				type = model->name();
		}
	}
	
	return type;
}

Terms ListModelInteractionAssigned::canAddTerms(const Terms& terms) const
{
	Q_UNUSED(terms);

	return terms;
}


Terms ListModelInteractionAssigned::addTerms(const Terms& terms, int , const RowControlsValues&)
{
	if (terms.size() == 0)
		return Terms();
	
	Terms dropped;
	dropped.setSortParent(availableModel()->allTerms());
	dropped.set(terms);

	Terms newTerms = dropped.combineTerms(JASP::CombinationType::CombinationCross);

	_addTerms(newTerms, false);
	setTerms();

	return Terms();
}

void ListModelInteractionAssigned::moveTerms(const QList<int> &indexes, int dropItemIndex)
{
	JASP::DropMode _dropMode = dropMode();
	if (indexes.length() == 0 || _dropMode == JASP::DropMode::DropNone)
		return;

	beginResetModel();
	Terms termsToMove = termsFromIndexes(indexes);
	if (dropItemIndex == -1)
		dropItemIndex = int(terms().size());
	for (int index : indexes)
	{
		if (index < dropItemIndex)
			dropItemIndex--;
	}

	Terms newTerms = _interactionTerms;
	newTerms.remove(termsToMove);
	newTerms.insert(dropItemIndex, termsToMove);
	_interactionTerms = newTerms;
	_setTerms(newTerms);

	endResetModel();
}

void ListModelInteractionAssigned::setTerms()
{	
	beginResetModel();
	
	_setTerms(interactionTerms());
	
	endResetModel();
}

void ListModelInteractionAssigned::sourceNamesChanged(QMap<QString, QString> map)
{
	// In an interaction model, if a name is changed, maybe a part of the interaction term has to be changed.
	QSet<int>				allChangedTermsIndex;
	Terms					oldInteractionTerms = interactionTerms();
	QMapIterator<QString, QString> it(map);

	while (it.hasNext())
	{
		it.next();
		const QString& oldName = it.key(), newName = it.value();

		// changeComponentName changes all interaction terms that have at least one of its components that much be changed.
		QSet<int> indexes = changeComponentName(oldName.toStdString(), newName.toStdString());
		allChangedTermsIndex += indexes;
	}

	// If this model is a source of another model, the namesChanged signal must be also emitted, but with all interaction terms that have been changed.
	QMap<QString, QString>	allTermsChangedMap;
	const Terms& newInteractionTerms = interactionTerms();
	for (int index : allChangedTermsIndex)
		allTermsChangedMap[oldInteractionTerms.at(size_t(index)).asQString()] = newInteractionTerms.at(size_t(index)).asQString();

	if (allTermsChangedMap.size() > 0)
		emit namesChanged(allTermsChangedMap);

	// setTerms will re-initialize the terms of this model, and will also provoke the re-initialization of the models that depend on this model.
	// So setTerms must be called after the namesChanged is emitted, so that the other models which depend on this model can change first the names of their terms.
	setTerms();
}
