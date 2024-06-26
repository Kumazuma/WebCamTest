///////////////////////////////////////////////////////////////////////////////
// Code generated by wxUiEditor - see https://github.com/KeyWorksRW/wxUiEditor/
//
// Do not edit any code above the "End of generated code" comment block.
// Any changes before that block will be lost if it is re-generated!
///////////////////////////////////////////////////////////////////////////////

// clang-format off

#pragma once

#include <wx/choice.h>
#include <wx/frame.h>
#include <wx/gdicmn.h>
#include <wx/menu.h>
#include <wx/panel.h>
#include <wx/statusbr.h>
#include <wx/tglbtn.h>
#include <wx/toolbar.h>

class CanvasPanel;

class MyFrameBase : public wxFrame
{
public:
    enum
    {
        ID_CHOICE_DEVICE = wxID_HIGHEST + 1,
        ID_CHOICE_MODE,
        ID_TGL_READ
    };

    MyFrameBase() {}
    MyFrameBase(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString,
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_FRAME_STYLE, const wxString &name = wxFrameNameStr)
    {
        Create(parent, id, title, pos, size, style, name);
    }

    bool Create(wxWindow *parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString,
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_FRAME_STYLE, const wxString &name = wxFrameNameStr);

protected:

    // Class member variables

    CanvasPanel* m_panel;
    wxChoice* choiceDevice_;
    wxChoice* choiceMode_;
    wxMenuBar* m_menubar;
    wxStatusBar* m_statusBar;
    wxToggleButton* tglRead_;
    wxToolBar* m_toolBar;
};

// ************* End of generated code ***********
// DO NOT EDIT THIS COMMENT BLOCK!
//
// Code below this comment block will be preserved
// if the code for this class is re-generated.
//
// clang-format on
// ***********************************************
