#include "ModelData.h"

int ModelData::getYearBegin() const
{
	return m_year_begin;
}

void ModelData::setYearBegin(int year)
{
	if (year == 0) {
		return;
	}
	m_year_begin = year;
	emit sYearBeginChange(year);
}

int ModelData::getYearEnd() const
{
	return m_year_end;
}

void ModelData::setYearEnd(int year)
{
	if (year == 0) {
		return;
	}
	m_year_end = year;
	emit sYearEndChange(year);
}
