#ifndef OSGGROUPMODEL_H
#define OSGGROUPMODEL_H

#include <QAbstractItemModel>
#include <osg/Node>

enum UserDataRole {
	PointerRole = Qt::UserRole
};

// Turn a osg::Node into a Qt model
class OSGGroupModel : public QAbstractItemModel {
	Q_OBJECT
public:
	enum UserDataRole {
		PointerRole = Qt::UserRole
	};
	template <class T>
	static T get(QAbstractItemModel *model, int row) {
		return dynamic_cast<T>(
			static_cast<osg::Node*>(
				model->data(model->index(row, 0), PointerRole).value<void*>()));
	}

	OSGGroupModel(QObject *parent);
	virtual void setNode(osg::Node *group);
	bool rootIncluded() const;
	void includeRoot(bool include);

	// tree like
	osg::Node *getNode(const QModelIndex &index) const;
	QModelIndex indexOf(osg::Node *node) const;
	QModelIndex indexOf(osg::Node *node, int column) const;

	// overrides
	//virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
	virtual QModelIndex parent(const QModelIndex &index) const override;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
	virtual QVariant data(const QModelIndex &index, int role) const override;
	virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

protected:
	osg::ref_ptr<osg::Node> m_node;
	bool m_include_root;
};
#endif