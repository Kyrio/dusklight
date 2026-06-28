#include "dusk/photo_mode.hpp"
#include "dusk/settings.h"
#include "dusk/ui/ui.hpp"

namespace dusk::photo_mode {

void reset() {
    getTransientSettings().photoMode.enableFlyCamera = false;
    getTransientSettings().photoMode.lockFlyCamera = false;
    getTransientSettings().photoMode.blockGameInput = false;
    getTransientSettings().photoMode.freezeTime = false;
}

void handle_event(const SDL_Event& event) {
    if (ui::any_document_visible()) {
        // Photo mode hotkeys should not be triggered in Dusklight menus
        return;
    }

    if (event.type != SDL_EVENT_GAMEPAD_BUTTON_DOWN &&
        event.type != SDL_EVENT_GAMEPAD_AXIS_MOTION && event.type != SDL_EVENT_KEY_DOWN)
    {
        return;
    }

    auto& settings = getSettings().photoMode;
    auto& transientSettings = getTransientSettings().photoMode;

    if (!settings.enablePhotoFeatures) {
        return;
    }

    if (event.type == SDL_EVENT_KEY_DOWN) {
        if (event.key.scancode == settings.keyBindings.enableFlyCamera) {
            toggle_fly_camera(!transientSettings.enableFlyCamera);
        } else if (event.key.scancode == settings.keyBindings.lockFlyCamera) {
            toggle_lock_fly_camera(!transientSettings.lockFlyCamera);
        } else if (event.key.scancode == settings.keyBindings.blockGameInput) {
            toggle_block_game_input(!transientSettings.blockGameInput);
        } else if (event.key.scancode == settings.keyBindings.freezeTime) {
            toggle_freeze_time(!transientSettings.freezeTime);
        } else if (event.key.scancode == settings.keyBindings.minimalHUD) {
            toggle_minimal_hud_override(!getSettings().game.minimalHUD);
        }
    }
}

void toggle_fly_camera(bool enabled) {
    if (getTransientSettings().photoMode.enableFlyCamera == enabled) {
        return;
    }

    getTransientSettings().photoMode.enableFlyCamera = enabled;
    getTransientSettings().photoMode.lockFlyCamera = false;

    if (getSettings().photoMode.autoBlockGameInput) {
        getTransientSettings().photoMode.blockGameInput = enabled;
    }

    ui::push_toast({
        .type = "photo_mode",
        .title = "Photo Mode",
        .content = enabled ? "Fly Camera enabled" : "Fly Camera disabled",
        .duration = std::chrono::seconds(2),
    });
}

void toggle_lock_fly_camera(bool enabled) {
    if (!getTransientSettings().photoMode.enableFlyCamera ||
        getTransientSettings().photoMode.lockFlyCamera == enabled)
    {
        return;
    }

    getTransientSettings().photoMode.lockFlyCamera = enabled;

    ui::push_toast({
        .type = "photo_mode",
        .title = "Photo Mode",
        .content = enabled ? "Fly Camera locked" : "Fly Camera unlocked",
        .duration = std::chrono::seconds(2),
    });
}

void toggle_block_game_input(bool enabled) {
    if (getTransientSettings().photoMode.blockGameInput == enabled) {
        return;
    }

    getTransientSettings().photoMode.blockGameInput = enabled;

    ui::push_toast({
        .type = "photo_mode",
        .title = "Photo Mode",
        .content = enabled ? "Game input blocked" : "Game input restored",
        .duration = std::chrono::seconds(2),
    });
}

void toggle_freeze_time(bool enabled) {
    if (getTransientSettings().photoMode.freezeTime == enabled) {
        return;
    }

    getTransientSettings().photoMode.freezeTime = enabled;

    ui::push_toast({
        .type = "photo_mode",
        .title = "Photo Mode",
        .content = enabled ? "Time paused" : "Time unpaused",
        .duration = std::chrono::seconds(2),
    });
}

void toggle_minimal_hud_override(bool enabled) {
    if (getSettings().game.minimalHUD == enabled) {
        return;
    }

    getSettings().game.minimalHUD.setOverrideValue(enabled);

    ui::push_toast({
        .type = "photo_mode",
        .title = "Photo Mode",
        .content = enabled ? "Minimal HUD enabled" : "Minimal HUD disabled",
        .duration = std::chrono::seconds(2),
    });
}

bool is_fly_camera_enabled() {
    return getTransientSettings().photoMode.enableFlyCamera;
}

bool is_fly_camera_locked() {
    return getTransientSettings().photoMode.lockFlyCamera;
}

bool is_game_input_blocked() {
    return getTransientSettings().photoMode.blockGameInput;
}

bool is_time_frozen() {
    return getTransientSettings().photoMode.freezeTime;
}

}  // namespace dusk::photo_mode
