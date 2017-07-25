#include "FirstPersonManipulator.h"
#include <QDebug>
#include <algorithm>
#include "Util.h"

FirstPersonManipulator::FirstPersonManipulator()
{
	stop();

	m_smoothing = .003;

	m_movement_smoothing = .01;
	m_base_speed = 10.0;
	m_speed_click = 0;
}

void FirstPersonManipulator::stop()
{
	m_pos_current = osg::Vec3d();
	m_pos_target = osg::Vec3d();
	m_x_current = 0;
	m_x_target = 0;
	m_y_current = 0;
	m_y_target = 0;
}

void FirstPersonManipulator::update(double dt_sec, KeyTracker *keys) {
	if (keys == nullptr) return;
	
	double dd = getMaxSpeed() * dt_sec; // should be multiplied by the time (13ms or so)
	//qDebug() << "speed" << dd;
	if (keys->keyPressed(Qt::Key_W)) {
		m_pos_target[0] += dd;
	}
	if (keys->keyPressed(Qt::Key_S)) {
		m_pos_target[0] += -dd;
	}
	if (keys->keyPressed(Qt::Key_D)) {
		m_pos_target[1] += -dd;
	}
	if (keys->keyPressed(Qt::Key_A)) {
		m_pos_target[1] += dd;
	}
	if (keys->keyPressed(Qt::Key_Shift)) {
		m_pos_target[2] += dd;
		//qDebug() << "SHIFT";
	}
	if (keys->keyPressed(Qt::Key_Control)) {
		m_pos_target[2] += -dd;
	}

	// Position
	double px, py, pz, tx, ty, tz;
	px = m_pos_current.x();
	py = m_pos_current.y();
	pz = m_pos_current.z();
	tx = m_pos_target.x();
	ty = m_pos_target.y();
	tz = m_pos_target.z();
	//qDebug() << "pxpypz" << px << py << pz;
	//qDebug() << "txtytz" << tx << ty << tz;
	double cx = Util::exponentialSmooth2(&px, tx, m_movement_smoothing, dt_sec, .001);
	double cy = Util::exponentialSmooth2(&py, ty, m_movement_smoothing, dt_sec, .001);
	double cz = Util::exponentialSmooth2(&pz, tz, m_movement_smoothing, dt_sec, .001);
	//qDebug() << "deltas" << cx << cy << cz;

	if (abs(cx) >= .0005) moveForward(cx);
	if (abs(cy) >= .0005) moveRight(-cy);
	if (abs(cz) >= .0005) moveUp(cz);
	// update the vector
	m_pos_current = osg::Vec3d(px, py, pz);

	// Rotation
	double dx = Util::exponentialSmooth2(&m_x_current, m_x_target, m_smoothing, dt_sec, .01);
	double dy = Util::exponentialSmooth2(&m_y_current, m_y_target, m_smoothing, dt_sec, .01);

	if (abs(dx) != 0 || abs(dy) != 0) {
		rotateByPixels(dx, dy);
	}
}

void FirstPersonManipulator::mouseMove(int dx, int dy)
{
	m_x_target += dx;
	m_y_target += dy;
}

void FirstPersonManipulator::accelerate(int ticks)
{
	m_speed_click += ticks;
}

double FirstPersonManipulator::getMaxSpeed()
{
	double max_speed = m_base_speed * pow(pow(2, .25), m_speed_click);
	//qDebug() << "ms" << max_speed;
	return max_speed;
}
