#include<iostream>
#include<fstream>
#include <stdlib.h>
#include<vector>
#include "unp.h"
#include<string.h>
#include<string>
#include<cstring>
#include "config.h"

//#include <arpa/inet.h>

using namespace std;

int create() {

	// creates a socket and returns it

	int s = socket(AF_INET, SOCK_DGRAM, 0);
	return s;
}

void sreset(char * str)		//string reset
		{
	int i = 0;
	while (str[i] != '\0')
		str[i++] = '\0';
}
string setstring(char* s)//extract only meaningful string from char array with size greater than number of words
		{
	string temp = "";
	int k = 0;
	while (s[k] != '\0') {
		temp.append(1, s[k++]);
	}
	return temp;
}
void s2c(string names, char * s)	//string to charcter array
		{
	sreset(s);	//clean the arary
	int m = 0;
	for (int j = 0; j < names.length(); j++) {
		s[m++] = names[j];
	}
}

int connect(sockaddr_in server_addr, int sock) {

	//simulates connection between client and server. To be improved in transfer layer phase
	char buff2[100];
	//sreset(buff2);

	int len = sizeof(server_addr);

	int s1 = sendto(sock, "connect", 7, 0, (struct sockaddr *) &server_addr,
			(socklen_t) len);
	int r = recvfrom(sock, buff2, sizeof(buff2), 0,
			(struct sockaddr *) &server_addr, (socklen_t *) &len);
	string s = setstring(buff2);
	s = s.substr(0, r);
	char *arr = new char[s.size()];
//	sreset(arr);
	s2c(s, arr);
	cout << arr << endl;

	if (strcmp(arr, "1") == 0) {
		cout << "Server Accepted" << endl;
		return 1;

	} else {
		cout << "Server Rejected" << endl;
		return 0;
	}
}

void showFiles() {
	FILE* ls_cmd = popen("ls ", "r");
	char buff[1024];
	size_t n;

	while ((n = fread(buff, 1, sizeof(buff) - 1, ls_cmd)) > 0) {
		buff[n] = '\0';
		printf("%s", buff);
	}
	cout << "\t\t*********************************************\n";
}

class packet {

public:

	char apph[100];
	char transh[100];
	char neth[100];
	char datah[100];

	int packetlength;
	char data[100];
	char fileName[10];

	int packetNmbr;
	int isAcked;	//0 for NAK and 1 for ACk

	packet() {
		packetNmbr = 0;
		isAcked = 0;
		for (int i = 0; i < sizeof(data); i++) {
			data[i] = '\0';
		}
	}
	packet(int l, char d[]) {
		packetlength = l;
		bzero(data, sizeof(data));
		int count = 0;
		while (count < l) {
			data[count] = d[count];
			count++;
		}
		//cout<<"data: "<<data<<endl;
	}

	void setData(char arr[], int size) {
		for (int i = 0; i < size && arr[i] != '\0'; i++) {
			data[i] = arr[i];
		}
	}

	void setapph(char head[], int size) {

		for (int i = 0; i < size; i++) {
			apph[i] = head[i];
		}
	}
	bool checkCode(char check[]) {
		if (apph[2] == check[0] && apph[3] == check[1] && apph[4] == check[2])
			return true;
		else
			return false;
	}
	void setFileName(char fileName[], int size) {
		for (int i = 0; i > i < size && fileName[i] != '\0'; i++)
			this->fileName[i] = fileName[i];
	}
	void clearPacket() {
		packetlength = 0;
		for (int i = 0; i < 100; i++) {
			apph[i] = 0;
			transh[i] = 0;
			neth[i] = 0;
			datah[i] = 0;
			data[i] = 0;
			if (i < 10)
				fileName[i] = 0;
		}
	}
	void setisAck(int a) {
		this->isAcked = a;
	}
};

class application {

public:
	string header;
	vector<char> fileData;
	packet sendPacket;
	packet receivePacket;
	vector<packet> fileDataPacket;
	int port;

	application(int p) {
		//menu here.........
		// set header according to options
//    cout << "Enter port number "; ---------------- commented cuz its gonna be added by config file now
//   cin >> port;

		port = p;

		//make header here
	}
	void fileDataflush() {
		fileDataPacket.clear();
	}

	void write(char *p) {
		ofstream outputFile;
		outputFile.open(p);
		if (outputFile)
			for (int i = 0; i < fileDataPacket.size(); i++) {
				for (int j = 0; j < 10; j++) {
					if (fileDataPacket[i].data[j] != '\0')
						outputFile << fileDataPacket[i].data[j];
				}
			}
		outputFile.close();
		cout << "\nThe file is written\n";
	}
	void read(char * Name) {
		sendPacket.setFileName(Name, sizeof(Name));
		//it read the file  to send it later
		char *arr;
		ifstream myfile;
		myfile.open(Name, ios::in);
		char c;
		while (!myfile.eof()) {
			myfile.get(c);
			fileData.push_back(c);
		}
		myfile.close();

		cout << "File read completely and stored\n";
	}

	void clearPacket() {
		sendPacket.clearPacket();
		receivePacket.clearPacket();
		fileDataPacket.clear();
	}

};

class transport {
public:
	string header;
	struct sockaddr_in server_addr;
	int sock;
	transport(int p, char ip[]) {
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = p;
		server_addr.sin_addr.s_addr = inet_addr(ip);
		memset(&(server_addr.sin_zero), '\0', 8);
		sock = create(); // --- recieving socket
	}
	int establishConnection() {

		if ((connect(server_addr, sock)) == 1) {
			cout << "Connected to Server" << endl;
			return 1;
		} else {
			cout << "Failed to Connect" << endl;
			return 0;
		}
	}

	void sendpkt(packet snd) {
		int len = sizeof(server_addr);
		//sending packet to the server
		int sd = sendto(sock, &snd, sizeof(struct packet), 0,
				(struct sockaddr *) &server_addr, (socklen_t) len);

	}
	packet receivepkt() {
		packet receive;
		int len = sizeof(server_addr);
//now receiving response from the server
		int r = recvfrom(sock, &receive, sizeof(struct packet), 0,
				(struct sockaddr*) &server_addr, (socklen_t*) &len);
		return receive;
	}
	void req_res(packet send) {
		char fileNameReceied[1024];
		bzero(fileNameReceied, 1023);

		int len = sizeof(server_addr);
		//sending packet to the server
		int s = sendto(sock, &send, sizeof(struct packet), 0,
				(struct sockaddr *) &server_addr, (socklen_t) len);
		//now receiving response from the server
		int r = recvfrom(sock, fileNameReceied, sizeof(fileNameReceied), 0,
				(struct sockaddr*) &server_addr, (socklen_t*) &len);

		cout << "\t\t*********The server has the following files*********\n";
		cout << "Printing data\n\n";
		for (int i = 0; i < sizeof(fileNameReceied); i++) {
			if (fileNameReceied[i] != '\0')
				cout << fileNameReceied[i];
		}

		cout << "Printed above data\n\n";
		bzero(fileNameReceied, 1024);

	}
	void sendAck(packet toSendAck, int a) {
		packet sendAck;
		sendAck.packetNmbr = toSendAck.packetNmbr;
		sendAck.isAcked = a;
		sendpkt(sendAck);
	}
	~transport() {
		close(sock);
	}
};

//just for the system pause
void pause_215(bool have_newline) {
	if (have_newline) {
// Ignore the newline after the user's previous input.
		cin.ignore(256, '\n');
	}

// Prompt for the user to press ENTER, then wait for a newline.
	cout << endl << "Press ENTER to start the client." << endl;
	cin.ignore(256, '\n');
}

void fileRead(char* ip, int &serverPort, int &clientPort, int &MSS,
		int &ackDrop, int &ackCorrupt, int &windowSize) {
	ifstream file;
	file.open("configC.txt", ifstream::in);
	if (file) {
		string word;
		while (file >> word) {
			if (word.compare("IP") == 0) {
				file >> ip;
			}

			else if (word.compare("Server_Port") == 0) {
				string input;
				file >> input;

				serverPort = atoi(input.c_str());
			} else if (word.compare("Client_Port") == 0) {
				string input;
				file >> input;

				clientPort = atoi(input.c_str());
			}

			else if (word.compare("Packet_size") == 0) {
				string input;
				file >> input;

				MSS = atoi(input.c_str());
			} else if (word.compare("ACK_drop") == 0) {
				string input;
				file >> input;

				ackDrop = atoi(input.c_str());
			} else if (word.compare("ACK_corrupt") == 0) {
				string input;
				file >> input;

				ackCorrupt = atoi(input.c_str());
			} else if (word.compare("Window_Size") == 0) {
				string input;
				file >> input;

				windowSize = atoi(input.c_str());
			}

		}
	}
	cout << "**********Client CONFIG FILE**********\n";
	cout << "The server ip is ";
	for (int i = 0; i < sizeof(ip); i++) {
		cout << ip[i];
	}
	cout << endl;
	cout << "The server port is " << serverPort << endl;
	cout << "The client port is " << clientPort << endl;
	cout << "The packet size is " << MSS << endl;
	cout << "The ack to drop is " << ackDrop << endl;
	cout << "The ack to corrupt is " << ackCorrupt << endl;
	cout << "**********Client CONFIG FILE**********\n\n";

	pause_215(false);

}

int main() {
//	int tempCount=0;
//	int tempCount1=0;

	int serverPort, clientPort, MSS, ackDrop, ackCorrupt;
	int windowSize; //What will be the size of the window
	string fileName;
	char ip[10];

	//reading config file
	fileRead(ip, serverPort, clientPort, MSS, ackDrop, ackCorrupt, windowSize);

//	std::ifstream file("config.txt");
//	std::string str;
//	int ii = 0;
//	while (std::getline(file, str)) {
//		configuration[ii++] = str;
//	}
	// s2c(configuration[0],ip);
//  sport = std::stoi( configuration[1]);
//  cport = std::stoi( configuration[2]);
//  fileName = configuration[3];
//  psize = std::stoi( configuration[4]);
//  drop = std::stoi( configuration[5]);
//  corrupt = std::stoi( configuration[6]);
//
//	int portNo;
//	cout << "Enter port number ";
//	cin >> portNo;

//	if (trans->establishConnection() == 1) {
	while (1) {
		application * app = new application(serverPort);
		transport * trans = new transport(serverPort, ip);

		app->fileData.clear();
		puts("\n\n\t##################################\n"
				"\t\t1: List File		       \n"
				"\t\t2: Download file                  \n"
				"\t\t3: Upload file                    \n"
				"\t\t4: Exit                   \n"
				"\t##################################\n");
		int option;
		cin >> option;
		app->fileDataPacket.clear();
		switch (option) {
		case 1: {
			//file listing
			char header[] = "0000000000000000000000000000000000000";

			//creating new packet
			app->sendPacket.setapph(header, sizeof(header));
			trans->req_res(app->sendPacket);
			break;
		}

			//file download
		case 2: {
			//file listing
			char header[] = "0000000000000000000000000000000000000";
			//creating new packet
			app->sendPacket.setapph(header, sizeof(header));
			trans->req_res(app->sendPacket);
			//cout << "\t\t*********The server has the following files*********\n";
			char filetoDownload[10];
			bzero(filetoDownload, 9);
			cout << "Enter the name of the file you want to download: ";
			cin >> filetoDownload;
			//assignnign the name of the file to download in the packet
			app->sendPacket.setFileName(filetoDownload, sizeof(filetoDownload));
			//		step 1
			char *arr;
			char header1[] = "0100000000000000000000000000000000000";
			//creating new packet
			app->sendPacket.setapph(header1, sizeof(header1));
			trans->sendpkt(app->sendPacket);

			cout << "FILE_DOWNLOAD_REQUEST send\n";
			//	step 2
			app->receivePacket = trans->receivepkt();

			char check[] = "001";
			cout << "FILE_DOWNLOAD_RESPONSE receive\n";
			if (app->receivePacket.checkCode(check))
				cout << "The server has upload response\n";
			else
				break;
			//	step 3
			char header2[] = "0101000000000000000000000000000000000";
			//creating new packet
			app->sendPacket.setapph(header2, sizeof(header2));
			trans->sendpkt(app->sendPacket);
			cout << "FILE_DOWNLOAD_START send\n";
			//	step4
			app->receivePacket = trans->receivepkt();
			//here it is sending the packet with the size of 10
			int count = 0;
			char check4[] = "011";
			if (app->receivePacket.checkCode(check4)) {
				cout << "Receiving file from the server\n";
				int packetNumberReceived = 1; //it is the number of the packet received
				//go back n
				do {
					vector<packet> frame; // to store the window temporary
					for (int i = 0;i < windowSize&& app->receivePacket.checkCode(check4);i++) {
//						printf("Check= %s \n", check4);
						//the server wil first get the packet then the data
						app->receivePacket = trans->receivepkt();
//						cout<<"Received\n";

						frame.push_back(app->receivePacket);
//						packet newpacket;
//						newpacket.setData(app->receivePacket.data,sizeof(app->receivePacket.data));

//						cout<<"Packets received "<<tempCount<<endl;
//											tempCount++;

					}
					for (int j = 0;j < windowSize && frame[j].checkCode(check4); j++) {
						if (packetNumberReceived % ackCorrupt == 0) {
							trans->sendAck(frame[j], 0);
							cout << "sent\n";
							//cout<<"\t"<<packetNumberReceived<<" is dropped\n";
							cout << "NAK of " << frame[j].packetNmbr
									<< " is sent\n";
//							app->fileDataPacket.push_back(frame[j]);
							//to loop out of the loop
							j=windowSize+1;
						}
						// if the packet is not corrupted then save it
						else {
							trans->sendAck(frame[j], 1);
							cout << "sent\n";

							cout << "ACK of " << frame[j].packetNmbr
									<< " SENT\n";
//						for (int i = 0; i < 10; i++) {
//							cout << newpacket.data[i];
//						}
//						cout << endl;
							app->fileDataPacket.push_back(frame[j]);

							cout<<"The  data was the ";
							for(int i=0;i<10;i++)
							{
								cout<<frame[j].data[i];
							}

							cout<<endl;
						}

						packetNumberReceived++;

//						cout<<"Packets sent "<<tempCount1<<endl;
//											tempCount1++;

					}

				} while (app->receivePacket.checkCode(check4));

				cout << "The total received packets are "
						<< packetNumberReceived << endl;

				cout
						<< "The FILE_DOWNLOAD_COMPLETED response from server is received that the file is sent\n\n\n";
				printf("The name of the file is %s \n",
						app->sendPacket.fileName);
				char path[] = "/home/hamza/DownloadClient/";
				char* filePath = new char[strlen(path)
						+ strlen(app->receivePacket.fileName) + 1];
				strcat(path, app->sendPacket.fileName);
				printf("\nFile we downloaded is in %s\n\n", path);
				//app->write(pathclearPacket);

				fstream out;
				out.open(path, std::ios::out);
				if (out.is_open()) {
					cout << "Writing File" << endl;
					for (int i = 0; i < app->fileDataPacket.size(); i++) {
						for (int j = 0; j < 10; j++) {
							if (app->fileDataPacket[i].data[j] != '\0')
								out << app->fileDataPacket[i].data[j];
						}
					}
					cout << "\nThe file is written\n";
				} else
					cout << "Unable to create file" << endl;
				app->fileDataPacket.clear();
				out.close();
				app->clearPacket();

			}
			break;
		}

//	      *****************************************************************************************************

		case 3: {
			cout
					<< "\n\n\t\t********* Files in the directory are *********\n\n";
			showFiles();
			char fileName[10];
			bzero(fileName, 10);
			cout << "Enter the name of the file which you want to upload: ";
			cin >> fileName;
			app->read(fileName);
//		step 1
			char header1[] = "1000000000000000000000000000000000000";
			//creating new packet
			app->sendPacket.setapph(header1, sizeof(header1));
			trans->sendpkt(app->sendPacket);
			cout << "FILE_UPLOAD_REQUEST send\n";
////	      sending name to the so that the server has a copy of it
//	step 2
			app->receivePacket = trans->receivepkt();
			char check[] = "001";
			cout << "FILE_UPLOAD_RESPONSE receive\n";
			if (app->receivePacket.checkCode(check))
				cout << "The server has upload response\n";
			else
				break;

//	step 3
			char header2[] = "1001000000000000000000000000000000000";
			//creating new packet
			app->sendPacket.setapph(header2, sizeof(header2));
			trans->sendpkt(app->sendPacket);
			cout << "FILE_UPLOAD_START send\n";
//	step4
			app->receivePacket = trans->receivepkt();
			//
			//here it is sending the packet with the size of 10
			int count = 0;
			char check4[] = "011";
			if (app->receivePacket.checkCode(check4)) {
				cout << "Sending file to the client\n";
				while (count < app->fileData.size()) {
					char *arr = new char[10];
					int iter = 0;
					while (iter < 10 && count < app->fileData.capacity()) {
						arr[iter] = app->fileData[count];
						iter++;
						count++;
					}
					//first will send header then it will send data
					char header3[] = "100110000000000000000000000000000000";
					app->sendPacket.setapph(header3, sizeof(header3));
					bzero(app->sendPacket.data, sizeof(app->sendPacket.data));
					app->sendPacket.setData(arr, 10);
					trans->sendpkt(app->sendPacket);
					for (int i = 0; i < 10; i++) {
						cout << app->sendPacket.data[i];
					}
					char clear[] = "";
					app->sendPacket.setData(clear, sizeof(clear));
				}
			}
			cout << "File packets send completely\n";

			char header[] = "101000000000000000000000000000000000";

			app->sendPacket.setapph(header, sizeof(header));
			bzero(app->sendPacket.data, 10);
			trans->sendpkt(app->sendPacket);
			cout << "The FILE_UPLOAD_COMPLETED packet is sent\n";
			//synchronization problem
			trans->sendpkt(app->sendPacket);
			break;
		}
		case 4: {
			exit(0);
			break;
		}
		default:
			break;
		}
		// to check the input is valid
		while (true) {
			cout << "Do you want to perform another function(Y/N)\n";
			char choice;
			cin >> choice;

			if (choice == 'N' || choice == 'n') {
				delete app;
				delete trans;
				exit(0);
			} else if (choice == 'Y' || choice == 'y')
				break;
			else
				cout << "\nInvalid input. ";
		}
	}
//	}

	return 0;

}
