#ifndef NARRATIVEROOT_H
#define NARRATIVEROOT_H

#include <osg/Group>
#include <memory>
#include <QObject>
#include <QDate>

class NarrativeGroup;
class ModelGroup;
class EResourceGroup;
class ECategoryGroup;
class CanvasScene;
class LockTable;

namespace VSim {
	namespace FlatBuffers {
		struct ModelInformationT;
		struct NavigationSettingsT;
		struct GraphicsSettingsT;
		struct WindowSettingsT;
		struct OtherSettingsT;
		struct ERSettingsT;
	}
}

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
	//void take(VSimRoot *other);
	// doesn't copy osg tree, just uses ref_ptrs
	void copy(VSimRoot *other);

	NarrativeGroup *narratives() const;
	EResourceGroup *resources() const;
	ModelGroup *models() const;
	CanvasScene *branding() const;

	// never null
	//VSim::FlatBuffers::SettingsT &settings() const;
	//VSim::FlatBuffers::OtherSettingsT &OtherSettingsT() const;
	VSim::FlatBuffers::ModelInformationT &modelInformation() const;
	VSim::FlatBuffers::NavigationSettingsT &navigationSettings() const;
	VSim::FlatBuffers::GraphicsSettingsT &graphicsSettings() const;
	VSim::FlatBuffers::WindowSettingsT &windowSettings() const;
	VSim::FlatBuffers::OtherSettingsT &otherSettings() const;
	VSim::FlatBuffers::ERSettingsT &erSettings() const;

	void prepareSave();
	void postLoad();

	// TODO FIXME: come up with better interfaces?
	// right now it's basically wrapped struct access
	// there is also the question of authorization
	// alteratives:
	// - lock(LockParams)
	// - lock().pw().settings().current()... named param idiom
	void setLockTable(const LockTable &lock);
	LockTable *lockTable();
	const LockTable *lockTableConst() const;

	void setNoExpiration();
	void setExpirationDate(QDate date);
	bool expires() const;
	QDate expirationDate() const;  // is null date if invalid

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

	static void copyModelInformation(VSim::FlatBuffers::ModelInformationT &dest,
		const VSim::FlatBuffers::ModelInformationT &src);
	static void copyNavigationSettings(VSim::FlatBuffers::NavigationSettingsT &dest,
		const VSim::FlatBuffers::NavigationSettingsT &src);
	static void copyGraphicsSettings(VSim::FlatBuffers::GraphicsSettingsT &dest,
		const VSim::FlatBuffers::GraphicsSettingsT &src);
	static void copyWindowSettings(VSim::FlatBuffers::WindowSettingsT &dest,
		const VSim::FlatBuffers::WindowSettingsT &src);
	static void copyOtherSettings(VSim::FlatBuffers::OtherSettingsT &dest,
		const VSim::FlatBuffers::OtherSettingsT &src);
	static void copyERSettings(VSim::FlatBuffers::ERSettingsT &dest,
		const VSim::FlatBuffers::ERSettingsT &src);

signals:
	//void sLockedChanged(bool locked);
	void sSettingsLockedChanged(bool locked);
	void sRestrictToCurrentChanged(bool restrict);
	void sDisableNavigationChanged(bool disable);

private:
	std::unique_ptr<NarrativeGroup> m_narratives;
	std::unique_ptr<ModelGroup> m_models;
	std::unique_ptr<EResourceGroup> m_resources;
	//std::unique_ptr<VSim::FlatBuffers::SettingsT> m_settings;
	std::unique_ptr<VSim::FlatBuffers::ModelInformationT> m_model_information;
	std::unique_ptr<VSim::FlatBuffers::NavigationSettingsT> m_navigation_settings;
	std::unique_ptr<VSim::FlatBuffers::GraphicsSettingsT> m_graphics_settings;
	std::unique_ptr<VSim::FlatBuffers::WindowSettingsT> m_window_settings;
	std::unique_ptr<VSim::FlatBuffers::OtherSettingsT> m_other_settings;
	std::unique_ptr<VSim::FlatBuffers::ERSettingsT> m_er_settings;
	std::unique_ptr<CanvasScene> m_branding_canvas;

	LockTable *m_lock;
	bool m_lock_settings;
	bool m_lock_add_remove;
	bool m_lock_navigation;
	QDate m_expiration_date;
};

#endif