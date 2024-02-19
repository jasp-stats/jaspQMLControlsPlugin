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

#include "boundcontrolmeasurescells.h"
#include "models/listmodelmeasurescellsassigned.h"
#include "models/listmodelfactorlevels.h"
#include "controls/jasplistcontrol.h"
#include "analysisform.h"
#include "qutils.h"

#include <QTimer>

using namespace std;


BoundControlMeasuresCells::BoundControlMeasuresCells(ListModelMeasuresCellsAssigned* model) : BoundControlBase(model->listView())
{
	_measuresCellsModel = model;
}

void BoundControlMeasuresCells::bindTo(const Json::Value &value)
{
	BoundControlBase::bindTo(value);

	Terms variables;

	for (const Json::Value& variable : value)
		variables.add(variable.asString());
	_measuresCellsModel->initLevels(getLevels(), variables, true);
}

Terms BoundControlMeasuresCells::getLevels() const
{
	Terms levels;
	for (ListModelFactorLevels* factorsModel : _sourceFactorsModels)
		levels.add(factorsModel->getLevels());
	
	return levels;
}

Json::Value BoundControlMeasuresCells::createJson() const
{
	Json::Value result(Json::arrayValue);
	size_t nbLevels = getLevels().size();

	for (int i = 0; i < nbLevels; i++)
		result.append("");
	
	return result;
}

bool BoundControlMeasuresCells::isJsonValid(const Json::Value &optionValue) const
{
	return optionValue.type() == Json::arrayValue;
}

void BoundControlMeasuresCells::addFactorModel(ListModelFactorLevels *factorModel)
{
	_sourceFactorsModels.push_back(factorModel);
}

void BoundControlMeasuresCells::resetBoundValue()
{
	Json::Value boundValue(Json::arrayValue);
	const Terms& terms = _measuresCellsModel->terms();
	
	for (const Term& term : terms)
		boundValue.append(term.asString());

	setBoundValue(boundValue);
}
