#pragma once
///////////////////////////////////////////////////////////////////////
// Browse.h - Provides browse functionality in repository            //
// ver 1.0                                                           //
// Language:    Visual C++ 11                                        //
// Application: CSE687 - Object Oriented Design                      //
// Author:      Anuja Mahajan, Syracuse University(MSCS-Spring 2018) //
//				apmahaja@syr.edu					                 //
///////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides Browse class which implements browse functionality in repository
* It accepts filename and package name and pops up requested file in notepad
* 

* Required Files:
* ---------------
* Process.h
*
* Maintenance History:
* --------------------
* ver 1.0 : 12 Mar 2018
* - first release
* ver 1.1 : 30 Apr 2018
* - added few function to filter files based on query params
*/
#include <string>
#include "Process.h"
#include "../DbCore/DbCore.h"
#include "../RepositoryCore/Constants.h"

using namespace NoSqlDb;

/////////////////////////////////////////////////////////////////////
// Browse class
// - provides browse functionality in repository

template<typename T>
class Browse
{
public:
	Browse<T>(DbCore<T> db) : dbCore(db) {}
	//void browse(std::string fileName, std::string package);
	std::vector<std::string> browse(std::string condition, std::string value);
	void browse(std::string fileName, std::string package, int version);
	std::string getFilePath(std::string key);
	std::vector<std::string> browse(std::unordered_map<std::string, std::string> filterCondition);
	DbElement<T> getElement(std::string fileName);
	std::vector<std::string> getFilesWithNoParents();
	static void identify(std::ostream& out = std::cout);

private:
	DbCore<T> dbCore;
	void browse(std::string filePath);
};

// ----< method to print current file path>--------------------------------------
template<typename T>
void Browse<T>::identify(std::ostream& out)
{
	out << "\n  \"" << __FILE__ << "\"";
}

// ----< method to browse file of given path>----------------------------
template<typename T>
void Browse<T>::browse(std::string filePath)
{
	Process p;
	p.title("Browsing the file specified by user (default- latest)");
	p.application(Constants::getAppPath());
	std::string cmdLine = "/A "+filePath;
	p.commandLine(cmdLine);
	p.create();
	CBP callback = []() { std::cout << "\n  --- child process exited with this message ---"; };
	p.setCallBackProcessing(callback);
	p.registerCallback();	
}

// ----< method to browse file with specified file name and package with latest version>----------------------------
/*template<typename T>
void Browse<T>::browse(std::string fileName, std::string package)
{
	//Find latest version of file with given filename and package
	Condition cond;
	std::string regex = package + "::" + fileName;
	std::regex keyRegex(regex);
	cond.setKeyRegex(keyRegex);
	Query<T> query(dbCore);
	query.select(cond);
	Version<T> version(query, dbCore);
	std::string key = version.getLatestVersionKey();
	DbElement<T> elem;
	std::unordered_map<std::string, DbElement<T>>& dbStore = dbCore.dbStore();
	std::unordered_map<std::string, DbElement<T>>::iterator it = dbStore.find(key);
	if (it == dbStore.end()) {
		std::cout << "\n  Key " << key << " does not exist in database";
	}
	else {
		elem = it->second;
		//Pop up file of latest version
		browse(elem.payLoad().filePath());
	}	
}*/


// ----< method to browse file with specified file name and package, version>----------------------------
template<typename T>
void Browse<T>::browse(std::string fileName, std::string package,int version)
{
	//Find key of file with given filename, version and package
	std::string key = package + "::" + fileName + "." +std::to_string(version);
	DbElement<T> elem;
	std::unordered_map<std::string, DbElement<T>>& dbStore = dbCore.dbStore();
	std::unordered_map<std::string, DbElement<T>>::iterator it = dbStore.find(key);
	if (it == dbStore.end()) {
		std::cout << "\n  Key " << key << " does not exist in database";
	}
	else {
		elem = it->second;
		//Pop up file of given version
		browse(elem.payLoad().filePath());
	}
}

// ----< method to browse file with specified file name and package with latest version>----------------------------
template<typename T>
std::vector<std::string> Browse<T>::browse(std::string condition, std::string value)
{
	//Find latest version of file with given filename and package
	Condition cond;
	if (condition._Equal("File Name"))
	{
		std::regex regex(value);
		cond.setNameRegex(regex);
	}
	else if (condition._Equal("Description"))
	{
		std::regex regex(value);
		cond.setDescriptionRegex(regex);
	}
	else if (condition._Equal("Category"))
	{
		std::regex regex(value);
		cond.setCategoryRegExp(regex);
	}
	else if (condition._Equal("Version"))
	{
		cond.setVersionRegExp(value);
	}
	else if (condition._Equal("Status"))
	{
		std::regex regex(value);
		cond.setStatusRegExp(regex);
	}
	else if (condition._Equal("No Parents"))
	{
		std::regex regex(value);
		cond.setStatusRegExp(regex);
	}
	else if (condition._Equal("Dependency"))
	{
		std::regex regex(value);
		cond.setChildRegExp(regex);
	}

	Query<T> query(dbCore);
	query.select(cond);
	std::unordered_map<std::string, DbElement<T>>& dbStore = dbCore.dbStore();
	std::vector<std::string> files;
	for (string key : query.keys())
	{		
		std::size_t found = key.find_last_of(":");
		files.push_back(key.substr(found +1));
	}
	query.show();
	return files;
}

// ----< method to browse file with specified file name and package with latest version>----------------------------
template<typename T>
std::string Browse<T>::getFilePath(std::string key)
{
	//Find latest version of file with given filename and package
	Condition cond;
	std::regex regEx(key);
	cond.setKeyRegex(regEx);
	
	Query<T> query(dbCore);
	query.select(cond);
	std::string resultKey;
	std::string filePath;
	if(query.keys().size() > 0)
		resultKey = query.keys().at(0);
	std::unordered_map<std::string, DbElement<T>>& dbStore = dbCore.dbStore();
	std::unordered_map<std::string, DbElement<T>>::iterator it = dbStore.find(resultKey);
	if (it == dbStore.end()) {
		std::cout << "\n  Key " << key << " does not exist in database";
	}
	else {
		DbElement<T> elem = it->second;
		filePath = elem.payLoad().filePath();
	}
	query.show();
	return filePath;
}

template<typename T>
std::vector<std::string> Browse<T>::browse(std::unordered_map<std::string, std::string> filterCondition)
{
	//Find latest version of file with given filename and package
	Condition cond;
	unordered_map<std::string, std::string>::iterator it;
	for (it = filterCondition.begin(); it != filterCondition.end(); it++)
	{
		std::string condition = it->first;
		std::string value = it->second;
		if (condition._Equal("File Name"))	{
			std::regex regex(value);
			cond.setNameRegex(regex);
		}
		else if (condition._Equal("Description"))  {
			std::regex regex(value);
			cond.setDescriptionRegex(regex);
		}
		else if (condition._Equal("Category"))  {
			std::regex regex(value);
			cond.setCategoryRegExp(regex);
		}
		else if (condition._Equal("Version"))
		{
			cond.setVersionRegExp(value);
		}
		else if (condition._Equal("Status"))  {
			std::regex regex(value);
			cond.setStatusRegExp(regex);
		}
		else if (condition._Equal("Dependency"))  {
			std::regex regex(value);
			cond.setChildRegExp(regex);
		}
	}	
	Query<T> query(dbCore);
	query.select(cond);
	std::vector<std::string> files;
	for (string key : query.keys()) {
		std::size_t found = key.find_last_of(":");
		files.push_back(key.substr(found + 1));
	}
	query.show();
	return files;
}

template<typename T>
DbElement<T> Browse<T>::getElement(std::string fileName)
{
	Condition cond;
	std::regex regEx(fileName);
	cond.setKeyRegex(regEx);
	Query<T> query(dbCore);
	query.select(cond);
	std::string resultKey;
	DbElement<T> elem;
	if (query.keys().size() > 0)
		resultKey = query.keys().at(0);
	std::unordered_map<std::string, DbElement<T>>& dbStore = dbCore.dbStore();
	std::unordered_map<std::string, DbElement<T>>::iterator it = dbStore.find(resultKey);
	if (it == dbStore.end()) {
		std::cout << "\n  Key " << resultKey << " does not exist in database";
	}
	else {
		elem = it->second;
	}
	query.show();
	return elem;
}

template<typename T>
std::vector<std::string> Browse<T>::getFilesWithNoParents()
{
	std::cout << "\n  Displaying files whch has no parents in repository";
	std::unordered_map<std::string, DbElement<T>>& dbStore = dbCore.dbStore();
	std::unordered_map<std::string, DbElement<T>>::iterator it;
	std::unordered_map<std::string, DbElement<T>>::iterator pit;
	std::vector<std::string> files;
	showHeader();
	for (string parentKey : dbCore.keys())
	{
		pit = dbStore.find(parentKey);
		DbElement<T> parentElem = pit->second;
		for (it = dbStore.begin(); it != dbStore.end(); it++)
		{
			std::string childKey = it->first;
			if (parentKey != childKey)
			{
				DbElement<T> elem = it->second;
				vector<std::string> children = elem.children();
				if (std::find(children.begin(), children.end(), parentKey) != children.end()) {
					break;
				}
				else {
					if (std::next(it) == dbStore.end())
					{
						std::string resultFile = parentKey.substr(parentKey.find_last_of(':') + 1);
						files.push_back(resultFile);
						showElem(parentKey, parentElem);
					}
					else
						continue;
				}
			}	
			else
			{
				if (std::next(it) == dbStore.end())
				{
					std::string resultFile = parentKey.substr(parentKey.find_last_of(':') + 1);
					files.push_back(resultFile);
					showElem(parentKey, parentElem);
				}
			}
		}	
	}

	return files;
}