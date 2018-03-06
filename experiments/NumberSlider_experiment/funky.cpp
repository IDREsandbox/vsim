#include <editButtons.h>

editButtons::editButtons(QWidget* parent) : QFrame(parent)
{
	ui.setupUi(this);
	setStyleSheet("background: rgba(0, 0, 0, 70); color: rgb(255, 255, 255); border-color: rgb(238, 238, 238);");
}
