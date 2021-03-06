///////////////////////////////////////////////////////////////////////
// Query.cpp - Provides test stub for DB prototype                  //
// ver 1.0                                                           //
// Language:    Visual C++ 11                                        //
// Application: CSE687 - Object Oriented Design                      //
// Author:      Anuja Mahajan, Syracuse University(MSCS-Spring 2018) //
//				apmahaja@syr.edu									 //
// Source:      Jim Fawcett, CST 4-187, Syracuse University          //
//              (315) 443-3948, jfawcett@twcny.rr.com                //
///////////////////////////////////////////////////////////////////////

#include <iostream>
#include "Query.h"



#ifdef TEST_QUERY
#include "../HelperUtilities/StringUtilities/StringUtilities.h"
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
	string s = "Utility";
	vector<string> c = { "DbCore","Utilities" };
	Payload p(s, c);
	dbElem.name("Jim");
	dbElem.descrip("CSE687");
	dbElem.dateTime(DateTime().now());
	dbElem.payLoad(p);
	db["Fawcett"] = dbElem;

	dbElem.name("Ammar");
	dbElem.descrip("TA for CSE687");
	dbElem.dateTime(DateTime().now());
	db["Salman"] = dbElem;

	dbElem.name("Jianan");
	dbElem.descrip("TA for CSE687");
	vector<string> categories = { "DbCore","Query","Test" };
	Payload p1("Query.cpp", categories);
	dbElem.payLoad(p1);
	dbElem.dateTime(DateTime("Mon Jan 16 12:12:12 2018"));
	db["Sun"] = dbElem;

	dbElem.descrip("Graduating");
	p.filePath("Test.cpp");
	dbElem.payLoad(p);
	dbElem.name("Nikhil");
	dbElem.dateTime(DateTime("Tue Jan 21 12:00:00 2018"));
	db["Prashar"] = dbElem;
	dbp.db() = db;
}

//----< Method to add children in database>--------------
void createChildren()
{
	DbProvider dbp;
	DbCore<Payload> db = dbp.db();
	db["Fawcett"].children().push_back("Salman");
	db["Fawcett"].children().push_back("Sun");

	db["Salman"].children().push_back("Sun");
	db["Salman"].children().push_back("Prashar");
	dbp.db() = db;
}

//----< Method to get value given by key>--------------
void getValueOfKey()
{
	DbProvider dbp;
	DbCore<Payload> db = dbp.db();
	std::string key = "Fawcett";
	cout << "\n  Value of Key: " << key;
	Query<Payload> query(db);
	query.getValue(key).show();
	putLine(2);
}

//----< Method to get Children of given key>--------------
void getChildrenOfKey()
{
	DbProvider dbp;
	DbCore<Payload> db = dbp.db();
	std::string key = "Salman";
	cout << "\n  Showing children of " << key;
	Query<Payload> query(db);
	query.getChildren(key).show();
	putLine(2);
}

//----< Method to get union of keys>--------------
void getUnionOfQuery()
{
	DbProvider dbp;
	DbCore<Payload> db = dbp.db();
	Query<Payload> query1(db), query2(db), resultQuery(db);
	Condition cond1, cond2;
	regex descrpRegEx("(TA)(.*)");
	regex nameRegEx("(J)(.*)");
	cond1.setDescriptionRegex(descrpRegEx);
	cout << "\n  Performing Query1 ( desc starting with \"TA\")";
	query1.select(cond1).show();
	cond2.setNameRegex(nameRegEx);
	cout << "\n  Performing Query2: ( name starting with \"J\"";
	query2.select(cond2).show();
	cout << "\n  Performing Union of query1 and query2 (OR operation)";
	resultQuery.select(cond2).from(query1.keys()).show();
	putLine();
}

//----< Method to get intersection of queries>--------------
void getIntersectionOfQuery()
{
	DbProvider dbp;
	DbCore<Payload> db = dbp.db();
	Query<Payload> query1(db);
	Query<Payload> query2(db);
	Query<Payload> resultQuery(db);
	Condition cond1, cond2;
	regex descrpRegEx("(TA)(.*)");
	regex nameRegEx("(J)(.*)");
	cond1.setDescriptionRegex(descrpRegEx);
	cond2.setNameRegex(nameRegEx);
	cout << "\n  Performing Query1 (description starting with \"TA\"): ";
	query1.select(cond1).show();
	putLine();
	cout << "\n  Performing Query2 (name with \"J\")";
	query2.select(cond2).show();
	putLine();
	cout << "\n  Performing Query2 on set of keys from Query1 (AND): ";
	resultQuery.select(cond1).select(cond2).show();
	putLine(2);
}


int main()
{
	Utilities::Title("Testing Query class");
	DbProvider dbp;
	DbCore<Payload> db = dbp.db();
	std::cout << "creating demo database";
	createDB();
	createChildren();
	std::cout << "Query to get the value of given key";
	getValueOfKey();
	std::cout << "Query to get the children of given key";
	getChildrenOfKey();
	std::cout << "Query to perform union of two queries";
	getUnionOfQuery();
	std::cout << "Query to perform intersection of two queries";
	getIntersectionOfQuery();

	std::cin.get();
}
#endif
