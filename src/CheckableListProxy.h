#ifndef CHECKABLELISTPROXY_H
#define CHECKABLELISTPROXY_H

#include <QAbstractProxyModel>

// This proxy is makes every item of a list (column 0) checkable and adds a row at the top for "(Check All)"
class CheckableListProxy : public QAbstractProxyModel {
	Q_OBJECT
public:
	CheckableListProxy(QObject *parent);

	// Proxy Overrides, hopefully this makes signals ok
	virtual void setSourceModel(QAbstractItemModel *sourceModel) override;
	virtual QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;
	virtual QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;

	// Overrides
	virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
	virtual QModelIndex parent(const QModelIndex &index) const override;
	virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
	virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

	virtual QModelIndex buddy(const QModelIndex &index) const override;

	bool isChecked(int row) const;
	std::vector<bool> getChecked() const;

private:
	void resetChecks();
	void updateTristate();

private:
	std::vector<bool> m_checked;
	int m_checked_count;
	int m_unchecked_count;
	Qt::CheckState m_check_all;
};
#endif