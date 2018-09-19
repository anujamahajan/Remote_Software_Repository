#include "XmlDocument.h"
#include "XmlElement.h"


using namespace XmlProcessing;


#ifdef TEST_XMLSTORAGE
#include "../DbCore/DbCore.h"
#include "../HelperUtilities/StringUtilities/StringUtilities.h"
using namespace Utilities;
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

//----< Loads database from XML >----------------------------
template<typename T>
void createDatabaseFromXML(std::string filepath, DbCore<T>& db)
{
	using Sptr = std::shared_ptr<AbstractXmlElement>;
	XmlDocument newXDoc(filepath, XmlDocument::file);
	std::vector<Sptr> found = newXDoc.element("database").select();
	std::vector<Sptr> records = newXDoc.descendents("dbRecord").select();
	//Iterate through all tags in element - each record tag represents key-value pair in database
	for (auto pRecord : records) {
		std::string key;
		DbElement<Payload> element;
		AbstractXmlElement::Attributes attribs = pRecord->attributes();
		if (attribs.size() > 0) {
			key = attribs[0].second;
		}
		std::vector<Sptr> pChildren = pRecord->children();
		element.name(pChildren[0]->children()[0]->value());
		for (auto pChild : pChildren) {

			if (pChild->tag() == "description") {
				if (pChild->children().size() > 0)
					element.descrip(pChild->children()[0]->value());
			}
			else if (pChild->tag() == "date") {
				if (pChild->children().size() > 0)
					element.dateTime(pChild->children()[0]->value());
			}
			else if (pChild->tag() == "children") {
				std::vector<Sptr> pElementChildren = pChild->children();
				if (pElementChildren.size() > 0) {
					for (auto pElementChild : pElementChildren) {
						if (pElementChild->tag() == "child")
							element.addChildren(pElementChild->children()[0]->value());
					}
				}
			}
			else if (typeid(T) == typeid(Payload)) {
				savePayloadData(pChild, element);
			}
		}
		db[key] = element;
	}
}

//----< Methos to load payload data from XML into db >----------------------------
using Sptr = std::shared_ptr<AbstractXmlElement>;
void savePayloadData(Sptr child, DbElement<Payload>& element)
{
	std::vector<Sptr> pElementChildren = child->children();
	if (pElementChildren.size() > 0) {
		for (auto pElementChild : pElementChildren) {
			if (pElementChild->tag() == "filepath") {
				if (pElementChild->children().size() > 0)
					element.payLoad().filePath(pElementChild->children()[0]->value());
			}
			else {
				std::vector<Sptr> pCategories = pElementChild->children();
				if (pCategories.size() > 0) {
					for (auto pCategory : pCategories) {
						if (pCategory->tag() == "category")
							element.payLoad().addCategories(pCategory->children()[0]->value());
					}
				}
			}
		}
	}
}

//----< Method to create database>--------------
void createDB()
{
	DbProvider dbp;
	DbCore<Payload> db = dbp.db();
	std::cout << "\n  Creating Database";
	DbElement<Payload> dbElem;
	std::string s = "Utility";
	std::vector<std::string> c = { "DbCore","Utilities" };
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
	std::vector<std::string> categories = { "DbCore","Query","Test" };
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



int main()
{
	Utilities::Title("Demonstatres XML storage and creating database from XML");
	DbProvider dbp;
	DbCore<Payload> db;
	createDB();
	createChildren();
	db = dbp.db();
	showDb(db);
	std::cout << "\n  Creating XML from database";
	std::string dbXML = db.saveDB("../database.xml");
	std::cout << "\n  " << dbXML;
	std::cout << "\n  Building database from XmlDocument";
	std::cout << "\n ----------------------------------";
	DbCore<Payload> newDb;
	createDatabaseFromXML("../database.xml",newDb);
	showDb(newDb);
	std::cin.get();
	std::cout << "\n\n";
	return 0;
}
#endif