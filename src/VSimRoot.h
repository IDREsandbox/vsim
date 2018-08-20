#ifndef NARRATIVEROOT_H
#define NARRATIVEROOT_H

#include <osg/Group>
#include <memory>
#include <QObject>

class NarrativeGroup;
class ModelGroup;
class EResourceGroup;
class ECategoryGroup;
class CanvasScene;
class LockTable;
namespace VSim { namespace FlatBuffers {
	struct SettingsT;
	struct LockSettingsT;
}}

class VSimRoot : public QObject {
	Q_OBJECT;
public:
	VSimRoot(QObject *parent = nullptr);
	~VSimRoot();

	void loadOld(osg::Group *old_group);
	// Merges two VSimRoots together
	//void merge(VSimRoot *other);

	// copies another VSimRoot
	// steals some unique_ptrs where QObject is no involved
	void take(VSimRoot *other);

	NarrativeGroup *narratives() const;
	EResourceGroup *resources() const;
	ModelGroup *models() const;
	CanvasScene *branding() const;

	// settings stuff, there is no settings class yet
	void copySettings(const VSimRoot *other);

	// never null
	VSim::FlatBuffers::SettingsT *settings() const;

	void prepareSave();
	void postLoad();

	// TODO FIXME: come up with better interfaces?
	// right now it's basically wrapped struct access
	// there is also the question of authorization
	// alteratives:
	// - lock(LockParams)
	// - lock().pw().settings().current()... named param idiom
	void setLockTable(const LockTable &lock);
	const LockTable *lockTableConst() const;

	//// unlock removes all lock stuff
	//bool unlock(QString pw);

	// returns true if password is successful
	// if not locked, then always true
	//bool hasPassword() const;
	//bool checkPassword(QString password) const;
	//void setPassword(QString password);

	bool settingsLocked() const;
	bool restrictedToCurrent() const; // restrict to current narratives
	bool navigationLocked() const;
	void setSettingsLocked(bool locked);
	void setRestrictToCurrent(bool restrict);
	void setNavigationLocked(bool disable);

	void debug();

	// only kind of works
	void moveAllToThread(QThread *thread);

signals:
	void sLockedChanged(bool locked);
	//void sSettingsLockedChanged(bool locked);
	//void sRestrictToCurrentChanged(bool restrict);
	//void sDisableNavigationChanged(bool disable);

private:
	std::unique_ptr<NarrativeGroup> m_narratives;
	std::unique_ptr<ModelGroup> m_models;
	std::unique_ptr<EResourceGroup> m_resources;
	std::unique_ptr<VSim::FlatBuffers::SettingsT> m_settings;
	std::unique_ptr<CanvasScene> m_branding_canvas;

	LockTable *m_lock;
	bool m_lock_settings;
	bool m_lock_add_remove;
	bool m_lock_navigation;
};

#endif