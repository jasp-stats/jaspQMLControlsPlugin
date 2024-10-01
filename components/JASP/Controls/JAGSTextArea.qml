import QtQuick
import JASP.Controls

TextArea
{
	textType: JASP.TextTypeJAGSmodel
	showLineNumber: true
	RSyntaxHighlighterQuick
	{
		textDocument:		parent.textDocument
	}
}
