#include <vector>

class Event;
class Style;
class Theme;

namespace Kindle {


	class Widget {
	public:
		Widget() = delete;
		Widget(Widget*, float, float); //the parent and the size of the widget (x size and y size

		bool IsDirty() { return _isDirty; }
		void MarkDirty() { _isDirty = true; }

		

	protected:
		Widget* parent = nullptr;
		std::vector<Widget*> children;
		std::vector<Event*> events;
		float x;
		float y;
		Style* style;
		Theme* theme;
		bool _isDirty;
	};
}