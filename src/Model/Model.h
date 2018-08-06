#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include <osg/Node>

class Model : public QObject {
	Q_OBJECT
public:
	Model(QObject *parent = nullptr);

	void copyReference(const Model &other);

	osg::Node *node() const;
	osg::ref_ptr<osg::Node> nodeRef() const;

	bool failedToLoad() const;
	void setFailedToLoad(bool failed);

	std::string name() const;
	void setName(const std::string &path);
	std::string path() const;
	//void setPath(const std::string &path);
	//bool relativePath() const;
	//void setRelativePath(bool relative);

	bool embedded() const;
	//void setEmbedded(bool embed);

	// A model can be
	// 1. embedded - node
	//    use embedModel()
	// 2. linked - node, path (absolute, relative), failed_to_load
	//    use setFile()
	// TODO: There is probably a better OOP solution to this

	void embedModel(osg::ref_ptr<osg::Node> node);

	// set model to be non-embedded
	// a null node sets failed_to_load
	void setFile(const std::string &path, osg::ref_ptr<osg::Node> node);

signals:
	//void sNodeYearChanged(osg::Node *node, int year, bool begin);
	void sChanged();

	void sNodeChanged();

private:
	osg::ref_ptr<osg::Node> m_node;
	std::string m_name;
	std::string m_path;
	//bool m_relative;
	bool m_embedded;
	bool m_failed_to_load;
};

#endif