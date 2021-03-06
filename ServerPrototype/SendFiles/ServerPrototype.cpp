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
*
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
*/

#include "ServerPrototype.h"
#include "../FileSystem/FileSystem.h"
#include <chrono>
#include "../RepositoryCore/RepositoryCore.h"

namespace MsgPassComm = MsgPassingCommunication;

using namespace Repository;
using namespace FileSystem;
using Msg = MsgPassingCommunication::Message;

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
  Msg reply = msg;
  reply.to(msg.from());
  reply.from(msg.to());
  return reply;
};

//----< respond to getFiles command >-----------------------------------------
std::function<Msg(Msg)> getFiles = [](Msg msg) {
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
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command(msg.command());
	std::string path = msg.value("path");
	std::string package = msg.value("package");
	if (path != "")
	{		
		//Create an instance of database
		DbProvider dbp;
		DbCore<Payload>& db = dbp.db();
		if (FileSystem::File::exists("../database.xml"))
			createDBFromXML("../database.xml", db);
		RepositoryCore<Payload> repository(db);
		if (msg.containsKey("children"))
			std::cout << "Check in dependent";
			//repository.checkInDependent(path, package);
		else
			repository.checkInRep(path,package);
	}
	else
	{
		std::cout << "\n  checkin message did not define a path attribute";
	}
	return reply;
};

//----< respond to checkOut command >-----------------------------------------
std::function<Msg(Msg)> checkOut = [](Msg msg) {
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command(msg.command());
	std::string path = msg.value("path");
	if (path != "")
	{
	}
	else
	{
		std::cout << "\n  checkout message did not define a path attribute";
	}
	return reply;
};

//----< respond to viewFile command >-----------------------------------------
std::function<Msg(Msg)> viewFile = [](Msg msg) {
	Msg reply;
	std::string fileName = msg.value("fileName");
	std::string path = msg.value("path");
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command(msg.command());
	if (path != "")
	{
		std::string sendPath = Path::getFullFileSpec(path);
		reply.file(fileName);
		reply.attribute("path", sendPath);
	}
	else
	{
		std::cout << "\n  viewFile message did not define a path attribute";
	}
	return reply;
};

//----< respond to viewMetaData command >-----------------------------------------
std::function<Msg(Msg)> viewMetaData = [](Msg msg) {
	Msg reply;
	std::string fileName = msg.value("fileName");
	std::string path = msg.value("path");
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command(msg.command());
	reply.attribute("fileName", fileName);
	

	if (path != "")
	{
		std::string author = "Anuja Mahajan";
		std::string description = "Test file";
		std::string filePath = Path::getFullFileSpec(path + "\\" + fileName);
		reply.attribute("author", author);
		reply.attribute("description",description);
		reply.attribute("payload",filePath);

	}
	else
	{
		std::cout << "\n  viewFileMetaData message did not define a path attribute";
	}
	return reply;
};

//----< respond to connect command >-----------------------------------------
std::function<Msg(Msg)> connectServ = [](Msg msg) {
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command(msg.command());
	return reply;
};

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

  Server server(serverEndPoint, "ServerPrototype");
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

