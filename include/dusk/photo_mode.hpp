#pragma once

#include <SDL3/SDL_events.h>

namespace dusk::photo_mode {

void reset();
void handle_event(const SDL_Event& event);

void toggle_fly_camera(bool enabled);
void toggle_lock_fly_camera(bool enabled);
void toggle_block_game_input(bool enabled);
void toggle_freeze_time(bool enabled);
void toggle_minimal_hud_override(bool enabled);

bool is_fly_camera_enabled();
bool is_fly_camera_locked();
bool is_game_input_blocked();
bool is_time_frozen();

}  // namespace dusk::photo_mode
