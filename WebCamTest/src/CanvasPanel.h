#pragma once

#include <wx/wx.h>

class CanvasPanel : public wxPanel {
public:
	CanvasPanel();
    CanvasPanel(wxWindow* parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxASCII_STR(wxPanelNameStr))
        : CanvasPanel()
    {
        CanvasPanel::Create(parent, winid, pos, size, style, name);
    }

    bool Create(wxWindow* parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString& name = wxASCII_STR(wxPanelNameStr));

private:

};