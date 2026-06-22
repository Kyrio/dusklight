#include "dusk/photo_mode.hpp"
#include "dusk/settings.h"

namespace dusk::photo_mode {

void toggleFlyCamera(bool enabled) {
    if (getTransientSettings().photoMode.enableFlyCamera != enabled) {
        getTransientSettings().photoMode.enableFlyCamera = enabled;
    }

    if (getSettings().photoMode.autoBlockGameInput) {
        getTransientSettings().photoMode.blockGameInput = enabled;
    }
}

bool isFlyCameraActive() {
    return getTransientSettings().photoMode.enableFlyCamera;
}

bool isFlyCameraLocked() {
    return getTransientSettings().photoMode.lockFlyCamera;
}

bool isBlockingGameInput() {
    return getTransientSettings().photoMode.blockGameInput;
}

bool isFreezingTime() {
    return getTransientSettings().photoMode.freezeTime;
}

}  // namespace dusk::photo_mode
