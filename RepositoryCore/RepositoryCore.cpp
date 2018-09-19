///////////////////////////////////////////////////////////////////////
// RepositoryCore.cpp - Provides test stub for RepositoryCore        //
// ver 1.0                                                           //
// Language:    Visual C++ 11                                        //
// Application: CSE687 - Object Oriented Design                      //
// Author:      Anuja Mahajan, Syracuse University(MSCS-Spring 2018) //
//				apmahaja@syr.edu									 //
///////////////////////////////////////////////////////////////////////
#include "RepositoryCore.h"

#ifdef TEST_REPCORE
#include "../Utilities/TestUtilities/TestUtilities.h"

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
	std::string path = Constants::repositoryPath + "Version/Version.h.1";
	vector<std::string> categories = { "Version" };
	Payload p(path, categories);

	dbElem.name("Version.cpp");
	dbElem.descrip("File name is Version.cpp");
	dbElem.dateTime(DateTime().now());
	p.status("Close");
	p.filePath(Constants::repositoryPath + "Version/Version.cpp.1");
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
	p.status("Close");
	p.filePath(Constants::repositoryPath + "Version/Version.h.2");
	dbElem.payLoad(p);
	db["Version::Version.h.2"] = dbElem;
	dbp.db() = db;
}

//----< Method to perform checkin operation>--------------
void checkInRep()
{
	std::cout << "\n  Testing checkin operation";
	DbProvider dbp;
	DbCore<Payload> dbCore = dbp.db();
	RepositoryCore<Payload> repository(dbCore);
	std::string filePath = "../Version/Version.cpp";
	std::string package = "Version";
	repository.checkInRep(filePath, package);
	std::cout << "\n  ----------------------------------------------";
}

//----< Method to perform checkout operation>--------------
void checkout()
{
	std::cout << "\n  Testing checkout operation";
	std::string fileName = "Version.cpp";
	std::string package = "Version";
	DbProvider dbp;
	DbCore<Payload> dbCore = dbp.db();
	if (File::exists("../database.xml"))
		createDBFromXML("../database.xml", dbCore);
	RepositoryCore<Payload> repository(dbCore);
	repository.checkOut(fileName, package);
	std::cout << "\n  ----------------------------------------------";
}

//----< Method to perform browse operation>--------------
void browse()
{
	std::cout << "\n  Testing browse operation";
	DbProvider dbp;
	DbCore<Payload> db = dbp.db();
	if (File::exists("../database.xml"))
		createDBFromXML("../database.xml", db);
	std::string fileName = "Version.cpp";
	std::string package = "Version";
	std::cout << "\n  Browse file:  " + fileName + " with package: " + package;
	
	RepositoryCore<Payload> repository(db);
	repository.browse(fileName, package);
	std::cout << "\n  ----------------------------------------------";
}


int main()
{
	Utilities::title("Testing repository class");
	createDB();
	checkInRep();
	checkout();
	browse();
	std::cin.get();
}

#endif
