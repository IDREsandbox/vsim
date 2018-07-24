#ifndef SWITCHWINDOW_H
#define SWITCHWINDOW_H

#include <QListView>

class SwitchListModel;

class SwitchWindow : public QFrame {
	Q_OBJECT;
public:
	SwitchWindow::SwitchWindow(QWidget *parent = nullptr);

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
