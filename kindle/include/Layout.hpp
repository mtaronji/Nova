#pragma once

namespace Kindle {

    enum class LayoutDirection {
        Row,
        Column
    };

    enum class JustifyContent {
        Start,
        Center,
        End,
        SpaceBetween,
        SpaceAround,
        SpaceEvenly
    };

    enum class AlignItems {
        Start,
        Center,
        End,
        Stretch
    };

    enum class AlignSelf {
        Auto,  // fall back to parent AlignItems
        Start,
        Center,
        End,
        Stretch
    };

    enum class FlexWrap {
        NoWrap,
        Wrap
    };

    struct FlexStyle {
        float grow = 0.0f;
        float shrink = 1.0f;
        float basis = -1.0f; // -1 means auto
    };

    struct Layout {
        LayoutDirection direction = LayoutDirection::Row;
        JustifyContent justifyContent = JustifyContent::Start;
        AlignItems alignItems = AlignItems::Stretch;
        FlexWrap wrap = FlexWrap::NoWrap;

        float gap = 0.0f; // spacing between elements
        float paddingLeft = 0.0f;
        float paddingRight = 0.0f;
        float paddingTop = 0.0f;
        float paddingBottom = 0.0f;

        // You may also want:
        // float marginLeft, etc. — for outer spacing

        // Optional per-child override
     /*   std::unordered_map<WidgetID, AlignSelf> alignOverrides;
        std::unordered_map<WidgetID, FlexStyle> flexOverrides;*/
    };
}