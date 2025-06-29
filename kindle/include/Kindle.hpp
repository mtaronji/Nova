#pragma once
#include "Widget.hpp"


namespace Kindle {

	class Kindle {
		public:
			Kindle() = default;
			Kindle(float, float); //application dimensions
			~Kindle();
		protected:
			Widget* root = nullptr; 
	};
}
