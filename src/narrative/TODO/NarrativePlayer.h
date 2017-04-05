/*
 * NarrativePlayer.h
 *
 *  Created on: Mar 9, 2011
 *      Author: eduardo
 */


#ifndef _NARRATIVEPLAYER_H_
#define _NARRATIVEPLAYER_H_ 1

class Narrative;
class NarrativeNode;
class NarrativeCameraManipulator;
struct NarrativeSelectionInfo;
#include "overlay/OverlayManager.h"

class NarrativePlayer
{
public:

    enum PlayState { TRANSITION, NODE };
    struct PlayStatus
    {
        PlayState state;
        int node;
        double time;
    };

    NarrativePlayer();
    virtual ~NarrativePlayer();

    void init(Narrative* narrative);
    void handleFrame();
    void play();
    void stop();
    void viewNodeAndOverlay(NarrativeSelectionInfo& nsi);
    void hideOverlay(OverlayManager::TransitionType trans);
    bool isPlaying() {
		return m_playing;
	}

	void setLoop(bool loop){m_loop = loop;};

protected:
    osg::ref_ptr<Narrative> m_narrative;
    osg::ref_ptr<NarrativeCameraManipulator> m_manipulator;
    osg::ref_ptr<osgGA::CameraManipulator> m_saved_manipulator;
    bool m_playing;
    double m_prev_time;
    PlayStatus m_play_status;

    osg::Matrixd getMatrix() const;
    void setCamera();
    void nodeReached();
    void nodeLeft();
	bool m_loop;
};

#endif /* _NARRATIVEPLAYER_H_ */
