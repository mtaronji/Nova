#include "Widget.hpp"

Kindle::Widget::Widget(Widget* parent, float x, float y, Kindle::Style style, Kindle::Layout layout){
	this->parent = parent;
	this->rect = { .x = x, .y = y };
	this->style = style;
	this->layout = std::move(layout);


}
Kindle::Widget::~Widget() {

}