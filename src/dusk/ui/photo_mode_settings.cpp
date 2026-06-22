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
    Pane& leftPane, Pane& rightPane, bool& var, bool defaultValue, ConfigBoolProps props) {
    auto& button = leftPane.add_child<BoolButton>(BoolButton::Props{
        .key = std::move(props.key),
        .icon = std::move(props.icon),
        .getValue = [&var] { return var; },
        .setValue =
            [&var, callback = std::move(props.onChange)](bool value) {
                if (value == var) {
                    return;
                }

                var = value;

                if (callback) {
                    callback(value);
                }
            },
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
        config_bool_select(leftPane, rightPane, getSettings().photoMode.enableHotkeys,
            {.key = "Enable Hotkeys",
                .helpText = "Listen to the photo mode hotkeys defined in the Keyboard and Gamepad "
                            "tabs.<br/>"
                            "<span class=\"tip\">Disabled by default to not disturb normal "
                            "gameplay.</span>"});

        config_bool_select(leftPane, rightPane, getSettings().photoMode.autoBlockGameInput,
            {
                .key = "Block Input By Default",
                .helpText =
                    "Start blocking game input as soon as the Fly Camera is enabled.",
            });

        leftPane.add_section("Session");
        transient_bool_select(leftPane, rightPane, getTransientSettings().photoMode.enableFlyCamera,
            false,
            {
                .key = "Fly Camera",
                .helpText = "Enable the photo mode camera, allowing you to fly around with the "
                            "controls you have set.<br/>"
                            "<span class=\"tip\">Tip: Bind a key to Toggle Fly Camera in the "
                            "Keyboard/Gamepad tab.</span>",
                .onChange = [](bool value) { dusk::photo_mode::toggleFlyCamera(value); },
            });

        transient_bool_select(leftPane, rightPane, getTransientSettings().photoMode.lockFlyCamera,
            false,
            {
                .key = "Lock Fly Camera",
                .helpText = "Lock the Fly Camera in place. Useful to avoid losing your shot while "
                            "adjusting settings.",
            });

        transient_bool_select(leftPane, rightPane, getTransientSettings().photoMode.blockGameInput,
            false,
            {
                .key = "Block Game Input",
                .helpText = "Block normal game input, such as player movement.",
            });

        transient_bool_select(leftPane, rightPane, getTransientSettings().photoMode.freezeTime,
            false,
            {
                .key = "Freeze Time",
                .helpText =
                    "Pause in-game time, which lets you move the camera around a still scene.",
            });

        config_bool_select(leftPane, rightPane, getSettings().game.minimalHUD,
            {
                .key = "Minimal HUD",
                .helpText = "Override the Minimal HUD setting for this session.<br/>"
                            "Useful to hide UI elements for your shots without affecting your "
                            "saved config.",
                .override = true,
            });
    });

    add_tab("Raw Data", TabBuilder());
    add_tab("Keyboard", TabBuilder());
    add_tab("Gamepad", TabBuilder());
}

}  // namespace dusk::ui
