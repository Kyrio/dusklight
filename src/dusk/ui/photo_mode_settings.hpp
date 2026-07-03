#pragma once

#include "window.hpp"
#include "pane.hpp"

namespace dusk::ui {

using BindingConfigVar = ConfigVar<int>;

class PhotoModeSettingsWindow : public Window {
public:
    PhotoModeSettingsWindow();

    void update() override;
    void hide(bool close) override;

private:
    void build_settings_tab(Pane& leftPane, Pane& rightPane);
    void build_controls_tab(Pane& leftPane, Pane& rightPane);

    void poll_pending_binding();
    void cancel_pending_binding();
    void finish_pending_binding();

    Rml::String pending_key_label() const;
    bool capture_active() const;

    BindingConfigVar* mPendingKeyBinding = nullptr;

    bool mReadyToCapture = false;
    bool mSuppressNavigationUntilNeutral = false;
};

}  // namespace dusk::ui
