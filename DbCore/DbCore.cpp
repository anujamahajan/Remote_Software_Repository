///////////////////////////////////////////////////////////////////////
// DbCore.cpp - Provides test stub for DB prototype                  //
// ver 1.0                                                           //
// Language:    Visual C++ 11                                        //
// Application: CSE687 - Object Oriented Design                      //
// Author:      Anuja Mahajan, Syracuse University(MSCS-Spring 2018) //
//				apmahaja@syr.edu									 //
// Source:      Jim Fawcett, CST 4-187, Syracuse University          //
//              (315) 443-3948, jfawcett@twcny.rr.com                //
///////////////////////////////////////////////////////////////////////


#include "DbCore.h"

using namespace NoSqlDb;
//----< test stub >----------------------------------------------------
#ifdef TEST_DBCORE
#include "../HelperUtilities/StringUtilities/StringUtilities.h"
#include "../HelperUtilities/TestUtilities/TestUtilities.h"
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

//----< Method to demonstartes Addition in dataabase >----------------------------
void addDBElement() {
	DbProvider dbp;
	DbCore<Payload> db = dbp.db();
	// create some demo elements and insert into db
	std::string childKey = "ABC";
	DbElement<Payload> dbElement;
	std::string s = "Utility";
	std::vector<std::string> c = { "DbCore","Utilities" };
	Payload p1(s, c);
	dbElement.name("XYZ");
	dbElement.descrip("Internation student..");
	dbElement.payLoad(p1);
	dbElement.dateTime(DateTime().now());
	db[childKey] = dbElement;

	std::string filepath = "DbCore";
	std::vector<std::string> categories = { "DbCore","Query1","Utility" };
	Payload p(filepath, categories);
	DbElement<Payload> dbElem;
	dbElem.name("J.");
	dbElem.descrip("CSE687");
	dbElem.dateTime(DateTime().now());
	dbElem.payLoad(p);
	std::cout << "\n  Adding child element in dbElement";
	dbElem.children().push_back(childKey);
	db["Fawcett"] = dbElem;
	dbp.db() = db;
	//display results
	showDb(db);
	putLine();
}

//----< Method to demonstartes Deletion in dataabase >----------------------------
void deleteDBElement() {
	DbProvider dbp;
	DbCore<Payload> db = dbp.db();
	// Delete element from DB
	std::cout << "\n  Deleting element from db";
	std::string key = "ABC";
	std::unordered_map<std::string, DbElement<Payload>>& dbStore = db.dbStore();
	std::unordered_map<std::string, DbElement<Payload>>::iterator it = dbStore.find(key);
	if (it == dbStore.end()) {
		std::cout << "\n  Key " << key << "does not exist in database";
	}
	else {
		dbStore.erase(key);
		dbStore.reserve(dbStore.size());
	}
	// display the results
	dbp.db() = db;
	showDb(db);
	putLine();
}

//----< Method to demonstartes Edition in dataabase >----------------------------
void editDBElement() {
	DbProvider dbp;
	DbCore<Payload> db = dbp.db();
	std::cout << "\n  Editing element in db";
	std::string key = "Fawcett";
	std::unordered_map<std::string, DbElement<Payload>>& dbStore = db.dbStore();
	std::unordered_map<std::string, DbElement<Payload>>::iterator it = dbStore.find(key);
	if (it == dbStore.end())
		std::cout << "\n  Key " << key << " does not exist in database";
	else {
		std::vector<std::string> c = { "Utiities" };
		Payload p("xyz..", c);
		it->second.name("Jim");
		it->second.descrip("jfawcett@twcny.rr.com");
		it->second.payLoad().categories(c);
		std::cout << "\n  Element with " << key << " after editing text metadata";
		//display results
		showHeader();
		showElem(key, it->second);
	}
	dbp.db() = db;
	putLine();
}

using namespace Utilities;
using namespace NoSqlDb;

int main()
{
	Utilities::Title("Testing DbCore - demonstartes basic Database operations");
	addDBElement();
	deleteDBElement();
	editDBElement();

	std::cin.get();
	return 0;
}
#endif
