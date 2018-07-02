#ifndef BRANDINGOVERLAY_H
#define BRANDINGOVERLAY_H

#include <QFrame>
#include <QLabel>

class CanvasEditor;

class BrandingOverlay : public QFrame {
	Q_OBJECT;
public:
	BrandingOverlay(QWidget *parent = nullptr);

	void enableEditing(bool enable);

public:
	CanvasEditor *m_editor;
	QLabel *m_label;
};

#endif