#pragma once

namespace dusk::photo_mode {

void toggleFlyCamera(bool enabled);
bool isFlyCameraActive();
bool isFlyCameraLocked();
bool isBlockingGameInput();
bool isFreezingTime();

}  // namespace dusk::photo_mode
