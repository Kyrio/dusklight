#include "photo_mode_settings.hpp"

#include "bool_button.hpp"
#include "dusk/config.hpp"
#include "dusk/photo_mode.hpp"
#include "pane.hpp"

namespace dusk::ui {

namespace {

constexpr std::array kCameraControlModeNames = {
    "Both",
    "Mouse & Keyboard",
    "Gamepad",
};

struct ConfigBoolProps {
    Rml::String key;
    Rml::String icon;
    Rml::String helpText;
    std::function<void(bool)> onChange;
    std::function<bool()> isDisabled;
};

struct SessionBoolProps {
    Rml::String key;
    Rml::String icon;
    Rml::String helpText;
    std::function<void(bool)> setValue;
    std::function<bool()> isDisabled;
};

Rml::String keyboard_key_name(int scancode) {
    if (scancode == PAD_KEY_INVALID) {
        return "Not Bound";
    }

    if (scancode < 0) {
        return "Unknown";
    }

    const char* name = SDL_GetScancodeName(static_cast<SDL_Scancode>(scancode));
    if (name == nullptr || name[0] == '\0') {
        return "Unknown";
    }

    return name;
}

bool features_enabled() {
    return getSettings().photoMode.enablePhotoFeatures;
}

BoolButton& config_bool_button(
    Pane& leftPane, Pane& rightPane, ConfigVar<bool>& var, ConfigBoolProps props) {
    auto& button = leftPane.add_child<BoolButton>(BoolButton::Props{
        .key = std::move(props.key),
        .icon = std::move(props.icon),
        .getValue = [&var] { return var.getValue(); },
        .setValue =
            [&var, callback = std::move(props.onChange)](bool value) {
                if (value == var.getValue()) {
                    return;
                }

                var.setValue(value);
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

BoolButton& session_bool_button(
    Pane& leftPane, Pane& rightPane, bool& var, bool defaultValue, SessionBoolProps props) {
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

void add_key_button(Pane& pane, ConfigVar<int>& binding, Rml::String title) {
    pane.add_select_button({
        .key = std::move(title),
        .getValue = [&binding] { return keyboard_key_name(binding.getValue()); },
    });
}

}  // namespace

PhotoModeSettingsWindow::PhotoModeSettingsWindow() {
    add_tab("Settings", [this](Rml::Element* content) {
        auto& leftPane = add_child<Pane>(content, Pane::Type::Controlled);
        auto& rightPane = add_child<Pane>(content, Pane::Type::Uncontrolled);

        build_settings_tab(leftPane, rightPane);
    });

    add_tab("Controls", TabBuilder([this](Rml::Element* content) {
        auto& leftPane = add_child<Pane>(content, Pane::Type::Controlled);
        auto& rightPane = add_child<Pane>(content, Pane::Type::Uncontrolled);

        build_controls_tab(leftPane, rightPane);
    }));

    add_tab("Camera Data", TabBuilder());
}

void PhotoModeSettingsWindow::build_settings_tab(Pane& leftPane, Pane& rightPane) {
    leftPane.add_section("General");
    config_bool_button(leftPane, rightPane, getSettings().photoMode.enablePhotoFeatures,
        {
            .key = "Enable Features",
            .helpText = "Enable Dusklight's photo mode features.<br/>"
                        "<span class=\"tip\">Tip: Disable this if you don't want any photo mode "
                        "hotkeys to be triggered during gameplay.</span>",
            .onChange =
                [](bool value) {
                    if (!value) {
                        photo_mode::reset();
                    }
                },
        });

    config_bool_button(leftPane, rightPane, getSettings().photoMode.autoBlockGameInput,
        {.key = "Block Input By Default",
            .helpText = "Block game input while the fly camera is enabled, unless you manually "
                        "unblock it.",
            .isDisabled = [] { return !features_enabled(); }});

    leftPane.add_section("Session");
    session_bool_button(leftPane, rightPane, getTransientSettings().photoMode.enableFlyCamera,
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

    session_bool_button(leftPane, rightPane, getTransientSettings().photoMode.lockFlyCamera, false,
        {
            .key = "Lock Fly Camera",
            .helpText = "Lock the fly camera in place. Useful to avoid losing your shot while "
                        "adjusting settings.",
            .setValue = [](bool value) { photo_mode::toggle_lock_fly_camera(value); },
            .isDisabled =
                [] {
                    return !features_enabled() || !getTransientSettings().photoMode.enableFlyCamera;
                },
        });

    session_bool_button(leftPane, rightPane, getTransientSettings().photoMode.blockGameInput, false,
        {
            .key = "Block Game Input",
            .helpText = "Block normal game input, such as player movement.",
            .setValue = [](bool value) { photo_mode::toggle_block_game_input(value); },
            .isDisabled = [] { return !features_enabled(); },
        });

    session_bool_button(leftPane, rightPane, getTransientSettings().photoMode.freezeTime, false,
        {
            .key = "Freeze Time",
            .helpText = "Pause in-game time, which lets you move the camera around a still scene.",
            .setValue = [](bool value) { photo_mode::toggle_freeze_time(value); },
            .isDisabled = [] { return !features_enabled(); },
        });

    leftPane.register_control(
        leftPane.add_child<BoolButton>(BoolButton::Props{
            .key = "Minimal HUD",
            .getValue = [] { return getSettings().game.minimalHUD.getValue(); },
            .setValue = [](bool value) { photo_mode::toggle_minimal_hud_override(value); },
            .isDisabled = [] { return !features_enabled(); },
            .isModified =
                [&var = getSettings().game.minimalHUD] {
                    return var.getValue() != var.getDefaultValue();
                },
        }),
        rightPane, [](Pane& pane) {
            pane.clear();
            pane.add_rml("Override the Minimal HUD setting for this session.<br/>"
                         "Useful to hide UI elements for your shots without affecting your "
                         "saved config.");
        });
}

void PhotoModeSettingsWindow::build_controls_tab(Pane& leftPane, Pane& rightPane) {
    auto& keyBindings = getSettings().photoMode.keyBindings;

    leftPane.register_control(leftPane.add_select_button({
                                  .key = "Key Bindings",
                                  .getValue = [] { return Rml::String(">"); },
                              }),
        rightPane, [&keyBindings](Pane& pane) {
            pane.clear();
            pane.add_section("Hotkeys");
            add_key_button(pane, keyBindings.enableFlyCamera, "Enable Fly Camera");
            add_key_button(pane, keyBindings.lockFlyCamera, "Lock Fly Camera");
            add_key_button(pane, keyBindings.blockGameInput, "Block Game Input");
            add_key_button(pane, keyBindings.freezeTime, "Freeze Time");
            add_key_button(pane, keyBindings.minimalHUD, "Minimal HUD");
        });

    leftPane.add_section("Options");

    leftPane.register_control(
        leftPane.add_select_button({
            .key = "Camera Control Device",
            .getValue =
                [] {
                    return kCameraControlModeNames[static_cast<u8>(
                        getSettings().photoMode.flyCameraControlMode.getValue())];
                },
            .isModified =
                [&var = getSettings().photoMode.flyCameraControlMode] {
                    return var.getValue() != var.getDefaultValue();
                },
        }),
        rightPane, [](Pane& pane) {
            pane.clear();

            for (int i = 0; i < kCameraControlModeNames.size(); i++) {
                pane
                    .add_button({
                        .text = kCameraControlModeNames[i],
                        .isSelected =
                            [i] {
                                return getSettings().photoMode.flyCameraControlMode.getValue() ==
                                       static_cast<FlyCameraControlMode>(i);
                            },
                    })
                    .on_pressed([i] {
                        getSettings().photoMode.flyCameraControlMode.setValue(
                            static_cast<FlyCameraControlMode>(i));
                        config::Save();
                    });
            }

            pane.add_rml("<br/>Choose which devices are used to control the fly camera.");
        });
}

}  // namespace dusk::ui
