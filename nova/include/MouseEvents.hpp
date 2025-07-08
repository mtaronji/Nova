#pragma once

struct MouseButtonEvent {
    int button; // e.g. 0 = left, 1 = right 2 = middle
    int action;
    int mods;
};

struct MouseMoveEvent {
    double x;
    double y;

    MouseMoveEvent operator-(const MouseMoveEvent& other) const {
        MouseMoveEvent result;
        result.x = this->x - other.x;
        result.y = this->y - other.y;
        return result;
    }
};
