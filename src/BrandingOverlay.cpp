#include "BrandingOverlay.h"

#include <QGridLayout>
#include <QLabel>

#include "Canvas/CanvasEditor.h"

BrandingOverlay::BrandingOverlay(QWidget * parent)
	: QFrame(parent)
{
	QGridLayout *layout = new QGridLayout(this);
	layout->setMargin(0);

	m_label = new QLabel(this);
	m_label->setText("Editing Branding Overlay");
	m_label->setAttribute(Qt::WA_TransparentForMouseEvents);
	layout->addWidget(m_label, 0, 0);
	m_label->setAlignment(Qt::AlignCenter);
	m_label->setObjectName("label");
	m_label->setStyleSheet("background: rgba(0, 0, 0, 100);"
		"font-size: 12px;"
		"color: white;"
	);

	m_editor = new CanvasEditor(parent);
	layout->addWidget(m_editor, 0, 0);

	enableEditing(false);
}

void BrandingOverlay::enableEditing(bool enable)
{
	m_label->setVisible(enable);
	m_editor->setEditable(enable);
	m_editor->setAttribute(Qt::WA_TransparentForMouseEvents, !enable);
	this->setAttribute(Qt::WA_TransparentForMouseEvents, !enable);
}
