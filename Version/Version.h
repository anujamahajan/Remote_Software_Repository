#pragma once
///////////////////////////////////////////////////////////////////////////////
// Version.h - Implements Versioning functionlity for file in repository     //
// ver 1.0                                                                   //
// Language:    Visual C++ 11                                                //
// Application: CSE687 - Object Oriented Design                              //
// Author:      Anuja Mahajan, Syracuse University(MSCS-Spring 2018)         //
//				apmahaja@syr.edu									         //
///////////////////////////////////////////////////////////////////////////////

/*
* Package Operations:
* -------------------
* This package provides:
* - Version class which provides functionality of versioning files from repository
* - Version will be incremented only if file is in close or close pending status
* - This file also provides functions to get latest version of the file

* Required Files:
* ---------------
* Query.h
*
* Maintenance History:
* --------------------
* ver 1.0 : 12 Mar 2018
* - first release
*/
#include <iostream>
#include <string>
#include "../Query/Query.h"

/////////////////////////////////////////////////////////////////////
// Version class
// - Version class provides methods to perform versioning file in repository.
// - It also provides method to find latest version of the file

template<typename T>
class Version
{
public:
	Version<T>(Query<T> query, DbCore<T> dbCore): query_(query),dbCore_(dbCore){}
	std::string generateFileName(std::string fileName);
	std::string getLatestVersionKey();
	std::string toString() { return std::to_string(version_); } // Convert version to string
	int version() { return version_; }
	static void identify(std::ostream& out = std::cout);

private:
	int version_=0;
	DbCore<T> dbCore_;
	Query<T> query_;
	Version<T>& getVersion();
};


// ----< method to find version of the file while checkin>--------------------------------------
template<typename T>
Version<T>& Version<T>::getVersion()
{
	std::vector<std::string> resultKeys = query_.keys();
	if (resultKeys.size() == 0)
		version_ = 1;
	else {
		int version = 0;
		int count = 0;
		std::string lastKey;
		for (auto key : resultKeys) {
			if (key != "") {
				int currentVersion = key.at(key.length() - 1) - '0';
				if (version < currentVersion )	{
					version = currentVersion;
					lastKey = key;
				}	
			}
		}
		//If status is not open then only increment version no.
		std::unordered_map<std::string, DbElement<T>>& dbStore = dbCore_.dbStore();
		std::unordered_map<std::string, DbElement<T>>::iterator it = dbStore.find(lastKey);
		if (it == dbStore.end()) {
			std::cout << "\n  Key " << lastKey << " does not exist in database";
		}
		else {
			DbElement<T>& dbElement=  it->second;
			std::string status = dbElement.payLoad().status();
			// Increment version if status is not Open
			if (status.compare("Open") != 0) {	
					version_ = ++version;
			} else
				version_ = version;
		}		
	}
	return *this;
}


// ----< method to find latest version of the file and return key of that file>-----------------------------------
template<typename T>
std::string Version<T>::getLatestVersionKey()
{
	std::vector<std::string> resultKeys = query_.keys();
	std::string lastKey;
	if(resultKeys.size() > 0)
	{
		int version = 0;	
		for (auto key : resultKeys) {
			if (key != "") {
				int currentVersion = key.at(key.length() - 1) - '0';
				if (version < currentVersion) {
					version = currentVersion;
					lastKey = key;
				}
			}
		}
	}
	return lastKey;
}

// ----< method to generate file name of new version while checkin>--------------------------------------
template<typename T>
std::string Version<T>::generateFileName(std::string fileName)
{
	std::string key = fileName + "."+ getVersion().toString();
	return key;
}

// ----< method to print current file path>--------------------------------------
template<typename T>
void Version<T>::identify(std::ostream& out)
{
	out << "\n  \"" << __FILE__ << "\"";
}

