#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include <osg/Node>

class Model : public QObject {
	Q_OBJECT
public:
	Model(QObject *parent = nullptr);
	//void loadNode(osg::Node *node); // sets name too
	void setNode(osg::Node *node);
	osg::Node *node() const;

	void setNodeYear(osg::Node *node, int year, bool begin);

	std::string name() const;
	void setName(const std::string &path);
	std::string path() const;
	void setPath(const std::string &path);

signals:
	void sNodeYearChanged(osg::Node *node, int year, bool begin);
	void sChanged();

private:
	osg::ref_ptr<osg::Node> m_node;
	bool m_external;
	std::string m_name;
	std::string m_path;
};

#endif