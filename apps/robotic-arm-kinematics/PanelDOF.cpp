/* +------------------------------------------------------------------------+
   |                     Mobile Robot Programming Toolkit (MRPT)            |
   |                          https://www.mrpt.org/                         |
   |                                                                        |
   | Copyright (c) 2005-2022, Individual contributors, see AUTHORS file     |
   | See: https://www.mrpt.org/Authors - All rights reserved.               |
   | Released under BSD License. See: https://www.mrpt.org/License          |
   +------------------------------------------------------------------------+ */

#include "PanelDOF.h"

//(*InternalHeaders(PanelDOF)
#include <wx/intl.h>
#include <wx/settings.h>
#include <wx/string.h>
//*)

#include "robotic_arm_kinematicsMain.h"

//(*IdInit(PanelDOF)
const long PanelDOF::ID_SIMPLEHTMLLISTBOX2 = wxNewId();
const long PanelDOF::ID_SLIDER1 = wxNewId();
const long PanelDOF::ID_TEXTCTRL1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(PanelDOF, wxPanel)
//(*EventTable(PanelDOF)
//*)
END_EVENT_TABLE()

PanelDOF::PanelDOF(wxWindow* parent, wxWindowID id)
{
	//(*Initialize(PanelDOF)
	wxFlexGridSizer* FlexGridSizer1;

	Create(
		parent, id, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL,
		_T("id"));
	FlexGridSizer1 = new wxFlexGridSizer(3, 1, 0, 0);
	Label1 = new wxSimpleHtmlListBox(
		this, ID_SIMPLEHTMLLISTBOX2, wxDefaultPosition, wxSize(-1, 30), 0,
		nullptr, wxNO_BORDER, wxDefaultValidator, _T("ID_SIMPLEHTMLLISTBOX2"));
	Label1->Append(_("&theta;<sub>i</sub>"));
	Label1->Disable();
	Label1->SetBackgroundColour(
		wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
	FlexGridSizer1->Add(
		Label1, 1, wxALL | wxEXPAND | wxALIGN_LEFT | wxALIGN_TOP, 2);
	Slider1 = new wxSlider(
		this, ID_SLIDER1, 0, -180, 180, wxDefaultPosition, wxSize(35, 150),
		wxSL_VERTICAL | wxSL_INVERSE, wxDefaultValidator, _T("ID_SLIDER1"));
	Slider1->SetMinSize(wxSize(-1, 80));
	FlexGridSizer1->Add(
		Slider1, 1,
		wxLEFT | wxRIGHT | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL,
		5);
	TextCtrl1 = new wxTextCtrl(
		this, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxSize(50, -1), 0,
		wxDefaultValidator, _T("ID_TEXTCTRL1"));
	TextCtrl1->Disable();
	FlexGridSizer1->Add(
		TextCtrl1, 1,
		wxALL | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);
	//*)

	using rakf = robotic_arm_kinematicsFrame;

	Bind(wxEVT_SCROLL_TOP, &rakf::OnSliderDOFScroll, the_win, ID_SLIDER1);
	Bind(wxEVT_SCROLL_BOTTOM, &rakf::OnSliderDOFScroll, the_win, ID_SLIDER1);
	Bind(wxEVT_SCROLL_LINEUP, &rakf::OnSliderDOFScroll, the_win, ID_SLIDER1);
	Bind(wxEVT_SCROLL_LINEDOWN, &rakf::OnSliderDOFScroll, the_win, ID_SLIDER1);
	Bind(wxEVT_SCROLL_PAGEUP, &rakf::OnSliderDOFScroll, the_win, ID_SLIDER1);
	Bind(wxEVT_SCROLL_PAGEDOWN, &rakf::OnSliderDOFScroll, the_win, ID_SLIDER1);
	Bind(
		wxEVT_SCROLL_THUMBTRACK, &rakf::OnSliderDOFScroll, the_win, ID_SLIDER1);
	Bind(
		wxEVT_SCROLL_THUMBRELEASE, &rakf::OnSliderDOFScroll, the_win,
		ID_SLIDER1);
	Bind(wxEVT_SCROLL_CHANGED, &rakf::OnSliderDOFScroll, the_win, ID_SLIDER1);
	Bind(wxEVT_SLIDER, &rakf::OnSliderDOFScrollBis, the_win, ID_SLIDER1);
}

PanelDOF::~PanelDOF()
{
	//(*Destroy(PanelDOF)
	//*)
}

void PanelDOF::OnSlider1CmdScroll([[maybe_unused]] wxScrollEvent& event) {}
