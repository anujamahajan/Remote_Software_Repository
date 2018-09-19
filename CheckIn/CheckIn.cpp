///////////////////////////////////////////////////////////////////////
// DbCore.cpp - Provides test stub for CheckIn                       //
// ver 1.0                                                           //
// Language:    Visual C++ 11                                        //
// Application: CSE687 - Object Oriented Design                      //
// Author:      Anuja Mahajan, Syracuse University(MSCS-Spring 2018) //
//				apmahaja@syr.edu									 //
///////////////////////////////////////////////////////////////////////
#include "CheckIn.h"
#include <regex>


using namespace NoSqlDb;
using namespace FileSystem;


//----< test stub >----------------------------------------------------

#ifdef TEST_CHECKIN
#include "../HelperUtilities/TestUtilities/TestUtilities.h"
///////////////////////////////////////////////////////////////////////
// DbProvider class
// - provides mechanism to share a test database between test functions
//   without explicitly passing as a function argument.
class DbProvider
{
public:
	DbCore<PayLoad::Payload>& db() { return db_; }
private:
	static DbCore<PayLoad::Payload> db_;
};

DbCore<PayLoad::Payload> DbProvider::db_;

using namespace Utilities;
//----< reduce the number of characters to type >----------------------
auto putLine = [](size_t n = 1, std::ostream& out = std::cout)
{
	Utilities::putline(n, out);
};

void checkInOpen()
{
	std::cout << "\n  Checking in file 'Constants.h' from package 'Repository'";
	std::string filePath = "../RepositoryCore/Constants.h";
	std::string package = "Constants";
	DbProvider dbp;
	DbCore<Payload>& db = dbp.db();
	if (File::exists("../database.xml"))
		createDBFromXML("../database.xml", db);
	CheckIn<Payload> checkIn(db);
	checkIn.checkIn(filePath, package);
}

void checkInClose()
{
	std::cout << "\n  Clsoing checkin of file 'Query.cpp.1' from package 'Query'";
	std::string filePath = Constants::repositoryPath + "Constants\\Constants.h.1";
	DbProvider dbp;
	DbCore<Payload> db = dbp.db();
	if (File::exists("../database.xml"))
		createDBFromXML("../database.xml", db);
	CheckIn<Payload> checkIn(db);
	checkIn.closeCheckIn(Path::getFullFileSpec(filePath));
}

int main()
{
	Utilities::Title("Testing CheckIn - demonstartes basic checkin operations");
	checkInOpen();
	checkInClose();
	std::cin.get();
}
#endif