#include "CanvasPanel.h"

CanvasPanel::CanvasPanel()
{
}

bool CanvasPanel::Create(wxWindow* parent, wxWindowID winid, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
	if (!wxPanel::Create(parent, winid, pos, size, style, name))
	{
		return false;
	}

	return true;
}
