#include "narrative/NarrativeModel.h"
#include "Util.h"
#include "RegisterMetaTypes.h"

NarrativeModel::NarrativeModel(QObject *parent, osg::Group *narratives, QUndoStack *stack)
	: QAbstractItemModel(parent),
	m_root(narratives),
	m_stack(stack)
{

}

QVariant NarrativeModel::data(const QModelIndex & index, int role) const
{
	if (!index.isValid()) {
		qDebug() << "NarrativeModel - invalid data request";
		return QVariant();
	}
	if (role != Qt::DisplayRole) return QVariant();

	osg::Node *node = static_cast<osg::Node*>(index.internalPointer());

	Narrative2 *nar_ptr = dynamic_cast<Narrative2*>(node);
	if (nar_ptr) {
		switch (index.column()) {
		case 0:
			return QString::fromStdString(nar_ptr->getTitle());
		case 1:
			return QString::fromStdString(nar_ptr->getDescription());
		case 2:
			return QString::fromStdString(nar_ptr->getAuthor());
		default:
			qDebug() << "narrative invalid column";
			return QVariant();
		}
	}
	NarrativeSlide *slide_ptr = dynamic_cast<NarrativeSlide*>(node);
	QVariant v;
	if (slide_ptr) {
		//osg::Matrixd m_camera_matrix;
		//float m_duration;
		//bool m_stay_on_node;
		//float m_transition_duration;
		//osg::ref_ptr<osg::Image> m_thumbnail;
		switch (index.column()) {
		case 0:
			v.setValue<osg::Matrixd>(slide_ptr->getCameraMatrix());
			return v;
		case 1:
			return slide_ptr->getDuration();
		case 2:
			return slide_ptr->getStayOnNode();
		case 3:
			return slide_ptr->getTransitionDuration();
		case 4:
			return Util::imageOsgToQt(slide_ptr->getThumbnail());
		default:
			return QVariant();
		}
	}
	NarrativeSlideLabels *label_ptr = dynamic_cast<NarrativeSlideLabels*>(node);
	if (label_ptr) {
		//float m_rX;
		//float m_rY;
		//float m_rW;
		//float m_rH;
		//std::string m_text;
		//std::string m_style;
		switch (index.column()) {
		case 0:
			return label_ptr->getrX();
		case 1:
			return label_ptr->getrY();
		case 2:
			return label_ptr->getrW();
		case 3:
			return label_ptr->getrH();
		case 4:
			return QString::fromStdString(label_ptr->getText());
		case 5:
			return QString::fromStdString(label_ptr->getStyle());
		}
	}

	qDebug() << "Narrative model - get data for invalid node type";
	return QVariant();
}

Qt::ItemFlags NarrativeModel::flags(const QModelIndex & index) const
{
	return Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
}

QVariant NarrativeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	return QVariant();
}

QModelIndex NarrativeModel::index(int row, int column, const QModelIndex & parent) const
{
	osg::Group *parent_group = nullptr;

	if (parent == QModelIndex()) {
		parent_group = m_root;
	}
	else {
		osg::Node *parent_node = static_cast<osg::Node*>(parent.internalPointer());
		parent_group = dynamic_cast<osg::Group*>(parent_node);
	
		if (parent_group == nullptr) {
			qWarning() << "Narrative model index request with a non-group parent";
			return QModelIndex();
		}
	}	
	return createIndex(row, column, parent_group->getChild(row));
}

QModelIndex NarrativeModel::parent(const QModelIndex & index) const
{
	osg::Node *node = static_cast<osg::Node*>(index.internalPointer());
	return indexOf(node->getParent(0));
}

int NarrativeModel::rowCount(const QModelIndex & parent) const
{
	if (parent == QModelIndex()) return m_root->getNumChildren();
	osg::Node *node = static_cast<osg::Node*>(parent.internalPointer());
	osg::Group *group = dynamic_cast<osg::Group*>(node);
	if (group == nullptr) {
		qWarning() << "Narrative model row request with a non-group parent";
		return 0;
	}
	return group->getNumChildren();
}

int NarrativeModel::columnCount(const QModelIndex & parent) const
{
	// if the parent is root then we want Narrative columns
	if (!parent.isValid()) {
		return 3;
	}
	osg::Node *node = static_cast<osg::Node*>(parent.internalPointer());
	Narrative2 *nar_ptr = dynamic_cast<Narrative2*>(node);
	NarrativeSlide *slide_ptr = dynamic_cast<NarrativeSlide*>(node);
	// Narrative parent -> Slide columns
	if (nar_ptr) {
		return 5;
	}
	// Slide parent -> Label columns
	else if (slide_ptr) {
		return 6;
	}
	// error
	qWarning() << "Narrative model column request with invalid node type";
	return 0;
}

bool NarrativeModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
	if (role != Qt::EditRole && role != Qt::DisplayRole) return false;
	int col = index.column();
	int row = index.row();

	qDebug() << "set data" << "value" << value << "row" << row << "col" << col << "role" << Qt::DisplayRole;
	
	osg::Node *node = static_cast<osg::Node*>(index.internalPointer());
	Narrative2 *nar_ptr = dynamic_cast<Narrative2*>(node);
	NarrativeSlide *slide_ptr = dynamic_cast<NarrativeSlide*>(node);
	NarrativeSlideLabels *label_ptr = dynamic_cast<NarrativeSlideLabels*>(node);
	if (nar_ptr) {
		switch (col) {
		case 0:
			nar_ptr->setTitle(value.toString().toStdString());
			break;
		case 1:
			nar_ptr->setDescription(value.toString().toStdString());
			break;
		case 2:
			nar_ptr->setAuthor(value.toString().toStdString());
			break;
		default:
			return false;
		}
	}
	else if (slide_ptr) {
		switch (col) {
			//osg::Matrixd m_camera_matrix;
			//float m_duration;
			//bool m_stay_on_node;
			//float m_transition_duration;
			//osg::ref_ptr<osg::Image> m_thumbnail;
		case 0:
			slide_ptr->setCameraMatrix(value.value<osg::Matrixd>());
			break;
		case 1:
			slide_ptr->setDuration(value.toDouble());
			break;
		case 2:
			slide_ptr->setStayOnNode(value.toBool());
			break;
		case 3:
			slide_ptr->setTransitionDuration(value.toDouble());
			break;
		case 4:
			slide_ptr->setThumbnail(Util::imageQtToOsg(value.value<QImage>()));
			break;
		default:
			return false;
		}
	}
	else if (label_ptr) {
		switch (col) {
		case 0:
			label_ptr->setrX(value.toFloat());
			break;
		case 1:
			label_ptr->setrY(value.toFloat());
			break;
		case 2:
			label_ptr->setrW(value.toFloat());
			break;
		case 3:
			label_ptr->setrH(value.toFloat());
			break;
		case 4:
			label_ptr->setText(value.toString().toStdString());
			break;
		case 5:
			label_ptr->setStyle(value.toString().toStdString());
			break;
		default:
			return false;
		}
	}
	else {
		qWarning() << "Narrative model invalid node";
		return false;
	}
	// if we made it to a break then it was a success
	emit dataChanged(index, index);
	return true; 
}

bool NarrativeModel::insertRows(int row, int count, const QModelIndex & parent)
{
	osg::Node *node = static_cast<osg::Node*>(parent.internalPointer());
	Narrative2 *nar_ptr = dynamic_cast<Narrative2*>(node);
	NarrativeSlide *slide_ptr = dynamic_cast<NarrativeSlide*>(node);
	
	// insert narrative
	if (!parent.isValid()) {
		for (int i = 0; i < count; i++) {
			m_root->insertChild(row + i, new Narrative2);
		}
	}
	// insert slides
	else if (nar_ptr != nullptr) {
		for (int i = 0; i < count; i++) {
			nar_ptr->insertChild(row + i, new NarrativeSlide);
		}
	}
	// insert labels
	else if (slide_ptr != nullptr) {
		for (int i = 0; i < count; i++) {
			slide_ptr->insertChild(row + i, new NarrativeSlideLabels);
		}
	}
	else {
		return false;
	}
	return true;
}

bool NarrativeModel::moveRows(const QModelIndex & sourceParent, int sourceRow, int count, const QModelIndex & destinationParent, int destinationRow)
{
	qDebug() << "Move rows";

	// Both parents are the root -> moving Narratives
	if (!sourceParent.isValid() && !destinationParent.isValid()) {
		int old_size = rowCount(destinationParent);
		int new_size;
		if (sourceParent == destinationParent) {
			new_size = old_size;
		}
		else {
			new_size = old_size + count;
		}

		// moving to index out of range

		if (destination)
		
		std::vector<osg::ref_ptr<osg::Node*>> nodes;
		nodes.push_

	}
	// try all of the type casts
	osg::Node *source_node = static_cast<osg::Node*>(sourceParent.internalPointer());
	Narrative2 *source_nar = dynamic_cast<Narrative2*>(source_node);
	NarrativeSlide *source_slide = dynamic_cast<NarrativeSlide*>(source_node);
	osg::Node *dest_node = static_cast<osg::Node*>(destinationParent.internalPointer());
	Narrative2 *dest_nar = dynamic_cast<Narrative2*>(source_node);;
	NarrativeSlide *dest_slide = dynamic_cast<NarrativeSlide*>(source_node);;

	// Both parents are Narratives -> moving Slides
	if (source_nar && dest_nar) {

	}

	// Both parents are Slides -> moving Labels
	if (source_slide && dest_slide) {

	}


	return false;
}

bool NarrativeModel::removeRows(int row, int count, const QModelIndex & parent)
{
	if (row < 0) return false;
	if (count < 0) return false;
	if (row + count >= rowCount(parent)) return false;

	qDebug() << "remove rows" << "row" << row << "count" << count;
	osg::Group *group;
	if (!parent.isValid()) {
		group = m_root;
	}
	else {
		osg::Node *node = static_cast<osg::Node*>(parent.internalPointer());
		group = dynamic_cast<osg::Group*>(node);
	}
	group->removeChildren(row, count);
	return true;
}

Qt::DropActions NarrativeModel::supportedDragActions() const
{
	return Qt::CopyAction | Qt::MoveAction;
}

Qt::DropActions NarrativeModel::supportedDropActions() const
{
	return Qt::CopyAction | Qt::MoveAction;
}

QModelIndex NarrativeModel::indexOf(osg::Node *node) const
{
	if (node == nullptr) {
		qDebug() << "null indexof";
		return QModelIndex();
	}
	if (node->getParent(0) == m_root) {
		// i am root;
		return QModelIndex();
	}
	size_t row = node->getParent(0)->getChildIndex(node);
	return createIndex(row, 0, node);
}

void NarrativeModel::debug()
{
	qInfo() << "Narratives:" << m_root->getNumChildren();
	for (uint i = 0; i < m_root->getNumChildren(); i++) {
		osg::Node *node = m_root->getChild(i);
		Narrative2 *nar = dynamic_cast<Narrative2*>(node);
		if (!nar) {
			qInfo() << "  " << i << "Error type";
			continue;
		}
		qInfo() << "  " << i << QString::fromStdString(nar->getTitle())
			<< QString::fromStdString(nar->getDescription())
			<< QString::fromStdString(nar->getAuthor());
	
		for (uint j = 0; j < nar->getNumChildren(); j++) {
			osg::Node *node = nar->getChild(j);
			NarrativeSlide *slide = dynamic_cast<NarrativeSlide*>(node);

			if (!slide) {
				qInfo() << "    " << j << "Error type";
				continue;
			}
			qInfo() << "    " << j << slide->getDuration() << slide->getStayOnNode() << slide->getTransitionDuration();
		}	
	}
}
