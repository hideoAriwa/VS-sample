#pragma once
#include "windows.h"
#include "iostream"
#include "Dbt.h"
#include "time.h"
#include "DrivesFunctions.h"
#include "StringManipulations.h"

using System::IO::FileSystemWatcher;
using System::IO::NotifyFilters;
using System::IO::FileSystemEventArgs;
using System::IO::RenamedEventArgs;
using System::IO::FileSystemEventHandler;
using System::IO::RenamedEventHandler;

ref class WatcherArray
{
private:
	//Array of FileSystemWatcher objects
	array<FileSystemWatcher^>^ watcherArray;
	//FILE pointer for the log file
	FILE *fOutput;

   void onRenamed( Object^ sender, RenamedEventArgs^ e )
   {	   
	   char dateString[9], timeString[9];
	   //These functions (from time.h) retrieve the current date and time
	   _strdate(dateString); _strtime(timeString);
	   openFile();
	   fprintf(fOutput,"%s Renamed to %s on %s at %s\n",e->OldFullPath->ToCharArray(),e->FullPath->ToCharArray(),dateString,timeString);
	   closeFile();
   }

   void onCreated( Object^ sender, FileSystemEventArgs^ e )
   {
	   char dateString[9], timeString[9];
	   _strdate(dateString); _strtime(timeString);
	   openFile();
	   fprintf(fOutput,"%s Created on %s at %s\n",e->FullPath->ToCharArray(),dateString,timeString);
	   closeFile();
   }

   void onDeleted( Object^ sender, FileSystemEventArgs^ e )
   {
	   char dateString[9], timeString[9];
	   _strdate(dateString); _strtime(timeString);
	   openFile();
	   fprintf(fOutput,"%s Deleted on %s at %s\n",e->FullPath->ToCharArray(),dateString,timeString);
	   closeFile();
   }


	
public:
	String^ filePath;

	WatcherArray(void)
	{
		initializeArray();
	}

	void initializeArray(void)
	{
		//Create 26 FileSystemWatcher objects, one for each drive
		//0 respresents A drive, 1 represents B drive, ... , 25 represents Z drive
		watcherArray = gcnew array<FileSystemWatcher^>(26);
		//Check for already connected removable devices so that their activity can be logged straight away
		bool * drives = DrivesFunctions::CheckExistingRemovableDrives();
		for(int i=0; i<26; i++)
		{
			watcherArray[i] = gcnew FileSystemWatcher();
			//If there is a removable device, set FileSystemWatcher object's path and start monitoring
			if(drives[i])
			{
				watcherArray[i]->Path = StringManipulations::AsciiToChar(i+65) + ":\\";
				watcherArray[i]->EnableRaisingEvents = true;
			}
			//Otherwise, let it remain passive
			else
			{
				watcherArray[i]->Path = "";
				watcherArray[i]->EnableRaisingEvents = false;
			}
			//Set the desired filters
			watcherArray[i]->NotifyFilter = static_cast<NotifyFilters>(NotifyFilters::LastAccess |
				NotifyFilters::LastWrite | NotifyFilters::FileName | NotifyFilters::DirectoryName);
			//Register functions with each change type
			watcherArray[i]->Created += gcnew FileSystemEventHandler(this,&WatcherArray::onCreated);
			watcherArray[i]->Deleted += gcnew FileSystemEventHandler(this,&WatcherArray::onDeleted);
			watcherArray[i]->Renamed += gcnew RenamedEventHandler(this,&WatcherArray::onRenamed);
		}
	}

	
	/*void setFilePath(String^ val) //sets the path of the output file
	{
		filePath = StringManipulations::StringToCharArray(val);
	}*/

	void closeFile()
	{	
		if(fOutput!=NULL)
			fclose(fOutput);
	}

	void openFile()
	{
		fOutput = fopen(StringManipulations::StringToCharArray(filePath),"a");
	}

	void beginWatcherOnDrive(char driveLetter)
	{
		//Since 0->A, 1->B and so on, extract the desired index from the letter by subtracting 65 (A's ascii value)
		int index = driveLetter-65;
		if( index >=0 && index<26)
		{
			//If there hasnt been a drive on this letter, set its path
			if(watcherArray[index]->Path == "")
				watcherArray[index]->Path = StringManipulations::AsciiToChar(int(driveLetter)) + ":\\";
			//Start monitoring
			watcherArray[index]->EnableRaisingEvents = true;
		}
	}
	
	//Stops monitoring on a given drive
	void stopWatcherOnDrive(char driveLetter)
	{
		int index = driveLetter-65;
		if( index>=0 && index<26)
		{
			watcherArray[index]->EnableRaisingEvents = false;
		}
	}
};
