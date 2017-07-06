#ifndef VECTORMODEL_HPP
#define VECTORMODEL_HPP
#include <QAbstractItemModel>
#include <QUndoStack>
#include <QDebug>
struct Thing {
	int nverts;
	std::string filename;
};

// creating a model
class VectorModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	VectorModel(std::vector<Thing*> &vector, QUndoStack *stack);

	//explicit VectorModel(const QString &data, QObject *parent = 0);
	QVariant data(const QModelIndex &index, int role) const override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;
	QVariant headerData(int section, Qt::Orientation orientation,
		int role = Qt::DisplayRole) const override;
	QModelIndex index(int row, int column,
		const QModelIndex &parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex &index) const override;
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;

	void poke();

private:
	//void setupModelData(const QStringList &lines, TreeItem *parent);

	//TreeItem *rootItem;

	// the data is stored somewhere else
	std::vector<Thing*> *m_data_ptr;

	// the stack?
	QUndoStack *m_stack;
};

#endif // VECTORMODEL_HPP