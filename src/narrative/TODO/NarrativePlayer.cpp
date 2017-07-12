// /*
 // * NarrativePlayer.cpp
 // *
 // *  Created on: Mar 9, 2011
 // *      Author: eduardo
 // */

// #include "util.h"
// #include "menu/Settings.h"
// #include "narrative/Narrative.h"
// #include "narrative/NarrativeNode.h"
// #include "narrative/NarrativeList.h"
// #include "narrative/NarrativeEditor.h"
// #include "narrative/NarrativeListEditor.h"
// #include "narrative/NarrativeCameraManipulator.h"
// #include "narrative/NarrativePlayer.h"
// #include "NRTSViewer.h"
// #include "VSimApp.h"
// #include "narrative/NarrativeManager.h"
// #include "overlay/OverlayManager.h"

// using namespace osg;
// using namespace osgGA;

// NarrativePlayer::NarrativePlayer():
    // m_narrative(NULL), m_playing(false), m_prev_time(0.0), m_loop(false)
// {
    // m_manipulator = new NarrativeCameraManipulator(this);
// }


// NarrativePlayer::~NarrativePlayer()
// {
// }

// void NarrativePlayer::init(Narrative* narrative)
// {
    // m_narrative = narrative;
    // m_prev_time = 0.0;
// }

// void NarrativePlayer::handleFrame()
// {
    // assert(m_playing);
    // double time = g_viewer->getFrameStamp()->getReferenceTime();
    // double dt = time - m_prev_time;
    // assert(time > 0.0);
    // m_prev_time = time;
    // m_play_status.time += dt;
    // if (m_play_status.state == TRANSITION)
    // {
        // NarrativeTransition* trans = m_narrative->getTransition(m_play_status.node);
        // if (m_play_status.time >= trans->getDuration())
        // {
            // m_play_status.time = 0.0;
            // m_play_status.node++;
            // m_play_status.state = NODE;
            // nodeReached();
			// if (m_narrative->getNode(m_play_status.node)->getStayOnNode())
				// stop();
        // }
    // }
    // else if (m_play_status.state == NODE)
    // {
        // NarrativeNode* node = m_narrative->getNode(m_play_status.node);
        // if (m_play_status.time >= node->getPauseAtNode())
        // {
            // m_play_status.time = 0.0;
            // nodeLeft();
            // if (m_play_status.node == (int)m_narrative->getNumNodes() - 1)
            // {
				// if(!m_loop){
					// m_narrative->setSelection(0,false);
					// stop();
					
				// }
				// else{
					// //if reached the last narrative, return to the first narrative, keep playing
					// //if hasn't reached the last narrative, keep playing the next narrative
					// NarrativeListEditor* narrativelisteditor = g_narrative_manager->getNarrativeListEditor();
					// NarrativeList* narrativelist = narrativelisteditor->getNarrativeList();
					// if( narrativelist->getSelection() < (int)(narrativelist->getNumNarratives())-1)
						// narrativelisteditor->changeSelection(narrativelist->getSelection()+1);
					// else
						// narrativelisteditor->changeSelection(0);
					

					// m_narrative = narrativelist->getSelectedNarrativeReference()->getNarrative();
					// g_narrative_manager->getNarrativeEditor()->showNarrative2(narrativelist->getSelectedNarrativeReference());

					// std::string show_text = "Playing next Narrative: " + m_narrative->getName();
					// osgNewWidget::WidgetManager::instance()->setInfoBox(show_text, osgNewWidget::UP_RIGHT);


					// m_play_status.node = 0;
					// m_play_status.state = NODE;
					// nodeReached();
				// }
            // }
            // else
            // {
                // m_play_status.state = TRANSITION;
            // }
        // }
    // }
    // setCamera();
// }

// void NarrativePlayer::play()
// {
	// if(!m_narrative)//no narrative initialized
		// return;
    // if (m_playing)
        // return;
	// std::string show_text = "Playing Narrative: " + m_narrative->getName();
	// osgNewWidget::WidgetManager::instance()->setInfoBox(show_text, osgNewWidget::UP_RIGHT);

    // g_narrative_manager->highlightPlayWidget(true);
    // g_narrative_manager->setMultiBarEnabled(false);
    // m_playing = true;
    // assert(m_saved_manipulator == NULL);
    // m_prev_time = g_viewer->getFrameStamp()->getReferenceTime();
    // m_saved_manipulator = g_viewer->getCameraManipulator();
    // g_viewer->setCameraManipulator(m_manipulator, false);
    // m_play_status.node = m_narrative->getSelection().node;
    // m_play_status.time = 0.0;
    // m_play_status.state = m_narrative->getSelection().isTransitionSelected ? TRANSITION : NODE;
	// NarrativeNode* node = m_narrative->getNode(m_play_status.node);
    // if (m_play_status.state == NODE)
	// {
		// // If it's a StayOnNode, ignore node's PauseAtNode time (start moving immediately).
		// bool stay = node->getStayOnNode();
		// if (stay)
			// m_play_status.time = node->getPauseAtNode();
		// //nodeReached();
	// }
    // else
	// {
        // //nodeLeft();
	// }
    // setCamera();
// }

// void NarrativePlayer::stop()
// {
    // if (!m_playing)
        // return;
    // g_narrative_manager->highlightPlayWidget(false);
    // g_narrative_manager->setMultiBarEnabled(true);
    // m_playing = false;
    // assert(m_saved_manipulator != NULL);
    // m_saved_manipulator->setByMatrix(m_manipulator->getMatrix());
    // g_viewer->setCameraManipulator(m_saved_manipulator, false);
    // m_saved_manipulator = NULL;
// }

// void NarrativePlayer::setCamera()
// {
    // osg::Matrixd matrix = getMatrix();
    // m_manipulator->setByMatrix(matrix);
// }

// osg::Matrixd NarrativePlayer::getMatrix() const
// {
    // assert(m_play_status.node < (int)m_narrative->getNumNodes());
    // if (m_play_status.state == NODE)
    // {
        // return m_narrative->getNode(m_play_status.node)->getViewMatrix();
    // }
    // NarrativeTransition* trans = m_narrative->getTransition(m_play_status.node);
    // double t;
    // if (trans->getDuration() < 0.0001)
    // {
        // t = 1.0;
    // }
    // else
    // {
        // double t0 = m_play_status.time / trans->getDuration();
        // assert(t0 >= 0.0 && t0 <= 1.0);
        // static const double r = sqrt(2.0/3.0);
        // static const double d = r - (r * r * r * 0.5);
        // t0 = (t0 - 0.5) * 2.0 * r;
        // t = t0 - (t0 * t0 * t0 * 0.5);
        // t /= d;
        // t *= 0.5;
        // t += 0.5;
    // }
    // assert(t >= -0.0000001 && t <= 1.0000001);

    // const osg::Matrixd& matrix0 = m_narrative->getNode(m_play_status.node)->getViewMatrix();
    // const osg::Matrixd& matrix1 = m_narrative->getNode(m_play_status.node + 1)->getViewMatrix();
    // osg::Vec3d pos0 = matrix0.getTrans();
    // osg::Vec3d pos1 = matrix1.getTrans();
    // osg::Vec3d pos = pos0 * (1.0 - t) + pos1 * t;
    // osg::Quat ori0 = matrix0.getRotate();
    // osg::Quat ori1 = matrix1.getRotate();
    // osg::Quat ori;
    // ori.slerp(t, ori0, ori1);
    // // Remove banking component
    // // TODO: use CoordinateFrame
    // Util::fixVerticalAxis(ori, osg::Vec3d(0.0, 0.0, 1.0), false);
    // return osg::Matrixd::rotate(ori) * osg::Matrixd::translate(pos);
// }

// void NarrativePlayer::nodeReached()
// {
    // NarrativeSelectionInfo nsi;
    // nsi.node = m_play_status.node;
    // nsi.isTransitionSelected = false;
    // g_vsimapp->getNarrativeManager()->changeSelection(nsi);
    // viewNodeAndOverlay(nsi);
// }

// void NarrativePlayer::nodeLeft()
// {
    // hideOverlay(OverlayManager::FADE);
    // if (m_play_status.node < (int)m_narrative->getNumNodes() - 1)
    // {
        // NarrativeSelectionInfo nsi;
        // nsi.node = m_play_status.node;
        // nsi.isTransitionSelected = true;
        // g_vsimapp->getNarrativeManager()->changeSelection(nsi);
    // }
// }

// void NarrativePlayer::viewNodeAndOverlay(NarrativeSelectionInfo& nsi)
// {
    // if (nsi.node < 0)
        // return;
    // NarrativeNode* node = m_narrative->getNode(nsi.node);
    // g_viewer->getCameraManipulator()->setByMatrix(node->getViewMatrix());
    // if (!nsi.isTransitionSelected)
        // g_vsimapp->getOverlayManager()->showOverlayContent(node, OverlayManager::FADE);
    // else
        // hideOverlay(OverlayManager::NO_FADE);
// }

// void NarrativePlayer::hideOverlay(OverlayManager::TransitionType trans)
// {
    // g_vsimapp->getOverlayManager()->hideOverlayContent(trans);
// }
