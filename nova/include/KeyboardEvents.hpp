#pragma once

struct KeyPressEvent {
    int key;      // platform-agnostic or platform-scancode
    int action;
    int mods;
};
