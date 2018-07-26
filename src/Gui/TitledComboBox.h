#ifndef TITLEDCOMBOBOX_H
#define TITLEDCOMBOBOX_H

#include <QComboBox>

class TitledComboBox : public QComboBox {
public:
	TitledComboBox(QWidget *parent = nullptr);

	void setTitle(const QString &title);

protected:
	void paintEvent(QPaintEvent *e) override;

private:
	QString m_title;
};
#endif