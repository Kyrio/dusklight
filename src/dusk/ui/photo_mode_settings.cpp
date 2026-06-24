#include "photo_mode_settings.hpp"

#include "bool_button.hpp"
#include "dusk/config.hpp"
#include "dusk/photo_mode.hpp"
#include "pane.hpp"

namespace dusk::ui {

namespace {

struct ConfigBoolProps {
    Rml::String key;
    Rml::String icon;
    Rml::String helpText;
    std::function<void(bool)> onChange;
    std::function<bool()> isDisabled;
    bool override;
};

struct TransientBoolProps {
    Rml::String key;
    Rml::String icon;
    Rml::String helpText;
    std::function<void(bool)> setValue;
    std::function<bool()> isDisabled;
};

bool features_enabled() {
    return getSettings().photoMode.enablePhotoFeatures;
}

SelectButton& config_bool_select(
    Pane& leftPane, Pane& rightPane, ConfigVar<bool>& var, ConfigBoolProps props) {
    auto& button = leftPane.add_child<BoolButton>(BoolButton::Props{
        .key = std::move(props.key),
        .icon = std::move(props.icon),
        .getValue = [&var] { return var.getValue(); },
        .setValue =
            [&var, callback = std::move(props.onChange), override = props.override](bool value) {
                if (value == var.getValue()) {
                    return;
                }

                if (override) {
                    var.setOverrideValue(value);
                } else {
                    var.setValue(value);
                }

                config::Save();
                if (callback) {
                    callback(value);
                }
            },
        .isDisabled = std::move(props.isDisabled),
        .isModified = [&var] { return var.getValue() != var.getDefaultValue(); },
    });
    leftPane.register_control(
        button, rightPane, [helpText = std::move(props.helpText)](Pane& pane) {
            pane.clear();
            pane.add_rml(helpText);
        });
    return button;
}

SelectButton& transient_bool_select(
    Pane& leftPane, Pane& rightPane, bool& var, bool defaultValue, TransientBoolProps props) {
    auto& button = leftPane.add_child<BoolButton>(BoolButton::Props{
        .key = std::move(props.key),
        .icon = std::move(props.icon),
        .getValue = [&var] { return var; },
        .setValue =
            props.setValue ? std::move(props.setValue) : [&var](bool value) { var = value; },
        .isDisabled = std::move(props.isDisabled),
        .isModified = [&var, defaultValue] { return var != defaultValue; },
    });
    leftPane.register_control(
        button, rightPane, [helpText = std::move(props.helpText)](Pane& pane) {
            pane.clear();
            pane.add_rml(helpText);
        });
    return button;
}

}  // namespace

PhotoModeSettingsWindow::PhotoModeSettingsWindow() {
    add_tab("Settings", [this](Rml::Element* content) {
        auto& leftPane = add_child<Pane>(content, Pane::Type::Controlled);
        auto& rightPane = add_child<Pane>(content, Pane::Type::Uncontrolled);

        leftPane.add_section("General");
        config_bool_select(leftPane, rightPane, getSettings().photoMode.enablePhotoFeatures,
            {
                .key = "Enable Features",
                .helpText =
                    "Enable Dusklight's photo mode features.<br/>"
                    "<span class=\"tip\">Tip: Disable this if you don't want any photo mode "
                    "hotkeys to be triggered during gameplay.</span>",
                .onChange =
                    [](bool value) {
                        if (!value) {
                            photo_mode::reset();
                        }
                    },
            });

        config_bool_select(leftPane, rightPane, getSettings().photoMode.autoBlockGameInput,
            {.key = "Block Input By Default",
                .helpText = "Block game input while the fly camera is enabled, unless you manually "
                            "unblock it.",
                .isDisabled = [] { return !features_enabled(); }});

        leftPane.add_section("Session");
        transient_bool_select(leftPane, rightPane, getTransientSettings().photoMode.enableFlyCamera,
            false,
            {
                .key = "Fly Camera",
                .helpText = "Enable the photo mode camera, allowing you to fly around with the "
                            "controls you have set.<br/>"
                            "<span class=\"tip\">Tip: Bind a key to Toggle Fly Camera in the "
                            "Controls tab.</span>",
                .setValue = [](bool value) { photo_mode::toggle_fly_camera(value); },
                .isDisabled = [] { return !features_enabled(); },
            });

        transient_bool_select(leftPane, rightPane, getTransientSettings().photoMode.lockFlyCamera,
            false,
            {
                .key = "Lock Fly Camera",
                .helpText = "Lock the fly camera in place. Useful to avoid losing your shot while "
                            "adjusting settings.",
                .setValue =
                    [](bool value) { photo_mode::toggle_lock_fly_camera(value); },
                .isDisabled = [] { return !features_enabled() || !getTransientSettings().photoMode.enableFlyCamera; },
            });

        transient_bool_select(leftPane, rightPane, getTransientSettings().photoMode.blockGameInput,
            false,
            {
                .key = "Block Game Input",
                .helpText = "Block normal game input, such as player movement.",
                .setValue =
                    [](bool value) { photo_mode::toggle_block_game_input(value); },
                .isDisabled = [] { return !features_enabled(); },
            });

        transient_bool_select(leftPane, rightPane, getTransientSettings().photoMode.freezeTime,
            false,
            {
                .key = "Freeze Time",
                .helpText =
                    "Pause in-game time, which lets you move the camera around a still scene.",
                .setValue = [](bool value) { photo_mode::toggle_freeze_time(value); },
                .isDisabled = [] { return !features_enabled(); },
            });

        config_bool_select(leftPane, rightPane, getSettings().game.minimalHUD,
            {
                .key = "Minimal HUD",
                .helpText = "Override the Minimal HUD setting for this session.<br/>"
                            "Useful to hide UI elements for your shots without affecting your "
                            "saved config.",
                .isDisabled = [] { return !features_enabled(); },
                .override = true,
            });
    });

    add_tab("Controls", TabBuilder());
    add_tab("Camera Data", TabBuilder());
}

}  // namespace dusk::ui
