#include "PrtDistDeployer.h"

/* GLobal Variables */
string configurationFile = "..\\PrtDistManager\\PrtDistManConfiguration.xml";
string allBinaries = ".\\";
string localDeploymentFolder = "..\\DeploymentFolder\\";
string vsdllsFolder = "..\\Resources\\VS2013\\";
string pstoolsFolder = "..\\Resources\\PsTools\\";
string scriptsFolder = "..\\Resources\\ScriptsForAzure\\";
#ifdef PRT_ARCH_X86
string prtWinUserDll = "..\\Debug\\Win32\\";
#else
string prtWinUserDll = "..\\Debug\\x64\\";
#endif
char* logFileName = "PRTDIST_DEPLOYER.txt";
FILE* logFile;

char* PrtDGetPathToPHome(char* nodeAddress)
{
	char* path = (char*)malloc(sizeof(char) * 1000);
	strcpy_s(path, 1000, "");
	strcat_s(path, 1000, "\\\\");
	strcat_s(path, 1000, nodeAddress);
	strcat_s(path, 1000, "\\Plang_Shared\\");
	return path;
}

string PrtDDeployPProgram()
{

	string remoteNetworkShare = PrtDistGetNetworkShare();
	string copycommand;
	//create the folder to be deployed in 
	DWORD ftyp = GetFileAttributesA(localDeploymentFolder.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		CreateDirectory(localDeploymentFolder.c_str(), NULL);


	//copy the configuration file
	string configFile = "PrtDistManConfiguration.xml";
	string newFilePath = localDeploymentFolder + configFile;
	CopyFile(configurationFile.c_str(), newFilePath.c_str(), FALSE);
	//copy all resources into the deployment package

	copycommand = "robocopy " + vsdllsFolder + " " + localDeploymentFolder + " > " + localDeploymentFolder + "PRTDIST_DEPLOYER_ROBO.txt";
	if (system(copycommand.c_str()) == -1)
	{
		cerr << "Failed to Copy VS2013 in " << localDeploymentFolder << endl;
		exit(-1);
	}

	copycommand = "robocopy /XF PRTDIST_DEPLOYER.txt" + allBinaries + " " + localDeploymentFolder + " >> " + localDeploymentFolder + "PRTDIST_DEPLOYER_ROBO.txt";
	if (system(copycommand.c_str()) == -1)
	{
		cerr << "Failed to Copy All Binaries in " << localDeploymentFolder << endl;
		exit(-1);
	}

	copycommand = "robocopy " + pstoolsFolder + " " + localDeploymentFolder + " >> " + localDeploymentFolder + "PRTDIST_DEPLOYER_ROBO.txt";
	if (system(copycommand.c_str()) == -1)
	{
		cerr << "Failed to Copy PsTools in " << localDeploymentFolder << endl;
		exit(-1);
	}

	copycommand = "robocopy " + scriptsFolder + " " + localDeploymentFolder + " >> " + localDeploymentFolder + "PRTDIST_DEPLOYER_ROBO.txt";
	if (system(copycommand.c_str()) == -1)
	{
		cerr << "Failed to Copy PsTools in " << localDeploymentFolder << endl;
		exit(-1);
	}

	copycommand = "robocopy " + prtWinUserDll + " " + localDeploymentFolder + " PrtWinUser.dll >> " + localDeploymentFolder + "PRTDIST_DEPLOYER_ROBO.txt";
	if (system(copycommand.c_str()) == -1)
	{
		cerr << "Failed to Copy PrtWinUser.dll in " << localDeploymentFolder << endl;
		exit(-1);
	}


	SYSTEMTIME time;
	GetLocalTime(&time);
	string jobName = to_string(time.wMonth) + "-" + to_string(time.wDay) + "-" + to_string(time.wYear) + "--" + to_string(time.wHour) + "-" + to_string(time.wMinute) + "-" + to_string(time.wSecond);

	string jobFolder = (remoteNetworkShare + jobName);
	//dump the jobname and folder in job.txt
	ofstream tempout;
	tempout.open(localDeploymentFolder + "job.txt");
	tempout << jobName << endl;
	tempout << jobFolder << endl;
	tempout.close();

	//copy all files

	CreateDirectory(jobFolder.c_str(), NULL);
	copycommand = "robocopy " + localDeploymentFolder + " " + jobFolder + " >> " + localDeploymentFolder + "PRTDIST_DEPLOYER.txt";
	if (system(copycommand.c_str()) == -1)
	{
		cerr << "Failed to Copy " << localDeploymentFolder << " to the destination folder " << jobFolder << endl;
		exit(-1);
	}
	return jobFolder;
}

string PrtDistGetNetworkShare() {
	int i = 0;
	char DM[200];
	XMLNODE** listofNodes;
	XMLNODE* currNode;
	string DeploymentFolder = "";
	strcpy_s(DM, 200, "NetworkShare");
	listofNodes = XMLDOMParseNodes(configurationFile.c_str());
	currNode = listofNodes[i];
	while (currNode != NULL)
	{
		if (strcmp(currNode->NodeName, DM) == 0)
		{
			DeploymentFolder = currNode->NodeValue;
		}
		currNode = listofNodes[i];
		i++;
	}
	PrtDistDeployerLog((char*)("Network Share = " + DeploymentFolder).c_str());
	return DeploymentFolder;
}

int main(int argc, char * argv[])
{
	char* log = (char*)calloc(100, sizeof(char));
	//create the Log File
	PrtDistDeployerCreateLogFile();
	string jobFolder = PrtDDeployPProgram();
	cout << "Deployed the P program at :" << endl << jobFolder << endl;
	PrtDistDeployerCloseLogFile();
	cout << "Press Key to Continue ....";
	getchar();

}

///
///PrtDist Deployer Logging
///
void PrtDistDeployerCreateLogFile()
{
	logFile = fopen(logFileName, "w+");
	fputs("Starting PrtDistDeployment ..... \n", logFile);
	fflush(logFile);
}

void PrtDistDeployerCloseLogFile()
{
	fputs("Done with Deployment ...... \n", logFile);
	fflush(logFile);
	fclose(logFile);
}

void PrtDistDeployerLog(char* log)
{
	fputs(log, logFile);
	fputs("\n", logFile);
	fflush(logFile);
}