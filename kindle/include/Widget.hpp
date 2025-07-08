#include <vector>
#include <memory>
#include "Style.hpp"
#include "Layout.hpp"

//ui for my mesh creation tool


namespace Kindle {

	class Event;

	class Widget {
	public:

		struct Rect {
			float x;
			float y;
		};

		Widget() = delete;
		Widget(Widget*, float, float, Kindle::Style, Kindle::Layout); //the parent and the size of the widget (x size and y size)
		~Widget();

		bool IsDirty() { return _isDirty; }
		void MarkDirty() { _isDirty = true; }
		void MarkClean() { _isDirty = false; }
		float GetWidth() { return rect.x; }
		float GetHeight() { return rect.y; }
		Rect GetRect() { return rect; }
		

	protected:
		Widget* parent = nullptr;
		std::vector<Widget*> children;
		std::vector<Event*> events;
		Rect rect;
		Kindle::Style style;
	    Kindle::Layout layout;
		bool _isDirty = true; 
	};
}