#pragma once
#include "windows.h"
#include "iostream"
#include "Dbt.h"
#include "WatcherArray.h"
#include "time.h"

ref class DrivesFunctions
{
public:

	DrivesFunctions(void)
	{
	}

	static bool * CheckExistingRemovableDrives()
	{
		bool * drives = new bool[26];
		for(int i=0; i<26; i++)
			drives[i]=false;
		DWORD mask = GetLogicalDrives();
		DWORD twosPower=1;
		int index=0;

		while(twosPower<2147483648)
		{
			if(mask & twosPower)
			{
				if(strcmp( "Removable",DrivesFunctions::DetermineDriveType(char(index+65)) ) == 0)
					drives[index] = 1;
			}
			twosPower*=2;
			index++;
		}
		return drives;
	}

	static char EvaluateMask(DWORD Mask)
	{
		DWORD twosPower=1;
		int driveName=65;

		while(twosPower<2147483648)
		{
			if(Mask & twosPower)
				return driveName;
			twosPower*=2;
			driveName++;
		}
		return '0';
	}

	static char * DetermineDriveType(char Drive)
	{
		char returnval[50]="";
		CHAR devicename[4];
		UINT DriveType;
		CHAR volumeNameBuffer[512];
		DWORD serialNumber;
		DWORD maxComponentLength;
		DWORD fileSystemFlags;
		CHAR fileSystemName[128];
		int retval;

		devicename[0]=Drive;
		devicename[1]=':';
		devicename[2]='\\';
		devicename[3]='\0';

		retval = GetVolumeInformationA(devicename,
							 volumeNameBuffer,
							 sizeof(volumeNameBuffer),
							 &serialNumber,
							 &maxComponentLength,
							 &fileSystemFlags,
							 fileSystemName,
							 sizeof(fileSystemName));
		DriveType = GetDriveTypeA(devicename);
		if(retval)
		{			
			if(DriveType==DRIVE_UNKNOWN)
				strcat(returnval,"Unknown");
			if(DriveType==DRIVE_NO_ROOT_DIR)
				strcat(returnval,"No root DIR");
			if(DriveType==DRIVE_REMOVABLE)
				strcat(returnval,"Removable");
			if(DriveType==DRIVE_FIXED)
				strcat(returnval,"Fixed");
			if(DriveType==DRIVE_REMOTE)
				strcat(returnval,"Remote");
			if(DriveType==DRIVE_CDROM)
				strcat(returnval,"CD-ROM");
			if(DriveType==DRIVE_RAMDISK)
				strcat(returnval,"RAMDISK");
		}
		return returnval;
	}
};
