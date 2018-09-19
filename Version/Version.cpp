///////////////////////////////////////////////////////////////////////
// Version.cpp - Provides test stub for Versioning                   //
// ver 1.0                                                           //
// Language:    Visual C++ 11                                        //
// Application: CSE687 - Object Oriented Design                      //
// Author:      Anuja Mahajan, Syracuse University(MSCS-Spring 2018) //
//				apmahaja@syr.edu									 //
// Source:      Jim Fawcett, CST 4-187, Syracuse University          //
//              (315) 443-3948, jfawcett@twcny.rr.com                //
///////////////////////////////////////////////////////////////////////

#include "Version.h"

#ifdef TEST_VERSION

#include "../Utilities/TestUtilities/TestUtilities.h"
#include "../RepositoryCore/Constants.h"
using namespace NoSqlDb;
///////////////////////////////////////////////////////////////////////
// DbProvider class
// - provides mechanism to share a test database between test functions
//   without explicitly passing as a function argument.
class DbProvider
{
public:
	DbCore<Payload>& db() { return db_; }
private:
	static DbCore<Payload> db_;
};

DbCore<Payload> DbProvider::db_;

using namespace Utilities;
//----< reduce the number of characters to type >----------------------
auto putLine = [](size_t n = 1, std::ostream& out = std::cout)
{
	Utilities::putline(n, out);
};


//----< Method to create database>--------------
void createDB()
{
	DbProvider dbp;
	DbCore<Payload> db = dbp.db();
	cout << "\n  Creating Database";
	DbElement<Payload> dbElem;
	std::string path = Constants::repositoryPath + "Version/Version.h";
	vector<std::string> categories = {"Version"};
	Payload p(path, categories);

	dbElem.name("Version.cpp");
	dbElem.descrip("File name is Version.cpp");
	dbElem.dateTime(DateTime().now());
	dbElem.payLoad(p);
	db["Version::Version.cpp.1"] = dbElem;

	dbElem.name("Version.h");
	dbElem.descrip("File name is Version.h");
	dbElem.dateTime(DateTime().now());
	dbElem.payLoad(p);
	p.status("Close");
	db["Version::Version.h.1"] = dbElem;

	dbElem.name("Version.h");
	dbElem.descrip("File name is Version.h");
	dbElem.dateTime(DateTime().now());
	dbElem.payLoad(p);
	p.status("Close");
	db["Version::Version.h.2"] = dbElem;
	dbp.db() = db;
}


int main()
{
	Utilities::title("Testing version Class");
	DbProvider dbp;
	createDB();
	DbCore<Payload> db = dbp.db();
	Condition cond;
	std::string regex = "Version::Version.h";
	std::regex keyRegex(regex);
	cond.setKeyRegex(keyRegex);
	Query<Payload> query(db);
	query.select(cond);
	Version<Payload> version(query, db);
	std::string latestKey = version.getLatestVersionKey();
	std::cout << "\n  Latest version key of: " +regex + " is: "+latestKey;
	std::cout << "\n  Checkin status: " + db[latestKey].payLoad().status();
	std::cout << "\n  New file name of next version: " +version.generateFileName("Version.h");

	Condition cond1;
	std::string regex1 = "Version::Version.cpp";
	std::regex keyRegex1(regex1);
	cond1.setKeyRegex(keyRegex1);
	Query<Payload> query1(db);
	query1.select(cond1);
	Version<Payload> version1(query1, db);
	std::string latestKey1 = version1.getLatestVersionKey();
	std::cout << "\n  Latest version key of: " + regex1 + " is: " + latestKey1;
	std::cout << "\n  Checkin status: " + db[latestKey1].payLoad().status();
	std::cout << "\n  New file name of next version: " + version1.generateFileName("Version.cpp");
	std::cin.get();
}
#endif