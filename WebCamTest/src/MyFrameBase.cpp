///////////////////////////////////////////////////////////////////////////////
// Code generated by wxUiEditor - see https://github.com/KeyWorksRW/wxUiEditor/
//
// Do not edit any code above the "End of generated code" comment block.
// Any changes before that block will be lost if it is re-generated!
///////////////////////////////////////////////////////////////////////////////

// clang-format off

#include <wx/sizer.h>
#include <wx/statbox.h>

#include "CanvasPanel.h"

#include "MyFrameBase.h"

bool MyFrameBase::Create(wxWindow* parent, wxWindowID id, const wxString& title,
    const wxPoint& pos, const wxSize& size, long style, const wxString &name)
{

    if (!wxFrame::Create(parent, id, title, pos, size, style, name))
        return false;

    m_toolBar = CreateToolBar();
    m_toolBar->Realize();

    m_menubar = new wxMenuBar();

    SetMenuBar(m_menubar);

    m_statusBar = CreateStatusBar();

    auto* box_sizer = new wxBoxSizer(wxVERTICAL);

    auto* static_box = new wxStaticBoxSizer(wxVERTICAL, this, "Device");

    choiceDevice_ = new wxChoice(static_box->GetStaticBox(), ID_CHOICE_DEVICE);
    static_box->Add(choiceDevice_, wxSizerFlags().Expand().Border(wxALL));

    box_sizer->Add(static_box, wxSizerFlags().Expand().Border(wxALL));

    auto* static_box2 = new wxStaticBoxSizer(wxVERTICAL, this, "Mode");

    choiceMode_ = new wxChoice(static_box2->GetStaticBox(), ID_CHOICE_MODE);
    static_box2->Add(choiceMode_, wxSizerFlags().Expand().Border(wxALL));

    box_sizer->Add(static_box2, wxSizerFlags().Expand().Border(wxALL));

    tglRead_ = new wxToggleButton(this, ID_TGL_READ, "Read");
    box_sizer->Add(tglRead_, wxSizerFlags().Expand().Border(wxALL));

    m_panel = new CanvasPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    box_sizer->Add(m_panel, wxSizerFlags(1).Expand().Border(wxALL));
    SetSizerAndFit(box_sizer);

    Centre(wxBOTH);

    return true;
}

// ************* End of generated code ***********
// DO NOT EDIT THIS COMMENT BLOCK!
//
// Code below this comment block will be preserved
// if the code for this class is re-generated.
//
// clang-format on
// ***********************************************
