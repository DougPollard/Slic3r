#ifndef slic3r_Tab_hpp_
#define slic3r_Tab_hpp_

//	 The "Expert" tab at the right of the main tabbed window.
//	
//	 This file implements following packages:
//	   Slic3r::GUI::Tab;
//	       Slic3r::GUI::Tab::Print;
//	       Slic3r::GUI::Tab::Filament;
//	       Slic3r::GUI::Tab::Printer;
//	   Slic3r::GUI::Tab::Page
//	       - Option page: For example, the Slic3r::GUI::Tab::Print has option pages "Layers and perimeters", "Infill", "Skirt and brim" ...
//	   Slic3r::GUI::SavePresetWindow
//	       - Dialog to select a new preset name to store the configuration.
//	   Slic3r::GUI::Tab::Preset;
//	       - Single preset item: name, file is default or external.

#include <wx/panel.h>
#include <wx/notebook.h>
#include <wx/scrolwin.h>
#include <wx/sizer.h>
#include <wx/bmpcbox.h>
#include <wx/bmpbuttn.h>
#include <wx/treectrl.h>
#include <wx/imaglist.h>
#include <wx/statbox.h>
#include <wx/dataview.h>

#include <map>
#include <vector>
#include <memory>

#include "BedShapeDialog.hpp"

//!enum { ID_TAB_TREE = wxID_HIGHEST + 1 };

namespace Slic3r {
namespace GUI {

typedef std::pair<wxBitmap*, std::string>				t_icon_description;
typedef std::vector<std::pair<wxBitmap*, std::string>>	t_icon_descriptions;

// Single Tab page containing a{ vsizer } of{ optgroups }
// package Slic3r::GUI::Tab::Page;
using ConfigOptionsGroupShp = std::shared_ptr<ConfigOptionsGroup>;
class Page : public wxScrolledWindow
{
	wxWindow*		m_parent;
	wxString		m_title;
	size_t			m_iconID;
	wxBoxSizer*		m_vsizer;
public:
	Page(wxWindow* parent, const wxString title, const int iconID) :
			m_parent(parent),
			m_title(title),
			m_iconID(iconID)
	{
		Create(m_parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
		m_vsizer = new wxBoxSizer(wxVERTICAL);
		m_item_color = &get_label_clr_default();
		SetSizer(m_vsizer);
	}
	~Page(){}

	bool				m_is_modified_values{ false };
	bool				m_is_nonsys_values{ true };

public:
	std::vector <ConfigOptionsGroupShp> m_optgroups;
	DynamicPrintConfig* m_config;

	wxBoxSizer*	vsizer() const { return m_vsizer; }
	wxWindow*	parent() const { return m_parent; }
	wxString	title()	 const { return m_title; }
	size_t		iconID() const { return m_iconID; }
	void		set_config(DynamicPrintConfig* config_in) { m_config = config_in; }
	void		reload_config();
	Field*		get_field(const t_config_option_key& opt_key, int opt_index = -1) const;
	bool		set_value(const t_config_option_key& opt_key, const boost::any& value);
	ConfigOptionsGroupShp	new_optgroup(const wxString& title, int noncommon_label_width = -1);

	bool		set_item_colour(const wxColour *clr) {
		if (m_item_color != clr) {
			m_item_color = clr;
			return true;
		}
		return false;
	}

	const wxColour	get_item_colour() {
			return *m_item_color;
	}

protected:
	// Color of TreeCtrlItem. The wxColour will be updated only if the new wxColour pointer differs from the currently rendered one.
	const wxColour*		m_item_color;
};

// Slic3r::GUI::Tab;

using PageShp = std::shared_ptr<Page>;
class Tab: public wxPanel
{
	wxNotebook*			m_parent;
#ifdef __WXOSX__
	wxPanel*			m_tmp_panel;
	int					m_size_move = -1;
#endif // __WXOSX__
protected:
	std::string			m_name;
	const wxString		m_title;
	wxBitmapComboBox*	m_presets_choice;
	wxBitmapButton*		m_btn_save_preset;
	wxBitmapButton*		m_btn_delete_preset;
	wxBitmapButton*		m_btn_hide_incompatible_presets;
	wxBoxSizer*			m_hsizer;
	wxBoxSizer*			m_left_sizer;
	wxTreeCtrl*			m_treectrl;
	wxImageList*		m_icons;
	wxCheckBox*			m_compatible_printers_checkbox;
	wxButton*			m_compatible_printers_btn;
	wxButton*			m_undo_btn;
	wxButton*			m_undo_to_sys_btn;
	wxButton*			m_question_btn;
	wxComboCtrl*		m_cc_presets_choice;
	wxDataViewTreeCtrl*	m_presetctrl;
	wxImageList*		m_preset_icons;

	// Cached bitmaps.
	// A "flag" icon to be displayned next to the preset name in the Tab's combo box.
	wxBitmap			m_bmp_show_incompatible_presets;
	wxBitmap			m_bmp_hide_incompatible_presets;
	// Bitmaps to be shown on the "Revert to system" aka "Lock to system" button next to each input field.
	wxBitmap 			m_bmp_value_lock;
	wxBitmap 			m_bmp_value_unlock;
	wxBitmap 			m_bmp_white_bullet;
	// The following bitmap points to either m_bmp_value_unlock or m_bmp_white_bullet, depending on whether the current preset has a parent preset.
	wxBitmap 		   *m_bmp_non_system;
	// Bitmaps to be shown on the "Undo user changes" button next to each input field.
	wxBitmap 			m_bmp_value_revert;
// 	wxBitmap 			m_bmp_value_unmodified;
	wxBitmap			m_bmp_question;

	// Colors for ui "decoration"
	wxColour			m_sys_label_clr;
	wxColour			m_modified_label_clr;
	wxColour			m_default_text_clr;

	// Tooltip text for reset buttons (for whole options group)
	wxString			m_ttg_value_lock;
	wxString			m_ttg_value_unlock;
	wxString			m_ttg_white_bullet_ns;
	// The following text points to either m_ttg_value_unlock or m_ttg_white_bullet_ns, depending on whether the current preset has a parent preset.
	wxString			*m_ttg_non_system;
	// Tooltip text to be shown on the "Undo user changes" button next to each input field.
	wxString			m_ttg_white_bullet;
	wxString			m_ttg_value_revert;

	// Tooltip text for reset buttons (for each option in group)
	wxString			m_tt_value_lock;
	wxString			m_tt_value_unlock;
	// The following text points to either m_tt_value_unlock or m_ttg_white_bullet_ns, depending on whether the current preset has a parent preset.
	wxString			*m_tt_non_system;
	// Tooltip text to be shown on the "Undo user changes" button next to each input field.
	wxString			m_tt_white_bullet;
	wxString			m_tt_value_revert;

	int					m_icon_count;
	std::map<std::string, size_t>	m_icon_index;		// Map from an icon file name to its index
	std::vector<PageShp>			m_pages;
	bool				m_disable_tree_sel_changed_event;
	bool				m_show_incompatible_presets;
	bool				m_no_controller;

	std::vector<std::string>	m_reload_dependent_tabs = {};
	enum OptStatus { osSystemValue = 1, osInitValue = 2 };
	std::map<std::string, int>	m_options_list;
	int							m_opt_status_value = 0;

	t_icon_descriptions	m_icon_descriptions = {};

	// The two following two event IDs are generated at Plater.pm by calling Wx::NewEventType.
	wxEventType			m_event_value_change = 0;
	wxEventType 		m_event_presets_changed = 0;

	bool				m_is_modified_values{ false };
	bool				m_is_nonsys_values{ true };
	bool				m_postpone_update_ui {false};

	size_t				m_selected_preset_item{ 0 };

public:
	PresetBundle*		m_preset_bundle;
	bool				m_show_btn_incompatible_presets = false;
	PresetCollection*	m_presets;
	DynamicPrintConfig*	m_config;
	ogStaticText*		m_parent_preset_description_line;
	wxStaticText*		m_colored_Label = nullptr;

public:
	Tab() {}
	Tab(wxNotebook* parent, const wxString& title, const char* name, bool no_controller) : 
		m_parent(parent), m_title(title), m_name(name), m_no_controller(no_controller) {
		Create(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBK_LEFT | wxTAB_TRAVERSAL, name);
		get_tabs_list().push_back(this);
	}
	~Tab(){
		delete_tab_from_list(this);
	}

	wxWindow*	parent() const { return m_parent; }
	wxString	title()	 const { return m_title; }
	std::string	name()	 const { return m_name; }

	// Set the events to the callbacks posted to the main frame window (currently implemented in Perl).
	void 		set_event_value_change(wxEventType evt) { m_event_value_change = evt; }
	void 		set_event_presets_changed(wxEventType evt) { m_event_presets_changed = evt; }
	
	void		create_preset_tab(PresetBundle *preset_bundle);
	void		load_current_preset();
	void		rebuild_page_tree();
	void		select_preset(const std::string& preset_name = "");
	bool		may_discard_current_dirty_preset(PresetCollection* presets = nullptr, const std::string& new_printer_name = "");
	wxSizer*	compatible_printers_widget(wxWindow* parent, wxCheckBox** checkbox, wxButton** btn);

	void		update_presetsctrl(wxDataViewTreeCtrl* ui, bool show_incompatible);
	void		load_key_value(const std::string& opt_key, const boost::any& value, bool saved_value = false);
	void		reload_compatible_printers_widget();

	void		OnTreeSelChange(wxTreeEvent& event);
	void		OnKeyDown(wxKeyEvent& event);

	void		save_preset(std::string name = "");
	void		delete_preset();
	void		toggle_show_hide_incompatible();
	void		update_show_hide_incompatible_button();
	void		update_ui_from_settings();
	void		update_labels_colour();
	void		update_changed_ui();
	void		get_sys_and_mod_flags(const std::string& opt_key, bool& sys_page, bool& modified_page);
	void		update_changed_tree_ui();
	void		update_undo_buttons();

	void		on_roll_back_value(const bool to_sys = false);

	PageShp		add_options_page(const wxString& title, const std::string& icon, bool is_extruder_pages = false);

	virtual void	OnActivate();
	virtual void	on_preset_loaded(){}
	virtual void	build() = 0;
	virtual void	update() = 0;
	virtual void	init_options_list();
	void			load_initial_data();
	void			update_dirty();
	void			update_tab_ui();
	void			load_config(const DynamicPrintConfig& config);
	virtual void	reload_config();
	Field*			get_field(const t_config_option_key& opt_key, int opt_index = -1) const;
	bool			set_value(const t_config_option_key& opt_key, const boost::any& value);
	wxSizer*		description_line_widget(wxWindow* parent, ogStaticText** StaticText);
	bool			current_preset_is_dirty();

	DynamicPrintConfig*	get_config() { return m_config; }
	PresetCollection*	get_presets() { return m_presets; }
	std::vector<std::string>	get_dependent_tabs() { return m_reload_dependent_tabs; }
	size_t				get_selected_preset_item() { return m_selected_preset_item; }

	void			on_value_change(const std::string& opt_key, const boost::any& value);

protected:
	void			on_presets_changed();
	void			update_preset_description_line();
	void			update_frequently_changed_parameters();
    void            update_wiping_button_visibility();
	void			update_tab_presets(wxComboCtrl* ui, bool show_incompatible);
	void			fill_icon_descriptions();
	void			set_tooltips_text();
};

//Slic3r::GUI::Tab::Print;
class TabPrint : public Tab
{
public:
	TabPrint() {}
	TabPrint(wxNotebook* parent, bool no_controller) : 
		Tab(parent, _(L("Print Settings")), "print", no_controller) {}
	~TabPrint(){}

	ogStaticText*	m_recommended_thin_wall_thickness_description_line;
	bool		m_support_material_overhangs_queried = false;

	void		build() override;
	void		reload_config() override;
	void		update() override;
	void		OnActivate() override;
};

//Slic3r::GUI::Tab::Filament;
class TabFilament : public Tab
{
	ogStaticText*	m_volumetric_speed_description_line;
	ogStaticText*	m_cooling_description_line;
public:
	TabFilament() {}
	TabFilament(wxNotebook* parent, bool no_controller) : 
		Tab(parent, _(L("Filament Settings")), "filament", no_controller) {}
	~TabFilament(){}

	void		build() override;
	void		reload_config() override;
	void		update() override;
	void		OnActivate() override;
};

//Slic3r::GUI::Tab::Printer;
class TabPrinter : public Tab
{
	bool		m_has_single_extruder_MM_page = false;
	bool		m_use_silent_mode = false;
	void		append_option_line(ConfigOptionsGroupShp optgroup, const std::string opt_key);
	bool		m_rebuild_kinematics_page = false;
public:
	wxButton*	m_serial_test_btn;
	wxButton*	m_octoprint_host_test_btn;

	size_t		m_extruders_count;
	size_t		m_extruders_count_old = 0;
	size_t		m_initial_extruders_count;
	size_t		m_sys_extruders_count;

	TabPrinter() {}
	TabPrinter(wxNotebook* parent, bool no_controller) : Tab(parent, _(L("Printer Settings")), "printer", no_controller) {}
	~TabPrinter(){}

	void		build() override;
	void		update() override;
	void		update_serial_ports();
	void		extruders_count_changed(size_t extruders_count);
	PageShp		build_kinematics_page();
	void		build_extruder_pages();
	void		on_preset_loaded() override;
	void		init_options_list() override;
};

class SavePresetWindow :public wxDialog
{
public:
	SavePresetWindow(wxWindow* parent) :wxDialog(parent, wxID_ANY, _(L("Save preset"))){}
	~SavePresetWindow(){}

	std::string		m_chosen_name;
	wxComboBox*		m_combo;

	void			build(const wxString& title, const std::string& default_name, std::vector<std::string> &values);
	void			accept();
	std::string		get_name() { return m_chosen_name; }
};

} // GUI
} // Slic3r

#endif /* slic3r_Tab_hpp_ */
