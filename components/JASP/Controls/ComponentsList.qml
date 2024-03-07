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
import QtQuick.Controls as QtControls
import JASP.Controls
import QtQuick.Layouts

ComponentsListBase
{
	id						: componentsList
	background				: itemRectangle
	implicitWidth 			: parent.width
	implicitHeight			: itemTitles.height + itemTitle.height + itemGrid.height + 2 * jaspTheme.contentMargin + (showAddIcon ? addIconItem.height : 0)
	shouldStealHover		: false
	innerControl			: itemGrid
	addItemManually			: !source && !rSource

	property alias	label				: componentsList.title
	property alias	columns				: itemGrid.columns
	property alias	rows				: itemGrid.rows
	property alias	itemGrid			: itemGrid
	property alias	itemRectangle		: itemRectangle
	property alias	itemScrollbar		: itemScrollbar
	property alias	itemTitle			: itemTitle
	property alias	itemTitles			: itemTitles
	property alias	rowSpacing			: itemGrid.rowSpacing
	property alias	columnSpacing		: itemGrid.columnSpacing
	property alias	addIconItem			: addIconItem
	property bool	showAddIcon			: addItemManually
	property string	removeIcon			: "cross.png"
	property string	addIcon				: "duplicate.png"
	property string addTooltip			: qsTr("Add a row")
	property string removeTooltip		: qsTr("Remove a row")
    property bool   addBorder           : true
	property var	titles				: []

	Item
	{
		id				: itemTitles
		anchors.top		: parent.top
		anchors.left	: parent.left
		height			: titles ? jaspTheme.variablesListTitle : 0
		width			: parent.width
		visible			: !title && titles.length > 0

		Repeater
		{
			id: rep
			model: titles
			Text
			{
				property int defaultOffset: index === 0 ? 0 : rep.itemAt(index-1).x + rep.itemAt(index-1).width + jaspTheme.contentMargin

				x		: (addBorder ? jaspTheme.contentMargin : 0) + (componentsList.offsets.length > index ? componentsList.offsets[index] : defaultOffset)
				text	: titles[index]
				font	: jaspTheme.font
				color	: enabled ? jaspTheme.textEnabled : jaspTheme.textDisabled

			}
		}
	}

	Text
	{
		id				: itemTitle
		anchors.top		: parent.top
		anchors.left	: parent.left
		text			: title
		height			: title ? jaspTheme.variablesListTitle : 0
		font			: jaspTheme.font
		color			: enabled ? jaspTheme.textEnabled : jaspTheme.textDisabled
	}

	Rectangle
	{
		id				: itemRectangle
		anchors.top		: itemTitles.visible ? itemTitles.bottom : itemTitle.bottom
		anchors.left	: parent.left
		height			: componentsList.height - itemTitle.height
		width			: parent.width
		color			: debug ? jaspTheme.debugBackgroundColor : jaspTheme.analysisBackgroundColor
        border.width	: addBorder ? 1 : 0
		border.color	: jaspTheme.borderColor
		radius			: jaspTheme.borderRadius

		JASPScrollBar
		{
			id				: itemScrollbar
			flickable		: itemFlickable
			manualAnchor	: true
			vertical		: true
			z				: 1337

			anchors
			{
				top			: parent.top
				right		: parent.right
				bottom		: parent.bottom
				margins		: 2
			}
		}

		Flickable
		{
			id						: itemFlickable
			anchors.fill			: parent
			anchors.margins			: addBorder ? jaspTheme.contentMargin : 0
			anchors.rightMargin		: itemScrollbar.width + anchors.margins
			clip					: true
			boundsBehavior			: Flickable.StopAtBounds
			contentWidth			: itemGrid.width
			contentHeight			: itemGrid.height

			Grid
			{
				id						: itemGrid
				width					: itemRectangle.width - 2 * jaspTheme.contentMargin - (itemScrollbar.visible ? itemScrollbar.width + 2 : 0)
				focus					: true
				columns					: 1
				rowSpacing				: 1
				columnSpacing			: 1

				Repeater
				{
					id						: itemRepeater
					model					: componentsList.model
					delegate				: components
				}
			}
		}
	}

	MenuButton
	{
		id					: addIconItem
		width				: height
		radius				: height
		visible				: showAddIcon && (maximumItems <= 0 || maximumItems > componentsList.count)
		iconSource			: jaspTheme.iconPath + addIcon
		onClicked			: addItem()
		toolTip				: addTooltip
		opacity				: enabled ? 1 : .5
		anchors
		{
			bottom			: parent.bottom
			horizontalCenter: parent.horizontalCenter
		}
	}

	Component
	{
		id: components

		FocusScope
		{
			id		: itemWrapper
			height	: rowComponentItem ? rowComponentItem.height : 0
			width	: componentsList.itemGrid.width

			property var	rowComponentItem	: model.rowComponent
			property bool	isDeletable			: addItemManually && (!model.type || model.type.includes("deletable"))

			Component.onCompleted:
			{
				if (rowComponentItem)
				{
					rowComponentItem.parent = itemWrapper;
					rowComponentItem.anchors.left = itemWrapper.left
					rowComponentItem.anchors.verticalCenter = itemWrapper.verticalCenter
				}
			}

			Image
			{
				id						: removeIconID
				source					: jaspTheme.iconPath + removeIcon
				anchors.right			: parent.right
				anchors.verticalCenter	: parent.verticalCenter
				visible					: rowComponentItem.enabled && itemWrapper.isDeletable && componentsList.count > componentsList.minimumItems
				height					: jaspTheme.iconSize
				width					: jaspTheme.iconSize
				z						: 2

				QtControls.ToolTip.text			: removeTooltip
				QtControls.ToolTip.timeout		: jaspTheme.toolTipTimeout
				QtControls.ToolTip.delay		: jaspTheme.toolTipDelay
				QtControls.ToolTip.visible		: removeTooltip !== "" && deleteMouseArea.containsMouse

				MouseArea
				{
					id					: deleteMouseArea
					anchors.fill		: parent
					onClicked			: removeItem(index)
					cursorShape			: Qt.PointingHandCursor
				}
			}
		}

	}

}
