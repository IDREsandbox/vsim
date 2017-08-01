#include "SimpleCameraManipulator.h"
void SimpleCameraManipulator::setByMatrix(const osg::Matrixd & matrix)
{
	m_inverse_matrix = osg::Matrixd::inverse(matrix);
}

void SimpleCameraManipulator::setByInverseMatrix(const osg::Matrixd & matrix)
{
	m_inverse_matrix = matrix;
}

osg::Matrixd SimpleCameraManipulator::getMatrix() const
{
	return osg::Matrix::inverse(m_inverse_matrix);
}

osg::Matrixd SimpleCameraManipulator::getInverseMatrix() const
{
	return m_inverse_matrix;
}
