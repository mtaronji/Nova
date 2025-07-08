#pragma once

namespace Kindle {
	class Widget;
	class UI {
		public:
			UI() = default;
			UI(float, float); //application dimensions
			~UI();
		protected:
			Widget* root = nullptr; 
	};
}
