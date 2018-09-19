#pragma once
///////////////////////////////////////////////////////////////////////
// DbCore.h - Implements NoSQL database prototype                    //
// ver 1.0                                                           //
// Language:    Visual C++ 11                                        //
// Application: CSE687 - Object Oriented Design                      //
// Author:      Anuja Mahajan, Syracuse University(MSCS-Spring 2018) //
//				apmahaja@syr.edu									 //
// Source:      Jim Fawcett, CST 4-187, Syracuse University          //
//              (315) 443-3948, jfawcett@twcny.rr.com                //
///////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides three classes:
* - DbCore implements core NoSql database operations, addition, deletion and edition.
* - DbElement provides the value part of our key-value database and methods to access element details.
*   It contains fields for name, description, date, child collection
*   and a payload field of the template type.
*   The package also provides functions for displaying:
* - set of all database keys
* - database elements
* - all records in the database
* -Payload class provides the file name and categories associated with it.

* Required Files:
* ---------------
* DateTime.h, DateTime.cpp
*
* Maintenance History:
* --------------------
* ver 1.2 : 04 Feb 2018
* - added code to
* ver 1.1 : 19 Jan 2018
* - added code to throw exception in index operators if input key is not in database
* ver 1.0 : 10 Jan 2018
* - first release
*/

#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include "../DateTime/DateTime.h"
#include <fstream>
#include "../XMLStorage/XmlDocument.h"
#include "../XMLStorage/XmlElement.h"
#include <filesystem>
#include "../PayLoad/PayLoad.h"

using namespace XmlProcessing;
using namespace PayLoad;

namespace NoSqlDb
{

	/////////////////////////////////////////////////////////////////////
	// DbElement class
	// - provides the value part of a NoSql key-value database

	template<typename T>
	class DbElement
	{
	public:
		//Default constructor for DbElement
		DbElement() {}
		using Key = std::string;
		using Children = std::vector<Key>;
		// methods to get and set DbElement fields
		std::string& name() { return name_; }
		std::string name() const { return name_; }
		void name(const std::string& name) { name_ = name; }

		std::string& descrip() { return descrip_; }
		std::string descrip() const { return descrip_; }
		void descrip(const std::string& name) { descrip_ = name; }

		DateTime& dateTime() { return dateTime_; }
		DateTime dateTime() const { return dateTime_; }
		void dateTime(const DateTime& dateTime) { dateTime_ = dateTime; }

		Children& children() { return children_; }
		Children children() const { return children_; }
		void children(const Children& children) { children_ = children; }

		T& payLoad() { return payLoad_; }
		T payLoad() const { return payLoad_; }
		void payLoad(const T& payLoad) { payLoad_ = payLoad; }
		//method to add child key
		void addChildren(std::string key) {
			children_.push_back(key);
		}
		//method to check if payload is empty or not
		bool isEmpty() {
			if (name_ == "" && descrip_ == "" && isDateTimeCurrent(dateTime_) && payLoad_.isEmpty()) {
				return true;
			}
			else
				return false;
		}

	private:
		std::string name_;
		std::string descrip_;
		DateTime dateTime_;
		Children children_;
		T payLoad_;
		bool isDateTimeCurrent(DateTime date);
	};

	/////////////////////////////////////////////////////////////////////
	// DbCore class
	// - provides core NoSql db operations
	// - does not provide editing, querying, or persistance operations

	template <typename T>
	class DbCore
	{
	public:
		using Key = std::string;
		using Keys = std::vector<Key>;
		using Children = Keys;
		using DbStore = std::unordered_map<Key, DbElement<T>>;
		using iterator = typename DbStore::iterator;
		// methods to access database elements
		Keys keys();
		bool contains(const Key& key);
		size_t size();
		void throwOnIndexNotFound(bool doThrow) { doThrow_ = doThrow; }
		DbElement<T>& operator[](const Key& key);
		DbElement<T> operator[](const Key& key) const;
		typename iterator begin() { return dbStore_.begin(); }
		typename iterator end() { return dbStore_.end(); }
		// methods to get and set the private database hash-map storage
		DbStore& dbStore() { return dbStore_; }
		DbStore dbStore() const { return dbStore_; }
		void dbStore(const DbStore& dbStore) { dbStore_ = dbStore; }
		//method to save the database to XML file
		std::string saveDB(std::string filepath);
		static void identify(std::ostream& out = std::cout);
		//DbCore<T>& createDBFromXml(std::string filePath,DbCore<T> db);

	private:
		DbStore dbStore_;
		bool doThrow_ = false;
	};


	// ----< method to print current file path>--------------------------------------
	template<typename P>
	void DbCore<P>::identify(std::ostream& out)
	{
		out << "\n  \"" << __FILE__ << "\"";
	}

	// ----< method to compare given date with current date time>-------------------------
	template<typename T>
	bool DbElement<T>::isDateTimeCurrent(DateTime date)
	{
		std::string time = date.time();
		std::string currentTime = DateTime().time();
		if (time.compare(currentTime) == 0)
			return true;
		return false;
	}

	/////////////////////////////////////////////////////////////////////
	// display functions
	//----< display database key set >-----------------------------------
	template<typename T>
	void showKeys(DbCore<T>& db, std::ostream& out = std::cout)
	{
		out << "\n  ";
		for (auto key : db.keys()) {
			out << key << " ";
		}
	}

	//----< show record header items >-----------------------------------
	inline void showHeader(std::ostream& out = std::cout)
	{
		out << "\n  ";
		out << std::setw(35) << std::left << "Key";
		out << std::setw(30) << std::left << "DateTime";
		out << std::setw(20) << std::left << "Name";
		out << std::setw(40) << std::left << "Description";
		out << std::setw(100) << std::left << "Payload";
		out << "\n  ";
		out << std::setw(35) << std::left << "--------------------";
		out << std::setw(30) << std::left << "------------------------";
		out << std::setw(20) << std::left << "-----------------";
		out << std::setw(40) << std::left << "----------------------------";
		out << std::setw(100) << std::left << "-----------------------";
	}

	//----< display DbElements >-----------------------------------------
	template<typename T>
	void showElem(std::string const key, DbElement<T>& el, std::ostream& out = std::cout)
	{
		out << "\n  ";
		out << std::setw(35) << std::left << key;
		out << std::setw(30) << std::left << std::string(el.dateTime());
		out << std::setw(20) << std::left << el.name();
		out << std::setw(40) << std::left << el.descrip();
		out << std::setw(100) << std::left << el.payLoad().filePath() << ":" << el.payLoad().status();
		typename DbElement<T>::Children children = el.children();
		if (children.size() > 0) {
			out << "\n    child keys: ";
			for (auto key : children) {
				out << " " << key;
			}
		}
	}

	//----< display all records in database >----------------------------
	template<typename T>
	void showDb(const DbCore<T>& db, std::ostream& out = std::cout)
	{
		showHeader(out);
		typename DbCore<T>::DbStore dbs = db.dbStore();
		for (auto item : dbs) {
			showElem(item.first, item.second, out);
		}
	}

	/////////////////////////////////////////////////////////////////////
	// DbCore<T> methods
	//----< does db contain this key? >----------------------------------
	template<typename T>
	bool DbCore<T>::contains(const Key& key)
	{
		iterator iter = dbStore_.find(key);
		return iter != dbStore_.end();
	}

	//----< returns current key set for db >-----------------------------
	template<typename T>
	typename DbCore<T>::Keys DbCore<T>::keys()
	{
		DbCore<T>::Keys dbKeys;
		DbStore& dbs = dbStore();
		size_t size = dbs.size();
		dbKeys.reserve(size);
		for (auto item : dbs) {
			dbKeys.push_back(item.first);
		}
		return dbKeys;
	}
	//----< return number of db elements >-------------------------------

	template<typename T>
	size_t DbCore<T>::size()
	{
		return dbStore_.size();
	}
	//----< extracts value from db with key >----------------------------
	/*
	*  - indexes non-const db objects
	*  - In order to create a key-value pair in the database like this:
	*      db[newKey] = newDbElement
	*    we need to index with the new key and assign a default element.
	*    That will be replaced by newDbElement when we return from operator[]
	*  - However, if we want to index without creating new elements, we need
	*    to throw an exception if the key does not exist in the database.
	*  - The behavior we get is determined by doThrow_.  If false we create
	*    a new element, if true, we throw. Creating new elements is the default
	*    behavior.
	*/
	template<typename T>
	DbElement<T>& DbCore<T>::operator[](const Key& key)
	{
		if (!contains(key))
		{
			if (doThrow_)
				throw(std::exception("key does not exist in db"));
			else
				return (dbStore_[key] = DbElement<T>());
		}
		return dbStore_[key];
	}
	//----< extracts value from db with key >----------------------------
	/*
	*  - indexes const db objects
	*/
	template<typename T>
	DbElement<T> DbCore<T>::operator[](const Key& key) const
	{
		if (!contains(key))
		{
			throw(std::exception("key does not exist in db"));
		}
		return dbStore_[key];
	}

	//----< Saves the Database to specified xml file>----------------------------
	template<typename T>
	std::string DbCore<T>::saveDB(std::string filepath)
	{
		using Sptr = std::shared_ptr<AbstractXmlElement>;
		Sptr pDb = makeTaggedElement("database");
		Sptr pDocElem = makeDocElement(pDb);
		XmlDocument xDoc(pDocElem);
		namespace fs = std::experimental::filesystem;
		fs::path p = filepath;
		std::cout << "\n  Database stored in the location:  \n  " << fs::absolute(p);
		std::unordered_map<std::string, DbElement<T>>::iterator it;
		for (auto it = dbStore_.begin(); it != dbStore_.end(); ++it) {
			Sptr pRecord = makeTaggedElement("dbRecord");
			pRecord->addAttrib("key", it->first);
			pDb->addChild(pRecord);
			Sptr pName = makeTaggedElement("name", it->second.name());
			pRecord->addChild(pName);
			Sptr pDescrip = makeTaggedElement("description", it->second.descrip());
			pRecord->addChild(pDescrip);
			Sptr pDateTime = makeTaggedElement("date", it->second.dateTime().time());
			pRecord->addChild(pDateTime);
			Sptr pChildren = makeTaggedElement("children");
			pRecord->addChild(pChildren);
			for (auto child : it->second.children()) {
				Sptr pChild = makeTaggedElement("child", child);
				pChildren->addChild(pChild);
			}
			//Checking for Payload type
			if (typeid(T) == typeid(Payload)) {
				Sptr pPayload = makeTaggedElement("payload");
				pRecord->addChild(pPayload);
				Sptr pfilePath = makeTaggedElement("filepath", it->second.payLoad().filePath());
				pPayload->addChild(pfilePath);
				Sptr pStatus = makeTaggedElement("status", it->second.payLoad().status());
				pPayload->addChild(pStatus);
				Sptr pAuhtor = makeTaggedElement("author", it->second.payLoad().author());
				pPayload->addChild(pAuhtor);
				Sptr pCategories = makeTaggedElement("categories");
				pPayload->addChild(pCategories);
				for (auto category : it->second.payLoad().categories()) {
					Sptr pCategory = makeTaggedElement("category", category);
					pCategories->addChild(pCategory);
				}
			}
		}
		std::ofstream dbFile(filepath);
		if (dbFile.is_open())
			dbFile << xDoc.toString();
		dbFile.close();
		return xDoc.toString();
	}

	//----< Checks if the file specified by string exists>----------------------------
	bool isFileExists(std::string filepath)
	{
		std::ifstream newfile(filepath);
		return newfile.good();
	}

	namespace fs = std::experimental::filesystem;
	//----< Loads database from XML >----------------------------
	template<typename T>
	void createDBFromXML(std::string filepath, DbCore<T>& db)
	{
		using Sptr = std::shared_ptr<AbstractXmlElement>;
		fs::path p = filepath;
		std::cout << "\n  " << fs::absolute(p);
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

			for (auto pChild : pChildren) {
				if (pChild->children().size() > 0)
				{
					if (pChild->tag() == "name") {
						element.name(pChild->children()[0]->value());
					}
					else if (pChild->tag() == "description") {
						element.descrip(pChild->children()[0]->value());
					}
					else if (pChild->tag() == "date") {
						element.dateTime(pChild->children()[0]->value());
					}
					else if (pChild->tag() == "children") {
						saveChildrenData(pChild, element);
					}
					else if (typeid(T) == typeid(Payload)) {
						savePayloadData(pChild, element);
					}
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
				else if (pElementChild->tag() == "status")
				{
					if (pElementChild->children().size() > 0)
						element.payLoad().status(pElementChild->children()[0]->value());
				}
				else if (pElementChild->tag() == "author")
				{
					if (pElementChild->children().size() > 0)
						element.payLoad().author(pElementChild->children()[0]->value());
				}
				else
				{
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

	//----< Methos to load payload data from XML into db >----------------------------
	using Sptr = std::shared_ptr<AbstractXmlElement>;
	void saveChildrenData(Sptr child, DbElement<Payload>& element)
	{
		std::vector<Sptr> pElementChildren = child->children();
		if (pElementChildren.size() > 0) {
			for (auto pElementChild : pElementChildren) {
				if (pElementChild->tag() == "child")
				{
					if(pElementChild->children().size() > 0)
						element.addChildren(pElementChild->children()[0]->value());
				}
					
			}
		}
	}
}
