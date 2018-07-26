// Copy paste of a Qt example

#ifndef ELIDEDLABEL_H
#define ELIDEDLABEL_H

#include <QFrame>
#include <QRect>
#include <QResizeEvent>
#include <QString>
#include <QWidget>

class ElidedLabel : public QFrame
{
	Q_OBJECT
	Q_PROPERTY(QString text READ text WRITE setText)

public:
	ElidedLabel(QWidget *parent = nullptr);

	void setText(const QString &text);
	const QString &text() const;

	void setHAlign(Qt::Alignment align);

protected:
	void paintEvent(QPaintEvent *event) override;

private:
	QString m_text;
	Qt::Alignment m_halign;
};

#endif