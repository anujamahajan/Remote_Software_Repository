#pragma once
///////////////////////////////////////////////////////////////////////
// RepositoryCore.h - Implements Prototype for repository            //
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
* - Repository Class which implements methods to perform basic operations on repository like checkin, checkout, browse
* - It uses CheckIn, CheckOut, Brwose class methods to perform repsective operations

* Required Files:
* ---------------
* CheckIn.h, CheckOut.h, Browse.h
*
* Maintenance History:
* --------------------
* ver 1.0 : 12 Mar 2018
* - first release
* ver 1.1 : 30 Apr 2018
* - Added browse functions to suport mosql db queries
*/

#include "../CheckIn/CheckIn.h"
#include "../CheckOut/CheckOut.h"
#include "../Browse/Browse.h"

/////////////////////////////////////////////////////////////////////
// Repository class
// - This class provides methods to perform basic operations in repository like checkin, chckout and browse.

template<typename T>
class RepositoryCore
{
public:
	RepositoryCore<T>(DbCore<T>& db) : dbCore(db) {}
	void checkInRep(std::string filePath, std::string package, std::string description, vector<std::string> categories);
	void checkInDependent(std::string filePath, std::string package, unordered_map<std::string, std::string> children, std::string description, vector<std::string> categories);
	void closeCheckIn(std::string filePath);
	void checkOut(std::string fileName, std::string package);
	void checkOut(std::string fileName,std::string package, int version);
	void checkOut(std::string fileName, std::string package, int version, bool isCopyDependent);
	void checkOut(std::string fileName, std::string package, bool isCopyDependent);
	//void browse(std::string fileName, std::string package);
	void browse(std::string fileName, std::string package, int version);
	vector<std::string> browse(std::string condition, std::string value);
	std::vector<std::string> browse(unordered_map<std::string, std::string> filterCondition);
	std::string getFilePath(std::string key);
	static void identify(std::ostream& out = std::cout);
	vector<std::string> getChildren(std::string fileName, std::string package);
	vector<std::string> getChildren(std::string fileName, std::string package, int version);
	DbElement<T> getElement(std::string fileName);
	vector<std::string> getFilesWithNoParents();

private:
	DbCore<T>& dbCore;
};

// ----< method to perform checkin with given filename and package>-------------------
template<typename T>
void RepositoryCore<T>::checkInRep(std::string filePath, std::string package,std::string description, vector<std::string> categories)
{
	CheckIn<T> check(dbCore);
	check.checkIn(filePath, package, description, categories);
}

// ----< method to checkin file with dependent in repository>-------------------
template<typename T>
void RepositoryCore<T>::checkInDependent(std::string filePath, std::string package, unordered_map<std::string,std::string> children,std::string description, vector<std::string> categories)
{
	CheckIn<T> check(dbCore);
	check.checkIn(filePath, package, children, description, categories);
}

// ----< method to perform close checkin>-------------------
template<typename T>
void RepositoryCore<T>::closeCheckIn(std::string filePath)
{
	CheckIn<T> check(dbCore);
	check.closeCheckIn(filePath);
}

// ----< method to perform checkout>-------------------
template<typename T>
void RepositoryCore<T>::checkOut(std::string fileName, std::string package)
{
	std::cout << "\n  Checking out file: " +fileName + " from package: "+package + " with latest version with dependent files";
	CheckOut<T> checkOut(dbCore);
	checkOut.checkOut(fileName, package);
}

// ----< method to perform checkout>-------------------
template<typename T>
void RepositoryCore<T>::checkOut(std::string fileName, std::string package, int version)
{
	std::cout << "\n  Checking out file: " + fileName + " from package: " + package + " with " + std::to_string(version) + " version with dependent files";
	CheckOut<T> checkOut(dbCore);
	checkOut.checkOut(fileName, package, version);
}

// ----< method to perform checkout>-------------------
template<typename T>
void RepositoryCore<T>::checkOut(std::string fileName, std::string package, int version, bool isCopyDependent)
{
	if (isCopyDependent) {
		checkOut(fileName, package, version);
	}
	else {
		std::cout << "\n  Checking out file: " + fileName + " from package: " + package + " with " + std::to_string(version) + " version without dependent files";
		CheckOut<T> checkOut(dbCore);
		checkOut.checkOut(fileName, package, version, false);
	}	
}

// ----< method to perform checkout>-------------------
template<typename T>
void RepositoryCore<T>::checkOut(std::string fileName, std::string package, bool isCopyDependent)
{
	if (isCopyDependent) {
		checkOut(fileName, package);
	}
	else {
		std::cout << "\n  Checking out file: " + fileName + " from package: " + package + " with latest version without dependent files";
		CheckOut<T> checkOut(dbCore);
		checkOut.checkOut(fileName, package, false);
	}
}

// ----< method to perform checkout>----------------
template<typename T>
vector<std::string> RepositoryCore<T>::getChildren(std::string fileName, std::string package)
{
	CheckOut<T> checkOut(dbCore);
	return checkOut.getChildren(fileName, package);
}

// ----< method to perform checkout>----------------
template<typename T>
vector<std::string> RepositoryCore<T>::getChildren(std::string fileName, std::string package, int version)
{
	CheckOut<T> checkOut(dbCore);
	return checkOut.getChildren(fileName, package, version);
}

// ----< method to perform browse>-------------------
/*template<typename T>
void RepositoryCore<T>::browse(std::string fileName, std::string package)
{
	Browse<T> browse(dbCore);
	browse.browse(fileName,package);
}*/

// ----< method to perform checkout>----------------
template<typename T>
void RepositoryCore<T>::browse(std::string fileName, std::string package,int version)
{
	Browse<T> browse(dbCore);
	browse.browse(fileName, package, version);
}

template<typename T>
std::vector<std::string> RepositoryCore<T>::browse(std::string condition, std::string value)
{
	Browse<T> browse(dbCore);
	return browse.browse(condition, value);
}
template<typename T>
std::vector<std::string> RepositoryCore<T>::browse(unordered_map<std::string,std::string> filterCondition)
{
	Browse<T> browse(dbCore);
	return browse.browse(filterCondition);
}

template<typename T>
std::string RepositoryCore<T>::getFilePath(std::string key)
{
	Browse<T> browse(dbCore);
	return browse.getFilePath(key);
}

template<typename T>
DbElement<T> RepositoryCore<T>::getElement(std::string fileName)
{
	Browse<T> browse(dbCore);
	return browse.getElement(fileName);
}

template<typename T>
vector<std::string> RepositoryCore<T>::getFilesWithNoParents()
{
	Browse<T> browse(dbCore);
	return browse.getFilesWithNoParents();
}

// ----< method to print current file path>--------------
template<typename T>
void RepositoryCore<T>::identify(std::ostream& out)
{
	out << "\n  \"" << __FILE__ << "\"";
}