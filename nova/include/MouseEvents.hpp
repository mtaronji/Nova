#pragma once

struct MouseButtonEvent {
    int button; // e.g. 0 = left, 1 = right 2 = middle
    int action;
    int mods;
};

struct MouseMoveEvent {
    double x;
    double y;
};
