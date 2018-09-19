#pragma once
///////////////////////////////////////////////////////////////////////
// CheckIn.h - Implements Checkin functionlity for repository        //
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
* - CheckIn class which provides functionality of checkin files and modify checkin in repository
* - It uses Version class to generate versions of the given file depending upon its status.
* - If status is open, file is overwritten.
* - If status is close, new version of that file is created
* - It also checks dependent files's status before closing checkin

* Required Files:
* ---------------
* Version.h
*
* Maintenance History:
* --------------------
* ver 1.0 : 12 Mar 2018
* - first release
* ver 1.1 : 30 Apr 2018
* - small modifications related to repository path, added description and category while checking in
*/

#include "../FileSystem/FileSystem.h"
#include "../Version/Version.h"
#include "../DbCore/DbCore.h"
#include "../RepositoryCore/Constants.h"

using namespace NoSqlDb;
/////////////////////////////////////////////////////////////////////
// CheckIn class
// - CheckIn class provides methods to perform checkin file in repository.

template<typename T>
class CheckIn
{
	public:	
	CheckIn<T>(DbCore<T>& db) : dbCore(db){};
	void checkIn(std::string filePath,std::string package,std::string description, vector<std::string> categories);
	void checkIn(std::string filePath, std::string package, unordered_map<std::string, std::string> children, std::string description, vector<std::string> categories);
	void closeCheckIn(std::string filePath);
	static void identify(std::ostream& out = std::cout);

	private:
	DbCore<T>& dbCore;
	bool checkIfFileExists(std::string key);
	bool isClosable(vector<std::string> childKeys);
	std::string findChildKey(std::string regex);
};

// ----< method to print current file path>--------------------------------------
template<typename T>
void CheckIn<T>::identify(std::ostream& out)
{
	out << "\n  \"" << __FILE__ << "\"";
}

// ----< method to checkin file with given name and package>--------------------------------------
template<typename T>
void CheckIn<T>::checkIn(std::string filePath, std::string package, std::string description, vector<std::string> categories)
{
	std::string fileName = FileSystem::Path::getName(filePath);
	std::cout << "\n  File name is: " + fileName;
	Condition cond;
	std::string regex = package + "::" + fileName;
	std::regex keyRegex(regex);
	cond.setKeyRegex(keyRegex);
	Query<T> query(dbCore);
	query.select(cond);
	Version<T> version(query, dbCore);
	std::string prevKey = version.getLatestVersionKey();
	std::string destFileName = version.generateFileName(fileName);
	std::string key = package + "::" + destFileName;
	std::string repositorypath = FileSystem::Path::getFullFileSpec(Constants::getRepositoryPath());
	std::string destinationDir = repositorypath + package;
	if(!FileSystem::Directory::exists(destinationDir))
		FileSystem::Directory::create(destinationDir);
	std::string srcPath = FileSystem::Path::getFullFileSpec(filePath);
	std::string destPath = FileSystem::Path::getFullFileSpec(destinationDir + "\\" + destFileName);
	std::string destinationPath = Constants::getRepositoryPath() + package + "\\" + destFileName;
	FileSystem::File::copy(srcPath, destPath, false);
	PayLoad::Payload p(destinationPath, categories);
	if (!checkIfFileExists(key)) {		
		DbElement<PayLoad::Payload> dbElem;
		dbElem.name(fileName);
		Query<T> childQ(dbCore);
		if (prevKey != "")	{
			childQ.getChildren(prevKey);
			if (childQ.keys().size() > 0)
				dbElem.children(childQ.keys());	}		
		dbElem.dateTime(DateTime().now());
		dbElem.payLoad(p);
		dbElem.descrip(description);
		dbCore[key] = dbElem;	}
	else  {
		DbElement<Payload>& dbElem = dbCore[key];
		dbElem.payLoad(p);
		dbElem.descrip(description);
		dbElem.dateTime(DateTime().now());	}
	std::cout << "\n  Creating XML from database";
	std::string dbXML = dbCore.saveDB("../database.xml");
}

// ----< method to checkin file with given name and package with dependents>--------------------------------------
template<typename T>
void CheckIn<T>::checkIn(std::string filePath, std::string package, unordered_map<std::string, std::string> children, std::string description, vector<std::string> categories)
{
	std::string fileName = FileSystem::Path::getName(filePath);
	Condition cond;
	std::string regex = package + "::" + fileName;
	std::regex keyRegex(regex);
	cond.setKeyRegex(keyRegex);
	Query<T> query(dbCore);
	query.select(cond);
	Version<T> version1(query, dbCore);
	std::string destFileName = version1.generateFileName(fileName);
	std::string key = package + "::" + destFileName;
	std::string repositorypath = FileSystem::Path::getFullFileSpec(Constants::getRepositoryPath());
	std::string destinationDir = repositorypath + package;
	if (!FileSystem::Directory::exists(destinationDir))
		FileSystem::Directory::create(destinationDir);
	std::string srcPath = FileSystem::Path::getFullFileSpec(filePath);
	std::string destPath = FileSystem::Path::getFullFileSpec(destinationDir + "\\" + destFileName);
	std::string destinationPath = Constants::getRepositoryPath() + package + "\\" + destFileName;
	FileSystem::File::copy(srcPath, destPath, false);
	Payload p(destinationPath, categories);
	unordered_map<std::string, std::string>::iterator it;
	vector<std::string> childKeys;
	for (it = children.begin(); it != children.end(); it++) {
		std::string childFileName = it->first;
		std::cout << "\n  Child File name is: " + childFileName;
		std::string regex1 = it->second + "::" + childFileName;
		std::string childKey = findChildKey(regex1);
		childKeys.push_back(childKey);
		p.addCategories(it->second);	}	
	if (!checkIfFileExists(key)) {	
		DbElement<Payload> dbElem;
		dbElem.name(fileName);
		dbElem.descrip(description);
		dbElem.dateTime(DateTime().now());
		dbElem.payLoad(p);
		dbElem.children(childKeys);
		dbCore[key] = dbElem;	}
	else {
		DbElement<Payload>& dbElem = dbCore[key];
		dbElem.payLoad(p);
		dbElem.children(childKeys);
		dbElem.descrip(description);
		dbElem.dateTime(DateTime().now());	}
	std::cout << "\n  Creating XML from database";
	std::string dbXML = dbCore.saveDB("../database.xml");
}

// ----< method to close checkin of given file path>--------------------------------------
template<typename T>
void CheckIn<T>::closeCheckIn(std::string filePath)
{
	std::cout << "\n  closing checkin of file: "+filePath;
	Condition cond;
	cond.setFilePathRegExp(filePath);
	Query<T> query(dbCore);
	query.select(cond);
	DbElement<T> elem;
	DbElement<T> parentElem;
	std::string resultKey;
	for (auto key : query.keys()) {
		std::unordered_map<std::string, DbElement<T>>& dbStore = dbCore.dbStore();
		std::unordered_map<std::string, DbElement<T>>::iterator it = dbStore.find(key);
		if (it == dbStore.end()) {
			std::cout << "\n  Key " << key << " does not exist in database";
		}
		else {
			elem = it->second;
			resultKey = key;
			break;
		}
	}
	//Check checkin status of dependent files
	if (isClosable(elem.children()))
		elem.payLoad().status("Close");
	else
		elem.payLoad().status("Close Pending");

	dbCore[resultKey] = elem;
	//Check for parent dependencies
	Condition cond1;
	std::regex regEx("Close Pending");
	cond1.setStatusRegExp(regEx);
	Query<T> query1(dbCore);
	query1.getParents(resultKey);
	if (query1.keys().size() > 0)
			query1.select(cond1);
	for (auto key : query1.keys()) {
		std::unordered_map<std::string, DbElement<T>>& dbStore = dbCore.dbStore();
		std::unordered_map<std::string, DbElement<T>>::iterator it = dbStore.find(key);
		 parentElem = it->second;
		//Check for each parent
		 if (isClosable(parentElem.children())) {
			 parentElem.payLoad().status("Close");
			 dbCore[key] = parentElem;
		}				
	}
	std::cout << "\n  Creating XML from database";
	std::string dbXML = dbCore.saveDB("../database.xml");
}

// ----< method to check if file eixsts in repository>-----------------------------
template<typename T>
bool CheckIn<T>::checkIfFileExists(std::string key)
{
	return dbCore.contains(key);
}

// ----< method to check if all dependent files are closed or not>-----------------------------
template<typename T>
bool CheckIn<T>::isClosable(vector<std::string> keys)
{
	std::unordered_map<std::string, DbElement<T>>& dbStore = dbCore.dbStore();
	for (auto key : keys) {	
		std::unordered_map<std::string, DbElement<T>>::iterator it = dbStore.find(key);
		DbElement<T>& dbElem = it->second;
		if (dbElem.payLoad().status() == "Open")
			return false;
	}
	return true;
}


template<typename T>
std::string CheckIn<T>::findChildKey(std::string regex)
{
	Condition cond1;	
	std::regex keyRegex1(regex);
	cond1.setKeyRegex(keyRegex1);
	Query<T> childQuery(dbCore);
	childQuery.select(cond1);
	Version<T> version(childQuery, dbCore);
	std::string childKey = version.getLatestVersionKey();
	return childKey;
}

