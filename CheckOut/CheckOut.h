#pragma once
///////////////////////////////////////////////////////////////////////
// CheckOut.h - Implements Checkout functionlity for repository       //
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
* - Checkout class which provides functionality of checking out file sfrom repository
* - It will copy files from repository in local directory removing version from file names.
* - It will copy latest version of the given file if not specified along with its dependent files

* Required Files:
* ---------------
* Version.h
*
* Maintenance History:
* --------------------
* ver 1.0 : 12 Mar 2018
* - first release
* ver 1.1 : 30 Apr 2018
* - modified to get child files of specified file
*/

#include <iostream> 
#include <string>
#include "../FileSystem/FileSystem.h"
#include "../Query/Query.h"

using namespace FileSystem;
/////////////////////////////////////////////////////////////////////
// CheckOut class
// Provides functionality of checking out file sfrom repository
// It will copy files from repository in local directory removing version from file names.
// It will copy latest version of the given file if not specified along with its dependent files

template<typename T>
class CheckOut
{
public:
	CheckOut<T>(DbCore<T>& db) : dbCore(db) {};
	void checkOut(std::string fileName, std::string package,int version);
	void checkOut(std::string fileName, std::string package);
	void checkOut(std::string fileName, std::string package,int version, bool isCopyDependent);
	void checkOut(std::string fileName, std::string package, bool isCopyDependent);
	vector<std::string> getChildren(std::string fileName, std::string package, int version);
	vector<std::string> getChildren(std::string fileName, std::string package);
	static void identify(std::ostream& out = std::cout);

private:
	DbCore<T> dbCore;
	void performCheckOut(std::unordered_map<std::string,std::string> files);
	std::string getFileWithoutVersion(std::string fileName);
};


// ----< method to print current file path>---------------------------------
template<typename T>
void CheckOut<T>::identify(std::ostream& out)
{
	out << "\n  \"" << __FILE__ << "\"";
}

// ----< method to checkout file with given version with dependent>-----------------------------
template<typename T>
void CheckOut<T>::checkOut(std::string fileName, std::string package, int version)
{
	std::cout << "\n  Checking out version: " + std::to_string(version) + " with dependent files of : " + fileName + " and package: " + package;
	checkOut(fileName, package, version, true);
}

// ----< method to checkout file with latest version with dependent>-----------------------------
template<typename T>
void CheckOut<T>::checkOut(std::string fileName, std::string package)
{
	checkOut(fileName, package, true);
}

// ----< method to checkout file with given version without dependent>-----------------------------
template<typename T>
void CheckOut<T>::checkOut(std::string fileName, std::string package, int version, bool isCopyDependent)
{
	//Get specific version key from db of child file
	Condition cond;
	std::string key = package + "::" + fileName + "." + std::to_string(version);
	// Add parent file for checkout
	std::unordered_map<std::string, std::string> files;
	std::unordered_map<std::string, DbElement<T>>& dbStore = dbCore.dbStore();
	std::unordered_map<std::string, DbElement<T>>::iterator it = dbStore.find(key);
	if (it != dbStore.end()) {
		DbElement<T> elem = it->second;
		files[elem.payLoad().filePath()] = package;
	}
	// Copy dependent files
	if (isCopyDependent) {
		Query<T> childQuery(dbCore);
		childQuery.getChildren(key);
		for (auto key : childQuery.keys()) {
			std::unordered_map<std::string, DbElement<T>>::iterator it = dbStore.find(key);
			if (it != dbStore.end())
			{
				std::string package = key.substr(0, key.find("::"));
				DbElement<T> elem = it->second;
				files[elem.payLoad().filePath()] = package;
			}
		}
	}	
	performCheckOut(files);
}

// ----< method to checkout file with latest version without dependent>-----------------------------
template<typename T>
void CheckOut<T>::checkOut(std::string fileName, std::string package, bool isCopyDependent)
{
	//Get latest version key from db of child file
	Condition cond;
	std::string regex = package + "::" + fileName;
	std::regex keyRegex(regex);
	cond.setKeyRegex(keyRegex);
	Query<T> query(dbCore);
	query.select(cond);
	Version<T> version(query, dbCore);
	std::string key = version.getLatestVersionKey();
	std::cout << "\n  latest version key : " + key;
	// Add parent file for checkout
	std::unordered_map<std::string, std::string> files;
	std::unordered_map<std::string, DbElement<T>>& dbStore = dbCore.dbStore();
	std::unordered_map<std::string, DbElement<T>>::iterator it = dbStore.find(key);
	if (it != dbStore.end()) {
		DbElement<T> elem = it->second;
		files[elem.payLoad().filePath()] = package;
	}
	// Copy dependent files
	if (isCopyDependent) {	
		Query<T> childQuery(dbCore);
		childQuery.getChildren(key);
		if (childQuery.keys().size() > 0) {
			for (auto childKey : childQuery.keys()) {
				std::unordered_map<std::string, DbElement<T>>::iterator it = dbStore.find(childKey);
				if (it != dbStore.end())
				{
					std::cout << "\n  Checking out dependent file: "+childKey;
					std::string package = childKey.substr(0, childKey.find("::"));
					DbElement<T> elem = it->second;
					files[elem.payLoad().filePath()] = package;
				}
				else
				{
					std::cout << "\n  Child key: " + childKey + " does not exist in the database";
				}
			}
		}
		
	}
	performCheckOut(files);
}

template<typename T>
vector<string> CheckOut<T>::getChildren(std::string fileName, std::string package)
{
	Condition cond;
	std::string regex = package + "::" + fileName;
	std::regex keyRegex(regex);
	cond.setKeyRegex(keyRegex);
	Query<T> query(dbCore);
	query.select(cond);
	Version<T> version(query, dbCore);
	std::string key = version.getLatestVersionKey();
	std::cout << "\n  latest version key : " + key;
	Query<T> childQuery(dbCore);
	childQuery.getChildren(key);
	vector<std::string> childFiles;
	std::unordered_map<std::string, DbElement<T>>& dbStore = dbCore.dbStore();
	for (auto childKey : childQuery.keys())
	{
		std::unordered_map<std::string, DbElement<T>>::iterator it = dbStore.find(childKey);
		if (it != dbStore.end())
		{
			DbElement<T> elem = it->second;
			childFiles.push_back(elem.name());
		}
		else
		{
			std::cout << "\n  Child key: " + childKey + " does not exist in the database";
		}
	}
	return childFiles;
}

template<typename T>
vector<string> CheckOut<T>::getChildren(std::string fileName, std::string package,int version)
{
	Condition cond;
	std::string key = package + "::" + fileName + "." + std::to_string(version);
	std::cout << "\n  latest version key : " + key;
	Query<T> childQuery(dbCore);
	childQuery.getChildren(key);
	vector<std::string> childFiles;
	std::unordered_map<std::string, DbElement<T>>& dbStore = dbCore.dbStore();
	for (auto childKey : childQuery.keys())
	{
		std::unordered_map<std::string, DbElement<T>>::iterator it = dbStore.find(childKey);
		if (it != dbStore.end())
		{
			DbElement<T> elem = it->second;
			childFiles.push_back(elem.name());
		}
		else
		{
			std::cout << "\n  Child key: " + childKey + " does not exist in the database";
		}
	}
	return childFiles;
}


// ----< method to perform actual checkout operation>-----------------------------
template<typename T>
void CheckOut<T>::performCheckOut(std::unordered_map<std::string, std::string> files)
{
	std::unordered_map<std::string,std::string>::iterator it;
	for (it = files.begin(); it != files.end(); it++)
	{
		std::string destinationDir = Constants::getLocalDirPath();
		std::string originalFileName = getFileWithoutVersion(FileSystem::Path::getName(it->first));
		FileSystem::File::copy(it->first, destinationDir + "\\" + originalFileName, false);
	}
}

// ----< method to remove version entry from file name>-----------------------------
template<typename T>
std::string CheckOut<T>::getFileWithoutVersion(std::string fileName)
{
	size_t pos = fileName.find_last_of(".");
	return fileName.substr(0, pos);
}