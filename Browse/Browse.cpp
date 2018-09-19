///////////////////////////////////////////////////////////////////////
// Browse.h - Provides test stub for browse                          //
// ver 1.0                                                           //
// Language:    Visual C++ 11                                        //
// Application: CSE687 - Object Oriented Design                      //
// Author:      Anuja Mahajan, Syracuse University(MSCS-Spring 2018) //
//				apmahaja@syr.edu									 //
///////////////////////////////////////////////////////////////////////

#include "Browse.h"

#ifdef TEST_BROWSE
#include "../PayLoad/PayLoad.h"
int main()
{
	std::cout << "\n  Demonstrating code pop-up in notepad";
	std::cout << "\n ======================================";

	Process p;
	p.title("test application");
	std::string appPath = "c:/windows/system32/notepad.exe";
	p.application(appPath);

	std::string cmdLine = "/A ../Browse/Browse.h";
	p.commandLine(cmdLine);

	std::cout << "\n  starting process: \"" << appPath << "\"";
	std::cout << "\n  with this cmdlne: \"" << cmdLine << "\"";
	p.create();

	CBP callback = []() { std::cout << "\n  --- child process exited with this message ---"; };
	p.setCallBackProcessing(callback);
	p.registerCallback();

	std::cout << "\n  after OnExit";
	std::cout.flush();
	char ch;
	std::cin.read(&ch, 1);
	return 0;
}

#endif