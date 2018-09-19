#pragma once
///////////////////////////////////////////////////////////////////////
// Payload.h - Implements Payload for NoSqlDb                        //
// ver 1.0                                                           //
// Language:    Visual C++ 11                                        //
// Application: CSE687 - Object Oriented Design                      //
// Author:      Anuja Mahajan, Syracuse University(MSCS-Spring 2018) //
//				apmahaja@syr.edu									 //
///////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides:
* - Payload class stores information regarding repository like filename, path, desctiption, checkin status

* Required Files:
* ---------------
* Version.h
*
* Maintenance History:
* --------------------
* ver 1.0 : 12 Mar 2018
* - first release
*/



#include <string>
#include <vector>
#include <iostream>
/////////////////////////////////////////////////////////////////////
// Payload class
// - provides fields filename and categories associated with it 
namespace PayLoad
{ 
class Payload
{
public:
	Payload(std::string filePath, std::vector<std::string> categories, std::string author="Anuja Mahajan", std::string status = "Open") {
		filePath_ = filePath;
		categories_ = categories;
		author_ = author;
		status_ = status;
	}

	Payload() {}
	//methods to access payload fields
	std::string filePath() { return filePath_; }
	std::vector<std::string> categories() { return categories_; }
	std::string status() { return status_; }
	std::string author() { return author_;}
	void author(std::string author) { author_ = author; }
	void status(std::string status) { status_ = status; }
	void filePath(const std::string filePath) { filePath_ = filePath; }
	void categories(const std::vector<std::string> categories) { categories_ = categories; }
	void addCategories(std::string category);
	std::string getCategories();
	bool isEmpty() {
		if (filePath_ == "" && categories_.size() == 0)
			return true;
		else
			return false;
	}
private:
	std::vector<std::string> categories_;
	std::string filePath_;
	std::string status_;
	std::string author_;
};


using namespace PayLoad;

// ----< function to add category in payload categories>----------------------------
void Payload::addCategories(std::string category)
{
	std::vector<std::string>::iterator it = std::find(categories_.begin(), categories_.end(), category);
	if (it == categories_.end()) {
		categories_.push_back(category);
	}
}

//----<This method converts payload categories into single string for diaplay purpose >----------------
std::string Payload::getCategories()
{
	std::string result = "";
	int i = 0;
	while (i != categories_.size()) {
		if (categories_.at(i) != "") {
			if (i == categories_.size() - 1) {
				result = result + categories_.at(i);
			}
			else {
				result = result + categories_.at(i) + ',';
			}
		}
		i++;
	}
	return result;
}
}