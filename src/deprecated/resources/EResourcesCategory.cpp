/*
 * EResourcesCategory.cpp
 *
 *  Created on: December 30, 2011
 *      Author: Franklin Fang
 */

#include "deprecated/resources/EResourcesCategory.h"
#include <string>

erCategory::erCategory(){
	Red = 0;
	Green = 0;
	Blue = 0;
	Name = "Default Category";
}

erCategory::erCategory(std::string name, unsigned char red, unsigned char green, unsigned char blue){
	Name = name;

	Red = red;
	Blue = blue;
	Green = green;

}

erCategory::~erCategory(){

}

erCategory &erCategory:: operator= (const erCategory & copy){
	
	Name = copy.Name;
	Red = copy.Red;
	Green = copy.Green;
	Blue = copy.Blue;

	return *this;//allow assignments to be chained
}

bool erCategory::operator == (const erCategory &other){
	if((Name == other.Name)&&(Red == other.Red)&&(Green == other.Green)&&(Blue == other.Blue))
		return true;
	return false;
}

bool erCategory::operator != (const erCategory &other){
	if((Name == other.Name)&&(Red == other.Red)&&(Green == other.Green)&&(Blue == other.Blue))
		return false;
	return true;
}