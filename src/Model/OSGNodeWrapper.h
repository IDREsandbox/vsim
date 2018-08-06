#ifndef OSGNODEWRAPPER_H
#define OSGNODEWRAPPER_H

#include <QObject>
#include <osg/Node>
#include <osg/NodeVisitor>

// wraps the root of an osg tree
// emits signals for custom node changes (begin/end year)
class OSGNodeWrapper : public QObject {
	Q_OBJECT;
public:
	OSGNodeWrapper(QObject *parent = nullptr);

	void setRoot(osg::ref_ptr<osg::Node> node);
	osg::Node *getRoot() const;

	void setNodeYear(osg::Node *node, int year, bool begin);
	static int getNodeYear(osg::Node *node, bool begin, bool *exists = nullptr);

	void enableTime(bool enabled);
	void setYear(int year);
	void applyTime(osg::Node *node = nullptr);

	// TODO:
	void addChild(osg::Group *group, osg::Node *node, size_t index);
	void removeChild(osg::Group *group, size_t index);
	void notifyChanged(osg::Node *node);

signals:
	void sNodeYearChanged(osg::Node *node, int year, bool begin);

	void sAboutToInsertChild(osg::Group *group, size_t index);
	void sInsertedChild(osg::Group *group, size_t index);
	void sAboutToRemoveChild(osg::Group *group, size_t index);
	void sRemovedChild(osg::Group *group, size_t index);

	// just use reset for now
	void sAboutToReset();
	void sReset();

	void sChanged(osg::Node *node);

private:
	osg::ref_ptr<osg::Node> m_root;
	bool m_time_enabled;
	int m_year;
};

// Goes through nodes and initializes yearBegin and yearEnd
// based on names
class TimeInitVisitor : public osg::NodeVisitor {
public:
	TimeInitVisitor(OSGNodeWrapper *base);
	
	// osg overrides
	virtual void apply(osg::Node &node) override;

	// sets begin/end years based on name
	// emits signals in base if relevant
	static void touch(OSGNodeWrapper *base, osg::Node *node);
private:
	OSGNodeWrapper *m_base;
};

// Returns a set of key transition times
class TimeGetVisitor : public osg::NodeVisitor {
public:
	TimeGetVisitor();
	virtual void apply(osg::Node &node) override;
	std::set<int> results() const;
private:
	std::set<int> m_begins;
	std::set<int> m_ends;
	std::set<int> bar;
};

// Hides nodes based on year
class TimeMaskVisitor : public osg::NodeVisitor {
public:
	TimeMaskVisitor(int year);
	virtual void apply(osg::Node &node) override;

	// mask a node based on year
	static void touch(osg::Node &node, int year);

	// shortcut for creating and calling apply()
	static void touchRecursive(osg::Node &node, int year);
private:
	int m_year;
};

class DebugVisitor : public osg::NodeVisitor {
public:
	DebugVisitor();
	virtual void apply(osg::Group &group) override;
	virtual void apply(osg::Node &node) override;

	void print(osg::Node &node) const;
private:
	int m_tabs;
};

#endif
