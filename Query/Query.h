#pragma once
///////////////////////////////////////////////////////////////////////
// Query.h Implements NoSQL database prototype                    //
// ver 1.0                                                           //
// Language:    Visual C++ 11                                        //
// Application: CSE687 - Object Oriented Design                      //
// Author:      Anuja Mahajan, Syracuse University(MSCS-Spring 2018) //
//				apmahaja@syr.edu									 //
// Source:      Jim Fawcett, CST 4-187, Syracuse University          //
//              (315) 443-3948, jfawcett@twcny.rr.com                //
///////////////////////////////////////////////////////////////////////
#include "../DbCore/DbCore.h"
#include <regex>
#include "../DateTime/DateTime.h"
#include "../PayLoad/PayLoad.h"

/*
* Package Operations:
* -------------------
* This package provides three classes:
* - Query class provides queries to perform database operations like fetching specific data.
* - It provides functions to perform AND/OR operations on query result and display them
* - Condition class provides fields for setting regular expression for each DbElement field

* Required Files:
* ---------------
* DateTime.h, DateTime.cpp
*
* Maintenance History:
* --------------------
* ver 1.0 : 10 Jan 2018
* - first release
* ver 1.1 : 19 Jan 2018
* - added code to throw exception in index operators if input key is not in database
* ver 1.2 : 04 Feb 2018
* - added condition class, AND and or queries
* ver 1.3 : 30 Apr 2018
* - added payload conditions
*/
using namespace NoSqlDb;
using namespace std;

/////////////////////////////////////////////////////////////////////
// Condition class
// - Condition class stores regular expressions for database elements	
// - It also provides methods to compare dates
class Condition {
public:
	Condition() {
		fromDate = DateTime("Sun Jan 01 00:00:00 2000");
		toDate = DateTime("Sun Jan 01 00:00:00 2000");
	}
	//Getters and setters for regular expressions for database elements
	regex & getKeyRegExp() { return keyRegExp; }
	regex& getNameRegExp() { return nameRegExp; }
	regex& getDescripRegExp() { return descripRegExp; }
	DateTime& getFromDate() { return fromDate; }
	DateTime& getToDate() { return toDate; }
	regex& getCategoryRegExp() { return categoryRegExp; }
	Condition& setKeyRegex(const regex& regex);
	Condition& setNameRegex(const regex& regex);
	Condition& setDescriptionRegex(const regex& regex);
	Condition& setDateTime(DateTime& fromDate, DateTime& toDate);
	Condition& setFromOrToDate(DateTime& date);
	Condition& setCategoryRegExp(const regex& regex);
	Condition& setChildRegExp(const regex& regex);
	regex& getChildRegExp() { return childRegExp; }
	DateTime getDateDefaultValue() { return DateTime("Sun Jan 01 00:00:00 2000"); };
	bool compareDates(DateTime date1, DateTime date2);
	bool matchCategory(DbElement<PayLoad::Payload> element, const regex regEx);
	bool matchChild(DbElement<PayLoad::Payload> element, const regex regEx);
	Condition& setFilePathRegExp(const std::string& regex);
	std::string getFilePathRegExp() { return filePathRegExp; }
	regex& getStatusRegExp() { return statusRegExp; }
	Condition& setStatusRegExp(const regex& regex);
	Condition& setVersionRegExp(const std::string& regex);
	std::string& getVersionRegExp() { return versionRegExp; }
	bool matchVersion(std::string key, const std::string regEx);

	bool compare(std::string dbString, std::string condition);

private:
	regex keyRegExp;
	regex nameRegExp;
	regex descripRegExp;
	DateTime fromDate;
	DateTime toDate;
	regex categoryRegExp;
	std::string filePathRegExp;
	regex statusRegExp;
	std::string versionRegExp;
	std::regex childRegExp;
};

/////////////////////////////////////////////////////////////////////
// Query class
// - Query class provides methods to perform queries on database.
// - Methods demonstartes database operations like, AND, OR and getting specific data 
//   matching with regular expression, and displaying the queried result
template<typename T>
class Query {
	// methods to run query
public:
	Query<T>(const DbCore<T>& db) {
		dbcore = db;
	}

	Query<T> getValue(std::string key);
	Query<T> getChildren(string key);
	Query<T>& select(const Condition& cond);
	Query<T>& from(vector<string> keys);
	void show();
	bool isValidKey(DbElement<T> element, string key, Condition cond);
	vector<string> keys() { return resultKeys; }
	Query<T> getParents(string key);
	static void identify(std::ostream& out = std::cout);

private:
	DbCore<T> dbcore;
	vector<string> resultKeys;
};

// ----< method to print current file path>--------------------------------------
template<typename T>
void Query<T>::identify(std::ostream& out)
{
	out << "\n  \"" << __FILE__ << "\"";
}

// ----< method to get value of the given key>-------------------------
template<typename T>
Query<T> Query<T>::getValue(string key)
{
	resultKeys.push_back(key);
	return *this;
}

// ----< method to retrieve child keys of the given keys>-------------------------
template<typename T>
Query<T> Query<T>::getChildren(string key)
{
	std::unordered_map<std::string, DbElement<T>>& _dbStore = dbcore.dbStore();
	std::unordered_map<std::string, DbElement<T>>::iterator it = _dbStore.find(key);
	if (it == _dbStore.end()) {
		std::cout << "\n  Key " << key << " does not exist in database";
	}
	else {
		for (auto childKey : it->second.children()) {
			std::unordered_map<std::string, DbElement<T>>::iterator it1 = _dbStore.find(childKey);
			if (it1 == _dbStore.end()) {
				std::cout << "\n  Key " << childKey << " does not exist in database";
			}
			else
				resultKeys.push_back(childKey);
		}
	}
	return *this;
}


// ----< method to retreive keys based on given condition>-------------------------
template<typename T>
Query<T>& Query<T>::select(const Condition& cond)
{
	unordered_map<string, DbElement<T>> dbStore = dbcore.dbStore();
	if (resultKeys.empty()) {
		for (auto it = dbStore.begin(); it != dbStore.end(); ++it) {
			if (isValidKey(it->second, it->first, cond))
				resultKeys.push_back(it->first);
		}
	}
	else {
		//Querying on the key set of previous query
		for (auto key : resultKeys) {
			if (key != "") {
				unordered_map<string, DbElement<T>>::iterator keyIt = dbStore.find(key);
				vector<string>::iterator resultIt = std::find(resultKeys.begin(), resultKeys.end(), key);
				if (keyIt == dbStore.end()) {
					resultKeys.erase(resultIt);
					resultKeys.reserve(resultKeys.size());
					std::cout << "\n  Key " << key << " does not exist in database";
					continue;
				}
				else {
					if (!isValidKey(keyIt->second, key, cond)) {
						resultKeys.erase(resultIt);
						resultKeys.reserve(resultKeys.size());
					}
				}
			}
		}
	}
	return *this;
}

// ----< method to perform union of two or more queries>-------------------------
template<typename T>
Query<T>& Query<T>::from(vector<string> keys)
{
	for (auto key : keys) {
		if (find(resultKeys.begin(), resultKeys.end(), key) == resultKeys.end()) {
			resultKeys.push_back(key);
		}
	}
	return *this;
}

// ----< method to display queried results>-------------------------
template<typename T>
void Query<T>::show()
{
	showHeader();
	for (auto key : resultKeys) {
		std::unordered_map<std::string, DbElement<T>>& dbStore = dbcore.dbStore();
		std::unordered_map<std::string, DbElement<T>>::iterator it = dbStore.find(key);
		if (it == dbStore.end()) {
			std::cout << "\n  Key " << key << " does not exist in database";
		}
		else {
			showElem(it->first, it->second);
		}
	}
}

// ----<Setter methods for regex fields in Condition class>-------------------------
Condition& Condition::setKeyRegex(const regex& regex)
{
	keyRegExp = regex;
	return *this;
}

Condition& Condition::setNameRegex(const regex& regex)
{
	nameRegExp = regex;
	return *this;
}

Condition& Condition::setDescriptionRegex(const regex& regex)
{
	descripRegExp = regex;
	return *this;
}

Condition& Condition::setDateTime(DateTime& fromD, DateTime& toD)
{
	fromDate = fromD < toD ? fromD : toD;
	toDate = fromD > toD ? fromD : toD;
	return *this;
}

Condition& Condition::setCategoryRegExp(const regex& regex)
{
	categoryRegExp = regex;
	return *this;
}

Condition& Condition::setFilePathRegExp(const std::string& regex)
{
	filePathRegExp = regex;
	return *this;
}

Condition& Condition::setStatusRegExp(const regex& regex)
{
	statusRegExp = regex;
	return *this;
}

Condition& Condition::setChildRegExp(const regex& regex)
{
	childRegExp = regex;
	return *this;
}

Condition& Condition::setVersionRegExp(const std::string& regex)
{
	versionRegExp = regex;
	return *this;
}

// ----< method to set current date in case only one date is provided to condition>------------
Condition& Condition::setFromOrToDate(DateTime& date)
{
	DateTime currentDayTime;
	if (date < currentDayTime)
		setDateTime(date, currentDayTime);
	else
		setDateTime(currentDayTime, date);
	return *this;
}

// ----< method to check if the key satisfies given condition>------------
template<typename T>
bool Query<T>::isValidKey(DbElement<T> element, string key, Condition cond)
{
	if (cond.getKeyRegExp()._Empty() || regex_search(key, cond.getKeyRegExp()))
	{
		if (cond.getNameRegExp()._Empty() || regex_search(element.name(), cond.getNameRegExp()))
		{
			if (cond.getDescripRegExp()._Empty() || regex_search(element.descrip(), cond.getDescripRegExp()))
			{
				if (cond.compareDates(cond.getFromDate(), cond.getDateDefaultValue()) || cond.compareDates(cond.getToDate(), cond.getDateDefaultValue()) || (cond.getFromDate() < element.dateTime() && cond.getToDate() > element.dateTime()))
				{
					//  checking template type is payload
						if (typeid(T) == typeid(Payload)) {
							if (cond.getCategoryRegExp()._Empty() || cond.matchCategory(element, cond.getCategoryRegExp()))
							{
								if (cond.getFilePathRegExp().empty() || cond.compare(element.payLoad().filePath(), cond.getFilePathRegExp()))
								{
									if (cond.getStatusRegExp()._Empty() || regex_search(element.payLoad().status(), cond.getStatusRegExp()))
									{
										if (cond.getVersionRegExp().empty() || cond.matchVersion(key, cond.getVersionRegExp()))
										{
											if (cond.getChildRegExp()._Empty() || cond.matchChild(element, cond.getChildRegExp()))
												return true;
											return false;
										}
										return false;
									}
									return false;
								}
								return false;
							}
							return false;
						}
						return true;
				}
			}
		}
	}
	return false;
}


// ----< method to match category in payload class>------------
bool Condition::matchCategory(DbElement<PayLoad::Payload> element, const regex regEx)
{
	vector<string> categories = element.payLoad().categories();
	for (auto category : categories)
	{
		if (regex_search(category, regEx))
			return true;
	}
	return false;
}

// ----< method to match category in payload class>------------
bool Condition::matchChild(DbElement<PayLoad::Payload> element, const regex regEx)
{
	vector<string> children = element.children();
	for (auto child : children)
	{
		if (regex_search(child, regEx))
			return true;
	}
	return false;
}

// ----< method to match version in payload class>------------
bool Condition::matchVersion(std::string key, const std::string regEx)
{
	string version = key.substr(key.find_last_of('.')+1);
	return version.compare(regEx)==0;
}

// ----< method to compare dates>------------
bool Condition::compareDates(DateTime date1, DateTime date2)
{
	string time1 = date1.time();
	string time2 = date2.time();
	if (time1.compare(time2) == 0)
		return true;
	return false;
}

// ----< method to compare string>------------
bool Condition::compare(std::string dbString,std::string condition)
{
	std::transform(dbString.begin(), dbString.end(), dbString.begin(), ::tolower);
	std::transform(condition.begin(), condition.end(), condition.begin(), ::tolower);
	return dbString.compare(condition) == 0;
}

// ----< method to retrieve parent keys of the given keys>-------------------------
template<typename T>
Query<T> Query<T>::getParents(string key)
{
	std::unordered_map<std::string, DbElement<T>>& dbStore = dbcore.dbStore();
	std::unordered_map<std::string, DbElement<T>>::iterator it;

	for (it = dbStore.begin(); it != dbStore.end(); it++)
	{
		DbElement<T> elem = it->second;
		vector<std::string> children = elem.children();
		if (std::find(children.begin(), children.end(), key) != children.end())
			resultKeys.push_back(it->first);
	}
	return *this;
}