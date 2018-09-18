#ifndef SWITCHWINDOW_H
#define SWITCHWINDOW_H

#include <QListView>
#include <QStyledItemDelegate>

class SwitchListModel;

class SwitchColorDelegate : public QStyledItemDelegate {

public:

	SwitchColorDelegate(QObject *parent = 0);

	void paint(QPainter *painter, const QStyleOptionViewItem &option,
		const QModelIndex &index) const override;
	bool editorEvent(QEvent *event, QAbstractItemModel *model,
		const QStyleOptionViewItem &option, const QModelIndex &index) override;

	//QSize sizeHint(const QStyleOptionViewItem &option,
	//	const QModelIndex &index) const override;
	//QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
	//	const QModelIndex &index) const override;
	//void setEditorData(QWidget *editor, const QModelIndex &index) const override;
	//void setModelData(QWidget *editor, QAbstractItemModel *model,
	//	const QModelIndex &index) const override;
};

class SwitchWindow : public QFrame {
	Q_OBJECT;
public:
	SwitchWindow(QWidget *parent = nullptr);

	void setModel(SwitchListModel *model);


	void onSwitchChanged(const QModelIndex &current);

private:
	class SwitchListView : public QListView {
	public:
		SwitchListView(SwitchWindow *window);
	protected:
		void currentChanged(const QModelIndex &current, const QModelIndex &previous) override;
	private:
		SwitchWindow *m_window;
	};

private:
	SwitchListModel *m_model;

	SwitchListView *m_switch_view;
	QListView *m_nodes_view;
	QListView *m_sets_view;
};

#endif
