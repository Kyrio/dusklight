#pragma once

#include "window.hpp"
#include "pane.hpp"

namespace dusk::ui {

class PhotoModeSettingsWindow : public Window {
public:
    PhotoModeSettingsWindow();

private:
    void build_settings_tab(Pane& leftPane, Pane& rightPane);
    void build_controls_tab(Pane& leftPane, Pane& rightPane);
};

}  // namespace dusk::ui
