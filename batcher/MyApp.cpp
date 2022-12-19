/**
	 * Class Name:		myApp.cpp
	 * Purpose:			reads the txt file to run all the command in different groups
	 * Coder:			Dylan Mahyuddin
	 * Date:			2022-10-13
*/
#include "ConsoleApp.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>
using namespace std;

//hold the different details of the txt reading
struct commandDetails {
	size_t groupNum = 0;
	wstring commandExe = L"";
	wstring commandArgs = L"";
	DWORD exitCode = 0;

	friend bool operator< (const commandDetails& lhs, const commandDetails& rhs) {

		return lhs.groupNum < rhs.groupNum;
	}
};


// https://www.positioniseverything.net/cpp-string-trim
// by osition is Everything
// edited by Dylan Mahyuddin 
wstring myTrimFunction(string str) {
	int start = 0;
	int end = str.size() - 1;
	wstring trimmedString;
	//find white spaces from start
	for (int i = 0; i <= end; i++) {
		if (str[i] != ' ') {
			start = i;  break;
		}
	}
	//find whitespaces from end
	for (int i = end; end >= 0; i--) {
		if (str[i] != ' ') {
			end = i;  break;
		}
	}
	//now trim
	for (int i = start; i <= end; i++) {
		trimmedString += str[i];
	}    return trimmedString;
}

void launch_using_create_process(wstring command, PROCESS_INFORMATION &pi) {

	STARTUPINFO sinfo{ 0 };
	sinfo.cb = sizeof(STARTUPINFO);
	unsigned long const CP_MAX_COMMANDLINE = 32768;
	try
	{
		wchar_t* commandLine = new wchar_t[CP_MAX_COMMANDLINE];
		wcsncpy_s(commandLine, CP_MAX_COMMANDLINE, command.c_str(), command.size() + 1);

		auto res = CreateProcess(
			NULL,
			commandLine,
			NULL,
			NULL,
			false,
			CREATE_NEW_CONSOLE,
			NULL,
			NULL,
			&sinfo,
			&pi
		);
		if (res == 0)
		{
			cerr << "Error " << GetLastError << endl;
		}
		delete[] commandLine;
	}
	catch (std::bad_alloc&)
	{
		wcerr << L"Insufficient memory to launch the app";
		return;
	}
}


MAKEAPP(myApp) {
	if (ConsoleApp::get_args()[1] != "") {
		ifstream commandFile = (ifstream)ConsoleApp::get_args()[1];
		string line, token;
		unsigned index, currCommand;
		vector<commandDetails> commandGroups;
		vector<PROCESS_INFORMATION> allProcess;
		if (commandFile){
			
			//file reading 
			while (getline(commandFile, line)) {
				istringstream word(line);
				index = 0;
				commandDetails currDetails;

				while (getline(word, token, ',')) {
					++index;
					switch (index)
					{
					case 1:
						currDetails.groupNum = stoi(token);
						currCommand = stoi(token);
						break;
					case 2:
						currDetails.commandExe = myTrimFunction(token);
						break;
					case 3:
						currDetails.commandArgs =  myTrimFunction(token);
					default:
						break;
					}
				}

				commandGroups.push_back(currDetails);
			}
			commandFile.close();
			std::sort(commandGroups.begin(), commandGroups.end());
			index = 0;
			unsigned groupStartIndex = 0;
			unsigned currLaunchGroup = commandGroups[0].groupNum;

			for (size_t i = 0; i < commandGroups.size();) {
				currLaunchGroup = commandGroups[i].groupNum;
				//creating the group process
				for (size_t j = groupStartIndex; j < commandGroups.size();++j) {
					if (commandGroups[j].groupNum == currLaunchGroup) {
						allProcess.push_back({ 0 });
						wstring wCommand = commandGroups[j].commandExe + L" " + commandGroups[j].commandArgs;
						launch_using_create_process(wCommand, allProcess[index]);
						index++;
					} else {
						break;
					}
				}
				//waiting for the current group 
				for (size_t j = groupStartIndex; j < index; j++) {
					if (WAIT_FAILED == WaitForSingleObject(allProcess[j].hProcess, INFINITE))
						cerr << "Failure waiting for process to terminate" << endl;
				}
				i = index;
				groupStartIndex = i;
			}
			index = 0;
			currLaunchGroup = 0;

			//Launch Time Reports 
			cout << "Launch Times" << endl;

			for (auto& x : commandGroups) {
				if (currLaunchGroup != x.groupNum) {
					cout << "\nGroup:" << x.groupNum << endl;
					currLaunchGroup = x.groupNum;
				}	
				DWORD exitCode = 0;
				GetExitCodeProcess(allProcess[index].hProcess, &exitCode);
				x.exitCode = exitCode;
				FILETIME createtionTime, exitTime, kernelTime, userTime;
				GetProcessTimes(allProcess[index].hProcess, &createtionTime, &exitTime, &kernelTime, &userTime);
				SYSTEMTIME kTime, uTime;
				FileTimeToSystemTime(&kernelTime, &kTime);
				FileTimeToSystemTime(&userTime, &uTime);
				printf("K:%02d:%02d.%03d U:%02d:%02d.%03d E:%d G:%d ",
					kTime.wMinute, kTime.wSecond, kTime.wMilliseconds,
					uTime.wMinute, uTime.wSecond, uTime.wMilliseconds,
					x.exitCode,
					x.groupNum);

				cout << " " << string(x.commandExe.begin(), x.commandExe.end())
					<< " " << string(x.commandArgs.begin(), x.commandArgs.end())
					<< endl;

				CloseHandle(allProcess[index].hThread);
				CloseHandle(allProcess[index].hProcess);
				index++;
				
			}

			//Error Code Reports
			cout << "\nErrors" << endl;
			index = 0;
			for (auto& x : commandGroups) {
				if (x.exitCode != 0) {
					cout << "G#: " << x.groupNum << "  Command: " << string(x.commandExe.begin(), x.commandExe.end()) << "  Params: " << string(x.commandArgs.begin(), x.commandArgs.end()) << endl;
					cout << " ---> Error = " << x.exitCode << endl;

				}
			}
		}
	}

	return 0;
}