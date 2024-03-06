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

import QtQuick

Item
{
	property var	model
	property int	itemHorizontalPadding:			0
	property int	itemVerticalPadding:			0
	property int	rowNumberWidth:					0
	property bool	cacheItems:						false
	property double viewportX:						0
	property double viewportY:						0
	property double viewportW:						0
	property double viewportH:						0
	property var	columnHeaderDelegate
	property var	rowNumberDelegate
	property var	itemDelegate
	property var	leftTopCornerItem
	property var	tableViewItem

	// Context properties
	property string	itemInputType
	property bool	itemEditable:					false
	property int	rowIndex:						0
	property int	columnIndex:					0
	property string	itemText
	property string	headerText



	function getColumnHeader(col)			{ return nullptr;	}
	function getRowHeader(row)				{ return nullptr;	}
}
