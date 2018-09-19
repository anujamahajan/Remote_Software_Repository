#pragma once
///////////////////////////////////////////////////////////////////////
// Constants.h - Provides constants for Repository                   //
// ver 1.0                                                           //
// Language:    Visual C++ 11                                        //
// Application: CSE687 - Object Oriented Design                      //
// Author:      Anuja Mahajan, Syracuse University(MSCS-Spring 2018) //
//				apmahaja@syr.edu									 //
///////////////////////////////////////////////////////////////////////
/*
* Maintenance History :
*--------------------
* ver 1.0 : 12 Mar 2018
* -first release
* ver 1.1 : 30 Apr 2018
* -made constants private and provided getters 
*/

#include <string>

class Constants
{
public:
	static std::string getRepositoryPath() { return repositoryPath; }
	static std::string getLocalDirPath() { return localDirPath; }
	static std::string getAppPath() { return appPath; }

private:
	static const std::string repositoryPath;
	static const std::string localDirPath;
	static const std::string appPath;
};