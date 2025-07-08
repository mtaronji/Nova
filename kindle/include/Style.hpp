#pragma once
#include <string>

namespace Kindle {

    struct Color {
        float Red;
        float Green;
        float Blue;
        float Alpha;
    };

    enum class  TextAlign {
        Left,
        Center,
        Right
    };

    enum class  Align {
        Start,
        Center,
        End,
        Stretch
    };

    enum class  CursorType {
        Arrow,
        Hand,
        IBeam,
        Crosshair,
        ResizeHorizontal,
        ResizeVertical,
        Move,
        Wait,
        None,
    };
    enum class  EasingFn {
        Linear,
        EaseIn,
        EaseOut,
        EaseInOut,
        Bounce,
        Elastic,
        Back,
        Step
    };
    struct Style {

        Style() : Style(Default()){}
        static Style Default() {
            Style s;
            s.paddingLeft = s.paddingRight = s.paddingTop = s.paddingBottom = 4.0f;
            s.marginLeft = s.marginRight = s.marginTop = s.marginBottom = 0.0f;
            s.borderRadius = 2.0f;
            s.backgroundColor = { 1.0f, 1.0f, 1.0f, 1.0f };
            s.textColor = { 0, 0, 0, 1.0f };
            s.fontSize = 14.0f;
            s.textAlign = TextAlign::Left;
            s.horizontalAlign = Align::Start;
            s.verticalAlign = Align::Start;
            s.easing = EasingFn::EaseInOut;
            s.opacity = 1.0f;
            s.isVisible = true;
            s.isEnabled = true;
            s.cursorOnHover = CursorType::Hand;
            s.zIndex = 1.0f;
            return s;
        }
        //  Layout & spacing
        float paddingLeft;
        float paddingRight;
        float paddingTop;
        float paddingBottom;
        float marginLeft;
        float marginRight;
        float marginTop;
        float marginBottom;
        float borderRadius;
        float spacing; // spacing between inner elements

        // Colors
        Color backgroundColor;
        Color textColor;
        Color borderColor;
        Color hoverColor;
        Color activeColor;
        Color disabledColor;


        float borderWidth;
        float outlineWidth;
        Color outlineColor;

        // Font & text
        std::string fontFamily;
        float fontSize;
        bool bold;
        bool italic;
        bool underline;
        TextAlign textAlign; // left, center, right

        // Box sizing and alignment
        float width;
        float height;
        bool autoSize; // shrink to fit content
        Align horizontalAlign; // start, center, end, stretch
        Align verticalAlign;

        //  Animation & transitions (optional)
        float transitionDuration;
        EasingFn easing;

        //  Shadow
        float shadowOffsetX;
        float shadowOffsetY;
        float shadowBlur;
        Color shadowColor;

        //  Cursor / pointer hints
        CursorType cursorOnHover;

        // State flags (optional)
        bool isVisible;
        bool isEnabled;

        //  Miscellaneous
        float opacity;
        float zIndex;

    protected:
    

    };
}