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

#ifndef FACTORSFORMBASE_H
#define FACTORSFORMBASE_H

#include "jasplistcontrol.h"
#include "boundcontrols/boundcontrolbase.h"
#include "models/listmodelfactorsform.h"


class FactorsFormBase :  public JASPListControl, public BoundControlBase
{
	Q_OBJECT
	QML_ELEMENT

	Q_PROPERTY( int		initNumberFactors		READ initNumberFactors		WRITE setInitNumberFactors	NOTIFY initNumberFactorsChanged	)
	Q_PROPERTY( int		countVariables			READ countVariables										NOTIFY countVariablesChanged	)


public:
	FactorsFormBase(QQuickItem* parent = nullptr);

	bool			isJsonValid(const Json::Value& optionValue)	const	override;
	Json::Value		createJson()								const	override;
	void			bindTo(const Json::Value& value)					override;
	ListModel*		model()										const	override	{ return _factorsModel; }
	void			setUpModel()										override;

	Q_INVOKABLE	void	addFactor()													{ _factorsModel->addFactor();						}
	Q_INVOKABLE void	removeFactor()												{ _factorsModel->removeFactor();					}
	Q_INVOKABLE void	titleChanged(int index, QString title)						{ _factorsModel->titleChangedSlot(index, title);	}
	Q_INVOKABLE void	factorAdded(int index, QVariant item);

	int					initNumberFactors()						const				{ return _initNumberFactors;						}
	int					countVariables()						const				{ return _initialized ? _factorsModel->countVariables() : 0; }
	JASPListControl*	availableVariablesList()				const				{ return _availableVariablesListItem;				}

signals:
	void initNumberFactorsChanged();
	void countVariablesChanged();

protected slots:
	void			termsChangedHandler() override;

protected:
	GENERIC_SET_FUNCTION(InitNumberFactors,	_initNumberFactors,		initNumberFactorsChanged,	int	)

private:
	ListModelFactorsForm*	_factorsModel				= nullptr;
	QString					_availableVariablesListName;
	JASPListControl*		_availableVariablesListItem	= nullptr;
	int						_initNumberFactors			= 1;
};

#endif // FACTORSFORMBASE_H
