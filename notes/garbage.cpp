		//osg::Matrixd matrix = m_window->getViewer()->getCameraManipulator()->getMatrix();
		//osg::Quat quat = matrix.getRotate();
		//double y, p, r;
		//Util::quatToYPR(quat, &y, &p, &r);
		//qDebug() << "ypr" << y*180/M_PI << p*180/M_PI << r*180/M_PI;
		//std::cout << matrix;
		//qDebug() << "3rd col" << matrix(0, 3);

		//osg::Vec3 trans, scale;
		//osg::Quat rot, so;
		//matrix.decompose(trans, rot, scale, so);
		//double roll;
		//osg::Vec3 dir;
		//rot.getRotate(roll, dir);
		//std::cout << "dir " << dir << "\nroll " << roll << "\n";
		//osg::Matrix m;
		//rot.get(m);

		//osg::Matrixd view = m_window->getViewer()->getCameraManipulator()->getInverseMatrix();
		//double distance = 0;
		//osg::Vec3 eye, center, up2;
		//view.getLookAt(eye, center, up2, distance);
		//std::cout << "eye " << eye << "\n";
		//std::cout << "center " << center << "\n";
		//std::cout << "up " << up2 << "\n";
		//std::cout << "distance " << distance << "\n";

		////osg::Vec3d fwd(.5,.5,.7071);
		//osg::Vec3d fwd(0,1,0);
		//fwd.normalize();

		//osg::Vec3d vertical(0, 0, 1);
		//osg::Vec3d right = fwd ^ vertical;
		//right.normalize();

		//osg::Vec3d up = right ^ fwd;
		//up.normalize();

		//osg::Matrixd newmatrix;
		//newmatrix.makeIdentity();
		//newmatrix(0, 0) = right[0];
		//newmatrix(0, 1) = right[1];
		//newmatrix(0, 2) = right[2];
		//newmatrix(1, 0) = up[0];
		//newmatrix(1, 1) = up[1];
		//newmatrix(1, 2) = up[2];
		//newmatrix(2, 0) = -fwd[0];
		//newmatrix(2, 1) = -fwd[1];
		//newmatrix(2, 2) = -fwd[2];
		//std::cout << "newmatrix" << newmatrix << "\n";

		//// current rotation matrix

		//// ypr rotation matrix
		//osg::Matrix m3;
		//Util::YPRToQuat(y, p, r).get(m3);


		//osg::Matrix rotator;
		//osg::Quat(M_PI / 2, osg::Vec3(0, 0, 1)).get(rotator);
		//std::cout << "rotator 90 z" << rotator;


		//std::cout << "reverse matrix " << m3;

		//std::cout << "rotated " << m3 * rotator;

		////osg::Matrix m2;
		////Util::YPRToQuat(M_PI / 4, M_PI / 4, 0).get(m2);
		////std::cout << "ypr to quat 1,1,1\n" << m2;

		////osg::Matrix m3;
		////osg::Quat(0, osg::Vec3(-1, 0, 0), M_PI/4, osg::Vec3(0, 0, 1), M_PI/4, osg::Vec3(.707, -.707, 0)).get(m3);
		////std::cout << "zzz\n" << m3;
		//
		////osg::Quat(0, osg::Vec3(-1, 0, 0), pitch, osg::Vec3(1, 0, 0), yaw, osg::Vec3(0, 0, 1));

		//osg::Matrixd testtrans, testrot;
		////testrot.makeRotate(M_PI/2, 1, 0, 0);
		//testrot.makeRotate(0, 1, 0, 0);
		//testtrans.makeTranslate(0, 0, 0);
		//osg::Matrixd cameraMatrix = newmatrix * testtrans;

		////quat.


		//m_window->getViewer()->getCameraManipulator()->setByMatrix(cameraMatrix);
