//////////////////////////////////////////////////////////////////////////////////
// ServerPrototype.cpp- Processes client reuqests                               //
// ver 1.0                                                                      //
// Language:    C++, Visual Studio 2017                                         //
// Application: Most Projects, CSE687 - Object Oriented Design                  //
// Source:      Jim Fawcett, CSE687 - Object Oriented Design, Spring 2018       //
// Author:      Anuja Mahajan, Syracuse University(MSCS-Spring 2018)            //
//				apmahaja@syr.edu									            //
//////////////////////////////////////////////////////////////////////////////////

/*
* Package Operations:
* -------------------
* - Provides server processes to Check-In, Check-Out, Browse client and server repository.
* - Provides server processes of View Meta Data and View file content after double clicking File
* - Provides processes to echo client msgs
* - Provides server processes to filter repository files based on given criteria
* Required Files:
* ---------------
* ServerPrototype.h
*
* Maintenance History:
* --------------------
* ver 1.0 : 30 Mar 2018
* - first release
* ver 1.1: 10 Apr 2018
* - Implemented server processes
* ver 1.2: 30Apr 2018
* - Implemented server processes for checkin, close checkin, browse, checkout
*/

#include "ServerPrototype.h"
#include "../FileSystem/FileSystem.h"
#include <chrono>
#include "../RepositoryCore/RepositoryCore.h"


namespace MsgPassComm = MsgPassingCommunication;

using namespace Repository;
using namespace FileSystem;
using Msg = MsgPassingCommunication::Message;

using namespace NoSqlDb;
using namespace PayLoad;

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

//----< respond to getFiles command >-----------------------------------------
Files Server::getFiles(const Repository::SearchPath& path)
{
  return Directory::getFiles(path);
}

//----< respond to getDirs command >-----------------------------------------
Dirs Server::getDirs(const Repository::SearchPath& path)
{
  return Directory::getDirectories(path);
}


namespace MsgPassingCommunication
{
  // These paths, global to MsgPassingCommunication, are needed by 
  // several of the ServerProcs, below.
  // - should make them const and make copies for ServerProc usage

  std::string sendFilePath;
  std::string saveFilePath;

//----< display the message content >-----------------------------------------
template<typename T>
void show(const T& t, const std::string& msg)
{
  std::cout << "\n  " << msg.c_str();
  for (auto item : t)
  {
    std::cout << "\n    " << item.c_str();
  }
}

//----< respond to echo command >-----------------------------------------
std::function<Msg(Msg)> echo = [](Msg msg) {
	std::cout << "\n\n";
  Msg reply = msg;
  reply.to(msg.from());
  reply.from(msg.to());
  return reply;
};

//----< respond to getFiles command >-----------------------------------------
std::function<Msg(Msg)> getFiles = [](Msg msg) {
	std::cout << "\n\n";
  Msg reply;
  reply.to(msg.from());
  reply.from(msg.to());
  reply.command(msg.command());
  reply.attribute("view", msg.value("view"));
  std::string path = msg.value("path");
  if (path != "")
  {
    std::string searchPath = storageRoot;
    if (path != ".")
      searchPath = searchPath + "\\" + path;
    Files files = Server::getFiles(searchPath);
    size_t count = 0;
    for (auto item : files)
    {
      std::string countStr = Utilities::Converter<size_t>::toString(++count);
      reply.attribute("file" + countStr, item);
    }
  }
  else
  {
    std::cout << "\n  getFiles message did not define a path attribute";
  }
  return reply;
};

//----< respond to getDirs command >-----------------------------------------
std::function<Msg(Msg)> getDirs = [](Msg msg) {
	std::cout << "\n\n";
  Msg reply;
  reply.to(msg.from());
  reply.from(msg.to());
  reply.command(msg.command());
  reply.attribute("view", msg.value("view"));
  std::string path = msg.value("path");

  if (path != "")
  {
    std::string searchPath = storageRoot;
    if (path != ".")
      searchPath = searchPath + "\\" + path;
    Files dirs = Server::getDirs(searchPath);
    size_t count = 0;
    for (auto item : dirs)
    {
      if (item != ".." && item != ".")
      {
        std::string countStr = Utilities::Converter<size_t>::toString(++count);
        reply.attribute("dir" + countStr, item);
      }
    }
  }
  else
  {
    std::cout << "\n  getDirs message did not define a path attribute";
  }
  return reply;
};
	
//----< respond to checkIn command >-----------------------------------------
std::function<Msg(Msg)> checkIn = [](Msg msg) {
	std::cout << "\n\n";
	std::cout << "\n  ==========================================================";
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command(msg.command());
	std::string fileName = msg.value("sendingFile");
	std::string package = msg.value("package");
	std::string description = msg.value("description");
	std::string categories = msg.value("categories");
	std::string filePath = saveFilePath +"/" +fileName;
	unordered_map<std::string, std::string> childMap;
	vector<std::string> categoryList;
		
		//Create an instance of database
		DbProvider dbp;
		DbCore<Payload>& db = dbp.db();
		if (FileSystem::File::exists("../database.xml"))
			createDBFromXML("../database.xml", db);
		RepositoryCore<Payload> repository(db);
		if (msg.containsKey("categories"))
		{
			categoryList = Utilities::split(categories,';');
		}
		if (msg.containsKey("children"))
		{
			std::string children = msg.value("children");
			if (children != "")
			{
				vector<std::string> childFiles = Utilities::split(children,';');
				std::cout << "\n  Checking in file: "+fileName +" with dependents";
				for (string childName : childFiles)
				{
					childMap[childName] = package;
				}
				repository.checkInDependent(filePath, package, childMap, description, categoryList);
				reply.attribute("fileName", fileName);
				return reply;
			}			
		}	
		else
		{
			std::cout << "\n  Checking in file: " + fileName;
			repository.checkInRep(filePath, package, description, categoryList);
			reply.attribute("fileName", fileName);
		}
		showDb(db);
	return reply;
};

//----< respond to checkOut command >-----------------------------------------
std::function<Msg(Msg)> checkOut = [](Msg msg) {
	std::cout << "\n\n";
	std::cout << "\n  ==========================================================";
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command(msg.command());
	std::string package = msg.value("package");
	reply.attribute("package", package);
	std::string files = msg.value("fileName");
	std::string version = msg.value("version");
	std::string isDependent = msg.value("dependent");
	vector<std::string> fileList = Utilities::split(files, ';');
	DbProvider dbp;
	DbCore<Payload>& db = dbp.db();
	if (FileSystem::File::exists("../database.xml"))
		createDBFromXML("../database.xml", db);
	RepositoryCore<Payload> repository(db);
	std::vector<std::string> children;
	if (fileList.size() > 1)	{
		for (string file : fileList)
		{
			vector<std::string> childFiles;
			repository.checkOut(file, package);
			childFiles = repository.getChildren(file, package);
			children.insert(std::end(children), std::begin(childFiles), std::end(childFiles));
		}
	}
	else	{
		if (version._Equal("latest") && isDependent._Equal("yes"))	{				
				repository.checkOut(fileList.at(0), package);
				children = repository.getChildren(fileList.at(0), package);
			}			
			else if(version._Equal("latest") && !isDependent._Equal("yes"))
				repository.checkOut(fileList.at(0), package, false);
			else if (!version._Equal("latest") && isDependent._Equal("yes"))	{
				repository.checkOut(fileList.at(0), package, atoi(version.c_str()));
				children = repository.getChildren(fileList.at(0), package, atoi(version.c_str()));
			}
			else
				repository.checkOut(fileList.at(0), package, atoi(version.c_str()),false);
	}
	for (string child : children)
		files = files + child +";";
	reply.attribute("fileName", files);
	return reply;
};

//----< respond to close checkin command >-----------------------------------------
std::function<Msg(Msg)> closeCheckIn = [](Msg msg) {
	std::cout << "\n\n";
	std::cout << "\n  ==========================================================";
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command(msg.command());
	std::string package = msg.value("package");
	std::string files = msg.value("fileName");
	std::string version = msg.value("version");
	std::string isDependent = msg.value("dependent");
	vector<std::string> fileList = Utilities::split(files, ';');
	DbProvider dbp;
	DbCore<Payload>& db = dbp.db();
	if (FileSystem::File::exists("../database.xml"))
		createDBFromXML("../database.xml", db);
	RepositoryCore<Payload> repository(db);
	if (fileList.size() > 0)	{
		std::cout << "\n  Closing status of files: "+files;
		for(string file : fileList)
		{	
			std::string filePath = storageRoot + "\\" + package + "\\" + file;
			repository.closeCheckIn(filePath);
		}
	}
	showDb(db);
	reply.attribute("fileName", files);
	return reply;
};

//----< respond to viewFile command >-----------------------------------------
std::function<Msg(Msg)> viewFile = [](Msg msg) {
	std::cout << "\n\n";
	std::cout << "\n  ==========================================================";
	Msg reply;
	std::string fileName = msg.value("fileName");
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command(msg.command());
	DbProvider dbp;
	DbCore<Payload>& db = dbp.db();
	if (FileSystem::File::exists("../database.xml"))
		createDBFromXML("../database.xml", db);
	RepositoryCore<Payload> repository(db);
	std::string filePath = repository.getFilePath(fileName);
	std::string destFileName = fileName.substr(0, fileName.find_last_of('.'));
	std::string destPath = sendFilePath + "/" + destFileName;
	FileSystem::File::copy(filePath,destPath);
	reply.attribute("sendingFile",destFileName);
	
	return reply;
};

//----< respond to viewMetaData command >-----------------------------------------
std::function<Msg(Msg)> viewMetaData = [](Msg msg) {
	std::cout << "\n\n";
	std::cout << "\n  ==========================================================";
	Msg reply;
	std::string fileName = msg.value("fileName");
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command(msg.command());
	reply.attribute("fileName", fileName);

	DbProvider dbp;
	DbCore<Payload>& db = dbp.db();
	if (FileSystem::File::exists("../database.xml"))
		createDBFromXML("../database.xml", db);
	RepositoryCore<Payload> repository(db);

	DbElement<Payload> elem = repository.getElement(fileName);
	reply.attribute("fileName",elem.name());
	reply.attribute("description", elem.descrip());
	reply.attribute("status",elem.payLoad().status());
	reply.attribute("date",elem.dateTime());
	reply.attribute("author",elem.payLoad().author());
	reply.attribute("categories",elem.payLoad().getCategories());
	string childFileNames = "";
	for (string child : elem.children())
	{
		string childFileName = child.substr(child.find_last_of(':')+1);
		childFileNames = childFileNames + childFileName + ";";
	}
	reply.attribute("children", childFileNames);
	showDb(db);
	return reply;
};

//----< respond to browse command >-----------------------------------------
std::function<Msg(Msg)> browse = [](Msg msg) {
	std::cout << "\n\n";
	std::cout << "\n  ==========================================================";
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command(msg.command());
	std::string criteria = msg.value("criteria");
	DbProvider dbp;
	DbCore<Payload>& db = dbp.db();
	if (FileSystem::File::exists("../database.xml"))
		createDBFromXML("../database.xml", db);
	RepositoryCore<Payload> repository(db);
	vector<std::string> fileList;
	if (criteria._Equal("No Parents"))	{
		std::vector<std::string> result= repository.getFilesWithNoParents();
		fileList.insert(fileList.end(), result.begin(), result.end());
	}
	else	{
		vector<std::string> criteriaList = Utilities::split(criteria, ';');
		if (criteriaList.size() > 1)	{
			unordered_map<std::string, std::string> filterCondition;
			for (string parameter : criteriaList)	{
				vector<std::string> parameterList = Utilities::split(parameter, '=');
				if (parameterList.size() == 2)
				{
					filterCondition[parameterList.at(0)] = parameterList.at(1);
				}
			}
			std::vector<std::string> result = repository.browse(filterCondition);
			fileList.insert(fileList.end(), result.begin(), result.end());
		}
		else	{
			vector<std::string> parameterList = Utilities::split(criteriaList.at(0), '=');
			if (parameterList.size() == 2)
			{
				std::vector<std::string> result = repository.browse(parameterList.at(0), parameterList.at(1));
				fileList.insert(fileList.end(), result.begin(), result.end());
			}
		}
	}
	string files = "";
	for (string file : fileList)
		files = files + file + ";";

	reply.attribute("files",files);
	return reply;
};

//----< respond to connect command >-----------------------------------------
std::function<Msg(Msg)> connectServ = [](Msg msg) {
	std::cout << "\n\n";
	std::cout << "\n  ==========================================================";
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command(msg.command());
	return reply;
};
}


using namespace MsgPassingCommunication;

int main()
{
  std::cout << "\n  Server Prototype";
  std::cout << "\n ==========================";
  std::cout << "\n";
  SetConsoleTitle(_T("Repository Server"));
  std::cout << "\n  Demonstrating Requirement #1- Use of C++11";
  std::cout << "\n  ==========================================================";
  std::cout << "\n  " << typeid(std::function<bool()>).name()
	  << ", declared in this function, "
	  << "\n  is only valid for C++11 and later versions.";

  sendFilePath = "../ServerPrototype/SendFiles";
  saveFilePath = "../ServerPrototype/SaveFiles";

  Server server(serverEndPoint, "ServerPrototype");
  DbProvider dbp;
  DbCore<Payload>& db = dbp.db();
  if (FileSystem::File::exists("../database.xml"))
	  createDBFromXML("../database.xml", db);
  MsgPassingCommunication::Context* pCtx = server.getContext();
  pCtx->saveFilePath = saveFilePath;
  pCtx->sendFilePath = sendFilePath;

  server.start();

  //Registers the server processes
  server.addMsgProc("echo", echo);
  server.addMsgProc("getFiles", getFiles);
  server.addMsgProc("getDirs", getDirs);
  server.addMsgProc("serverQuit", echo);
  server.addMsgProc("checkIn",checkIn);
  server.addMsgProc("viewFile", viewFile);
  server.addMsgProc("viewMetaData", viewMetaData);
  server.addMsgProc("checkOut", checkOut);
  server.addMsgProc("connect", connectServ);
  server.addMsgProc("closeCheckIn", closeCheckIn);
  server.addMsgProc("browse",browse);
  server.processMessages();

  std::cout << "\n  press enter to exit";
  std::cin.get();
  std::cout << "\n";

  Msg msg(serverEndPoint, serverEndPoint);
  msg.command("serverQuit");
  server.postMessage(msg);
  server.stop();
  return 0;
}

