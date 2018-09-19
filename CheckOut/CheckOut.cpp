///////////////////////////////////////////////////////////////////////
// CheckOut.cpp - Provides test stub for CheckIn                       //
// ver 1.0                                                           //
// Language:    Visual C++ 11                                        //
// Application: CSE687 - Object Oriented Design                      //
// Author:      Anuja Mahajan, Syracuse University(MSCS-Spring 2018) //
//				apmahaja@syr.edu									 //
///////////////////////////////////////////////////////////////////////
#include "CheckOut.h"

//----< test stub >----------------------------------------------------
#ifdef TEST_CHECKOUT
#include "../PayLoad/PayLoad.h"
#include "../Utilities/TestUtilities/TestUtilities.h"
#include "../Query/Query.h"
#include "../Version/Version.h"
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

DbCore<PayLoad::Payload> DbProvider::db_;

using namespace Utilities;
//----< reduce the number of characters to type >----------------------
auto putLine = [](size_t n = 1, std::ostream& out = std::cout)
{
	Utilities::putline(n, out);
};

void checkout()
{
	std::cout << "\n  Checking out file (default) latest version with dependent";
	std::cout << "\n  Checking out file 'Version.cpp' from package 'Version'";
	std::string fileName = "Version.cpp";
	std::string package = "Version";
	DbProvider dbp;
	DbCore<Payload> db = dbp.db();
	if (File::exists("../database.xml"))
		createDBFromXML("../database.xml", db);
	CheckOut<Payload> co(db);
	co.checkOut(fileName, package);
}

void checkoutWithoutDependent()
{
	std::cout << "\n Checking out file without dependent";
	std::cout << "\n  Checking out file 'Version.cpp' from package 'Version'";
	std::string fileName = "Version.cpp";
	std::string package = "Version";
	DbProvider dbp;
	DbCore<Payload> db = dbp.db();
	if (File::exists("../database.xml"))
		createDBFromXML("../database.xml", db);
	CheckOut<Payload> co(db);
	co.checkOut(fileName, package, false);
}

void checkOutVersion()
{
	std::cout << "\n Checking out file (default)";
	std::cout << "\n  Checking out file 'Version.cpp' from package 'Version'";
	std::string fileName = "RepositoryCore.cpp";
	std::string package = "RepositoryCore";
	int version = 1;
	DbProvider dbp;
	DbCore<Payload> db = dbp.db();
	if (File::exists("../database.xml"))
		createDBFromXML("../database.xml", db);
	CheckOut<Payload> co(db);
	co.checkOut(fileName, package, version);
}

int main()
{
	Utilities::Title("Testing CheckIn - demonstartes basic checkin operations");
	checkoutWithoutDependent();
	checkout();
	checkOutVersion();
	
	std::cin.get();
}
#endif