#include<iostream>
#include<vector>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include <stdio.h>
#include <time.h>
#include<errno.h>
#include<string.h>
#include<cstring>
#include <fstream>
#include "unp.h"
#include<string>
#include "config.h"

using namespace std;

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
    while (s[k] != '\0')
	{
	temp.append(1, s[k++]);
	}
    return temp;
    }
void s2c(string names, char * s)	//string to charcter array
    {
    sreset(s);	//clean the arary
    int m = 0;
    for (int j = 0; j < names.length(); j++)
	{
	s[m++] = names[j];
	}
    }

class packet
    {

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

    packet()
	{
	packetNmbr = 0;
	isAcked = 0;
	for (int i = 0; i < sizeof(data); i++)
	    {
	    data[i] = '0';
	    }
	}
    packet(int l, char d[])
	{
	packetlength = l;
	int count = 0;
	while (count < l)
	    {
	    data[count] = d[count];
	    count++;
	    }
	//cout<<"data: "<<data<<endl;
	}

    void setData(char arr[], int size)
	{
	for (int i = 0; i < size; i++)
	    {
	    data[i] = arr[i];
	    }
	}

    void setapph(char head[], int size)
	{

	for (int i = 0; i < size; i++)
	    {
	    apph[i] = head[i];
	    }
	}
    bool checkCode(char check[])
	{
	if (apph[2] == check[0] && apph[3] == check[1] && apph[4] == check[2])
	    return true;

	else
	    return false;
	}
    bool checkType(char check[])
	{
	if (apph[0] == check[0] && apph[1] == check[1])
	    return true;
	else
	    return false;
	}

    };

//just for the system pause
void pause_215(bool have_newline)
    {
    if (have_newline)
	{
// Ignore the newline after the user's previous input.
	cin.ignore(256, '\n');
	}

// Prompt for the user to press ENTER, then wait for a newline.
    cout << endl << "Press ENTER to start the server." << endl;
    cin.ignore(256, '\n');
    }

void fileRead(char* ip, int &serverPort, int &MSS, int &ackDrop,
	int &ackCorrupt, int &windowSize)
    {
    ifstream file;
    file.open("configC.txt", ifstream::in);
    if (file)
	{
	string word;
	while (file >> word)
	    {
	    if (word.compare("IP") == 0)
		{
		file >> ip;
		}

	    else if (word.compare("Server_Port") == 0)
		{
		string input;
		file >> input;

		serverPort = atoi(input.c_str());
		}

	    else if (word.compare("Packet_size") == 0)
		{
		string input;
		file >> input;

		MSS = atoi(input.c_str());
		}
	    else if (word.compare("ACK_drop") == 0)
		{
		string input;
		file >> input;

		ackDrop = atoi(input.c_str());
		}
	    else if (word.compare("ACK_corrupt") == 0)
		{
		string input;
		file >> input;

		ackCorrupt = atoi(input.c_str());
		}
	    else if (word.compare("Window_Size") == 0)
		{
		string input;
		file >> input;

		windowSize = atoi(input.c_str());
		}

	    }
	}
    cout << "**********Server CONFIG FILE**********\n";
    cout << "The server ip is ";
    for (int i = 0; i < sizeof(ip); i++)
	{
	cout << ip[i];
	}
    cout << endl;
    cout << "The server port is " << serverPort << endl;
    cout << "The packet size is " << MSS << endl;
    cout << "The ack to drop is " << ackDrop << endl;
    cout << "The ack to corrupt is " << ackCorrupt << endl;
    cout << "**********Server CONFIG FILE**********\n\n";

    pause_215(false);

    }

int main()
    {
    char ip[10];
    int serverPort, MSS, ackDrop, ackCorrupt;
    int windowSize; //What will be the size of the window

    //reading config file
    fileRead(ip, serverPort, MSS, ackDrop, ackCorrupt, windowSize);

    vector<packet> fileDataPackets;
    vector<char> fileReaddata;
    vector<packet> packetsUpload;
    char msg2[10];
    sreset(msg2);
    int conn_sock, n, err;
    struct sockaddr_in server_addr, client_addr;

    conn_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (conn_sock == -1)
	{
	cout << "Error ";
	exit(0);
	}

//	int portNo;
//	cout << "Enter port number ";
//	cin >> portNo;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = serverPort;
    server_addr.sin_addr.s_addr = inet_addr(ip);

    err = bind(conn_sock, (struct sockaddr *) &server_addr,
	    sizeof(server_addr));
    if (err == -1)
	{
	cout << "Error" << endl;
	}
    //struct sockaddr_in client_addr;
    int len = sizeof(client_addr);

    cout << "Waiting for Client Requests" << endl;
    int r, s;
//	int r = recvfrom(conn_sock, msg2, sizeof(msg2), 0,
//			(struct sockaddr*) &client_addr, (socklen_t*) &len);
//	string s = setstring(msg2);
//	char *arr = new char[s.size()];
//	sreset(arr);
//	s2c(s, arr);
//	cout << "Client Msg: " << arr << endl;
//	if (strcmp(arr, "connect") == 0) {
//		cout << "Client Accepted" << endl;
////		char *data,tmp[200];
//		int s = sendto(conn_sock, "1", 1, 0, (struct sockaddr*) &client_addr,
//				(socklen_t) len);
//	} else {
//		cout << "Client Rejected" << endl;
//		int s = sendto(conn_sock, "0", 1, 0, (struct sockaddr*) &client_addr,
//				(socklen_t) len);
//	}
//	cout << "Reply Message sent" << endl;
    int tempCount = 0;
    int tempCount1 = 0;

    char pckt[37];
    string header;
    char packetHeader[37];
    strcpy(packetHeader, header.c_str());

    //from here the download and upload starts
    while (true)
	{
	packet receivePacket;
	r = recvfrom(conn_sock, &receivePacket, sizeof(struct packet), 0,
		(struct sockaddr*) &client_addr, (socklen_t*) &len);
//      cout << "Received the header from the server\n";
//      cout << "Header= ";
//      for (int i = 0; i < 37; i++)
//	{
//	  cout << receivePacket.apph[i];
//	}
//      cout << endl;

	//file download from client
	char checkA[] = "00";	//list
	char checkB[] = "01";	//download
	char checkC[] = "10";	//upload

	//file listing
	if (receivePacket.checkType(checkA))
	    {
	    cout << "List mode\n";
	    printf("Header of the list= %s\n", receivePacket.apph);
	    //here it received the header with the request code 000
//	  recvfrom (conn_sock, &receivePacket, sizeof(struct packet), 0,(struct sockaddr*) &client_addr, (socklen_t*) &len);

	    char check[] = "000";
	    if (receivePacket.checkCode(check))
		{
		cout << "Received the FILE_LIST_REQUEST from the client\n";
		FILE* ls_terminal = popen("ls ", "r");
		char fileNames[1024];
		bzero(fileNames, 1024);
		size_t n;

		while ((n = fread(fileNames, 1, sizeof(fileNames) - 1,
			ls_terminal)) > 0)
		    {
		    fileNames[n] = '\0';
		    // printf ("%s", fileNames);
		    }
		for (int i = 0; i < sizeof(fileNames); i++)
		    {
		    cout << fileNames[i];
		    }
		cout << endl;

		sendto(conn_sock, fileNames, sizeof(fileNames), 0,
			(struct sockaddr*) &client_addr, (socklen_t) len);
		cout
			<< "FILE_LIST_REQUEST of the server with directory is sent\n";
		}
	    }

	// **************************************************************************************************

	//file upload to client
	if (receivePacket.checkType(checkB))
	    {
	    cout << "The FILE_DOWNLOAD_REQUEST is found\n";

//	      char fileToDownload[10];
////	      //receiving file name
//	      r = recvfrom (conn_sock, fileToDownload, sizeof(fileToDownload), 0,(struct sockaddr*) &client_addr, (socklen_t*) &len);
////
//	      printf("\ncFile we have to download is %s\n\n",fileToDownload);
	    //means the client wants to send the file
	    char check1[] = "000";
	    char check2[] = "010";
	    char check3[] = "011";
	    char check4[] = "100";
	    if (receivePacket.checkCode(check1))
		{
		packet sendPacket;
		char header[] = "1000100000000000000000000000000000000";
		sendPacket.setapph(header, sizeof(header));
		//the serverpacket is sending the response
		s = sendto(conn_sock, &sendPacket, sizeof(struct packet), 0,
			(struct sockaddr*) &client_addr, (socklen_t) len);
		cout << "The FILE_DOWNLOAD_RESPONSE is sent to the client\n";

		char *arr;
		ifstream myfile;
		myfile.open(receivePacket.fileName, ios::in);
		char c;
		while (!myfile.eof())
		    {
		    myfile.get(c);
		    fileReaddata.push_back(c);
		    }
		myfile.close();

		cout << "File read completely and stored\n";
		}
	    else if (receivePacket.checkCode(check2))
		{
		cout << "The FILE_DOWNLOAD_START signal received\n";

//	  sending the transfer header
		char header2[] = "1001100000000000000000000000000000000";
		//the serverpacket is sending the response
		packet sendPacket;
		sendPacket.setapph(header2, sizeof(header));
		s = sendto(conn_sock, &sendPacket, sizeof(struct packet), 0,
			(struct sockaddr*) &client_addr, (socklen_t) len);
		cout << "The FILE_UPLOAD_TRANSFER is sent to the client\n";

//	      char *datca, tmp[200];
		//means receive untill the client is sending
		cout << "Entering \n";
		//the sequence number of the packets
		srand(time(NULL));
		int pktNumber = rand() % 100;
		int count = 0;
		int packetCount = 0;

//		cout<<"the packet size is "<<packetsUpload.size()<<endl;
//		packetsUpload.clear();
//		cout<<"the packet size after is "<<packetsUpload.size()<<endl;
		// go back n
		while (count < fileReaddata.size())
		    {

		    packet temp;
		    char* arr;
		    arr = new char[10];
		    int iter = 0;
		    while (iter < 10 && count < fileReaddata.capacity())
			{
			arr[iter] = fileReaddata[count];
			iter++;
			count++;
			}
		    //first will send header then it will send data
		    char header3[] = "100110000000000000000000000000000000";

		    bzero(temp.apph, 36);
		    temp.setapph(header3, sizeof(header3));
		    temp.packetNmbr = pktNumber;
		    bzero(temp.data, sizeof(temp.data));
		    temp.setData(arr, 10);

		    packetsUpload.push_back(temp);
		    packetCount++;
		    pktNumber++;
		    }

//				cout << "The packets numbers are \n";
//				for (int i = 0; i < packetsUpload.size(); i++) {
//					cout << packetsUpload[i].packetNmbr << endl;
//				}

		int windowHead = 0;
//		int windowHead = 0 ;

		while (windowHead < packetCount - 1)
		    {
		    cout << "Entered\n";
		    //sending the frame
		    for (int i = 0; i < windowSize && windowHead < packetCount; i++)
			{
			cout<<"Window head = "<<windowHead<<endl;

			cout << "   Packet number = "
				<< packetsUpload[windowHead+i].packetNmbr << endl;
//						cout<<"Testing packet number "<<packetsUpload[windowHead].packetNmbr<<endl;
			int s2 = sendto(conn_sock, &packetsUpload[windowHead+i],
				sizeof(struct packet), 0,
				(struct sockaddr*) &client_addr,
				(socklen_t) len);
//						cout<<"Packets sent "<<tempCount<<endl;
//											tempCount++;

			}
//			windowHead+=windowSize;


		    int checkP = windowHead - windowSize; // a varibale to match the packetNumber or sequence
		    for (int j = 0; j < windowSize && windowHead < packetCount; j++)
			{

			packet ackPacket;
			int s2 = recvfrom(conn_sock, &ackPacket,
				sizeof(struct packet), 0,
				(struct sockaddr*) &client_addr,
				(socklen_t*) &len);

//					cout<<"\t\t"<<ackPacket.isAcked<<endl;
			if (ackPacket.isAcked == 1
				&& ackPacket.packetNmbr
					== packetsUpload[windowHead].packetNmbr)
			    {
			    cout << "The ACK of the packet number "<< ackPacket.packetNmbr << " is received\n";
				windowHead++;

			    }
			else
			    {
			    cout << "The NAK of the packet number "
				    << ackPacket.packetNmbr << " is received\n";

			    j=windowSize+1;
			    }

//					for (int i = 0; i < 10; i++) {
//						cout << sendPacket.data[i];
//					}
//						char clear[] = "";
//						sendPacket.setData(clear, sizeof(clear));


//						cout<<"Packets received "<<tempCount1<<endl;
//																	tempCount1++;
			}

		    }

		cout << "File packets send completely\n";

		//after sending clearing the packets
		fileReaddata.clear();




		char header[] = "011000000000000000000000000000000000";

		sendPacket.setapph(header, sizeof(header));
		bzero(sendPacket.data, 10);

		int s2 = sendto(conn_sock, &sendPacket, sizeof(struct packet),
			0, (struct sockaddr *) &client_addr, (socklen_t) len);
		cout << "The FILE_DOWNLOAD_COMPLETED packet is sent\n";

		//synchronization problem
		sendto(conn_sock, &sendPacket, sizeof(struct packet), 0,
			(struct sockaddr *) &client_addr, (socklen_t) len);

//	      cout << "It is out from the while loop\n";
		}
		// to clear the packets stored to sent
	    packetsUpload.clear();
	    }

	// **************************************************************************************************

	//file downloading(upload from client)
	if (receivePacket.checkType(checkC))
	    {
	    cout << "The FILE_UPLOAD_REQUEST is found\n";

//	      char fileToDownload[10];
////	      //receiving file name
//	      r = recvfrom (conn_sock, fileToDownload, sizeof(fileToDownload), 0,(struct sockaddr*) &client_addr, (socklen_t*) &len);
////
//	      printf("\ncFile we have to download is %s\n\n",fileToDownload);
	    //means the client wants to send the file
	    char check1[] = "000";
	    char check2[] = "010";
	    char check3[] = "011";
	    char check4[] = "100";
	    if (receivePacket.checkCode(check1))
		{
		packet sendPacket;
		char header[] = "0100100000000000000000000000000000000";
		sendPacket.setapph(header, sizeof(header));
		//the serverpacket is sending the response
		s = sendto(conn_sock, &sendPacket, sizeof(struct packet), 0,
			(struct sockaddr*) &client_addr, (socklen_t) len);
		cout << "The FILE_UPLOAD_RESPONSE is sent to the client\n";
		}
	    else if (receivePacket.checkCode(check2))
		{
		cout << "The FILE_UPLOAD_START signal received\n";

//	  sending the transfer header
		char header2[] = "0101100000000000000000000000000000000";
		//the serverpacket is sending the response
		packet sendPacket;
		sendPacket.setapph(header2, sizeof(header));
		s = sendto(conn_sock, &sendPacket, sizeof(struct packet), 0,
			(struct sockaddr*) &client_addr, (socklen_t) len);
		cout << "The FILE_UPLOAD_TRANSFER is sent to the client\n";

//	      char *datca, tmp[200];
		//means receive untill the client is sending
		cout << "Entering \n";
		do
		    {

		    //the server wil first get the packet then the data
		    int t = recvfrom(conn_sock, &receivePacket,
			    sizeof(struct packet), 0,
			    (struct sockaddr*) &client_addr, (socklen_t*) &len);

		    packet newpacket;
		    newpacket.setData(receivePacket.data,
			    sizeof(receivePacket.data));
		    for (int i = 0; i < 10; i++)
			{
			cout << newpacket.data[i];
			}
		    cout << endl;

		    printf("Header= %s\n", newpacket.apph);
		    fileDataPackets.push_back(newpacket);

		    }
		while (receivePacket.checkCode(check3));
		cout << "It is out from the while loop\n";
		}
//  //File download transfer
	    else if (receivePacket.checkCode(check4))
		{
		cout
			<< "The FILE_UPLOAD_COMPLETED response from client is received that the file is sent\n\n\n";
//		  cout << "The data in the file will be\n";

//		  	      printf("\nFile we downloaded is %s\n\n",receivePacket.fileName);

		char path[] = "/home/hamza/DownloadServer/";
		cout << "The file name is ";
		for (int i = 0; i < sizeof(receivePacket.fileName); i++)
		    cout << receivePacket.fileName[i];
		cout << endl;
		char* filePath = new char[strlen(path)
			+ strlen(receivePacket.fileName)];
		strcat(path, receivePacket.fileName);

		printf("\nFile we downloaded is %s\n\n", path);

		fstream out;
		out.open(path, std::ios::out);
		if (out.is_open())
		    {
		    cout << "Writing File" << endl;
		    for (int i = 0; i < fileDataPackets.size(); i++)
			{
			for (int j = 0; j < 10; j++)
			    {
			    if (fileDataPackets[i].data[j] != '\0')
				out << fileDataPackets[i].data[j];
			    }

			}
		    fileDataPackets.clear(); //clearing the packets
		    out.close();
		    cout << "\nThe file is written\n";
		    }
		else
		    cout << "Unable to create file" << endl;
		}
	    }

	}

    exit(0);

    }
