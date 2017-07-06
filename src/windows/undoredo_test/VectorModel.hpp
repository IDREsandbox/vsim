#ifndef VECTORMODEL_HPP
#define VECTORMODEL_HPP
#include <QAbstractItemModel>
#include <QUndoStack>
#include <QDebug>

struct Other;


struct Node {
	Node *parent = nullptr;
	Node(Node *p) : parent(p) {}
	Node() : parent(nullptr) {}
	virtual ~Node() = 0 {}
};

struct Thing : public Node {
	int nverts;
	std::string filename;
	std::vector<Other*> children;

	Thing(int n, std::string f)
		: Node(), nverts(n), filename(f) {}
};

struct Other : public Node {
	int hairstyle;
	std::string pajamas;
	std::string three;

	Other(Node *p, int h, std::string pa, std::string t)
		: Node(p), hairstyle(h), pajamas(pa), three(t) {}
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