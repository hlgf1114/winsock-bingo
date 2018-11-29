#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#include "getHostName.h"

#define SERVERIP "hlgf1114.iptime.org"
#define SERVERPORT 80
#define BUFSIZE    512

//board
#define BOARD_SIZE 5
//#define BACKLOG 3
int server_board[BOARD_SIZE][BOARD_SIZE]; //���� ������ �迭
int client_board[BOARD_SIZE][BOARD_SIZE]; //Ŭ���̾�Ʈ ������ �迭
int check_number[BOARD_SIZE*BOARD_SIZE+1]={0}; //�ߺ��˻�� �迭

SOCKET listen_sock;
SOCKET client_sock;
int server_fd, client_fd; //���� ���ϵ�ũ����
int turn[4]; //���ø����̼� �������� ����

// ���� �Լ� ���� ��� �� ����
void err_quit(char *msg)
{
      LPVOID lpMsgBuf;
      FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
            NULL, WSAGetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&lpMsgBuf, 0, NULL);
      MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
      LocalFree(lpMsgBuf);
      exit(1);
}

// ���� �Լ� ���� ���
void err_display(char *msg)
{
      LPVOID lpMsgBuf;
      FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
            NULL, WSAGetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&lpMsgBuf, 0, NULL);
      printf("[%s] %s", msg, (char *)lpMsgBuf);
      LocalFree(lpMsgBuf);
}
void sockSetting(){
    int retval;

      // ���� �ʱ�ȭ
      WSADATA wsa;
      if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
            return 1;

      // socket()
      listen_sock = socket(AF_INET, SOCK_STREAM, 0);
      if(listen_sock == INVALID_SOCKET)
            err_quit("socket()");


      // bind()
      SOCKADDR_IN serveraddr;
      ZeroMemory(&serveraddr, sizeof(serveraddr));
      serveraddr.sin_family = AF_INET;
      // get address by host
      serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
      serveraddr.sin_port = htons(SERVERPORT);
      retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
      if(retval == SOCKET_ERROR)
            err_quit("bind()");

      // listen()
      retval = listen(listen_sock, SOMAXCONN);
      if(retval == SOCKET_ERROR)
            err_quit("listen()");

      // ������ ��ſ� ����� ����
      SOCKADDR_IN clientaddr;
      int addrlen;
      char buf[BUFSIZE+1];

            // accept()
            addrlen = sizeof(clientaddr);
            client_sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);
            if(client_sock == INVALID_SOCKET)
            {
                  err_display("accept()");
            }

            // ������ Ŭ���̾�Ʈ ���� ���
            printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
                   inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
}
void serverGameInit(){
    int check_number[BOARD_SIZE*BOARD_SIZE]={0};    //remove same number
    int i,j;
    srand(time(NULL)); //time random

	for(i=0; i < BOARD_SIZE; i++){
		for(j=0; j < BOARD_SIZE; j++){
			while(1){
				int temp = rand()%25; // random 0~24

				if(check_number[temp]==0){//�ߺ����� �˰���
					check_number[temp]=1;
					server_board[i][j]=temp+1;
					break;
				}
			}
		}
	}
}

void clientGameInit(){
    int check_number[BOARD_SIZE*BOARD_SIZE]={0};
	int i, j;
	int array_len;

	srand(time(NULL)+100); //���� �����ǰ� �ٸ��� �ϱ����� 100�� �߰�

	for(i=0; i < BOARD_SIZE; i++)
	{
		for(j=0; j < BOARD_SIZE; j++)
		{
			while(1)
			{
				int temp = rand()%25; //0~24 ���� �߻�

				if(check_number[temp]==0) //�ߺ����� �˰���
				{
					check_number[temp]=1;
					client_board[i][j]=temp+1;
					break;
				}
			}
		}
	}
	array_len = send(client_sock,(char *)&client_board,sizeof(client_board),0);
	//array_len = write(client_fd, client_board, sizeof(client_board));
	printf("%d ����Ʈ�� �����Ͽ����ϴ�\n", array_len);
	err_display("send()");
	//error_check(array_len, "����������");
}

void gamePrint(int turn_count)
{
	int i, j;

	system("clear"); //���� ȿ���� ���� ȭ�� �ʱ�ȭ
	printf("%c[1;33m",27); //�͹̳� ���ڻ��� ��������� ����

	printf("@------   ����   ------@\n");
	printf("���� �ϼ�: %d\n", turn_count);
	printf("+----+----+----+----+----+\n");
	for(i=0; i < BOARD_SIZE; i++)
	{
		for(j=0; j < BOARD_SIZE; j++)
		{
			if(server_board[i][j]==0)
			{
				printf("| ");
				printf("%c[1;31m",27);
				printf("%2c ", 88);
				printf("%c[1;33m",27);
			}
			else
				printf("| %2d ", server_board[i][j]);
		}
		printf("|\n");
		printf("+----+----+----+----+----+\n");
	}
	printf("%c[0m", 27); //�͹̳� ���ڻ��� ���������� ����
}

int bingo_check(int board[][BOARD_SIZE])
{
	int i;
	int count=0;

	for(i=0; i < BOARD_SIZE; i++) //����
	{
		if(board[i][0]==0&&board[i][1]==0&&board[i][2]==0&&board[i][3]==0&&board[i][4]==0) //����
		{
			count++;
		}
		if(board[0][i]==0&&board[1][i]==0&&board[2][i]==0&&board[3][i]==0&&board[4][i]==0) //����
			count++;
	}
	if(board[0][0]==0&&board[1][1]==0&&board[2][2]==0&&board[3][3]==0&&board[4][4]==0)
		count++;
	if(board[0][4]==0&&board[1][3]==0&&board[2][2]==0&&board[3][1]==0&&board[4][0]==0)
		count++;
	return count;
}
void board_X(int board[][BOARD_SIZE], int number)
{
	int i, j;

	for(i=0; i < BOARD_SIZE; i++)
	{
		for(j=0; j < BOARD_SIZE; j++)
		{
			if(board[i][j]==number)
				board[i][j]=0; //Xǥ ó��
		}
	}
}
void server_turn()
{
	int array_len;

	while(1)
	{
		turn[0]=rand()%25+1;
		if(check_number[turn[0]]==0)
		{
			check_number[turn[0]]=1;
			break;
		}
	}
	printf("here\n");
	game_run();

	array_len = send(client_sock,(char*)&turn,sizeof(turn),0);
	//array_len=write(client_fd, turn, sizeof(turn));
	printf("%d ����Ʈ: ������ �� ������ �����Ͽ����ϴ�\n", array_len);
	err_display("send()");
	//error_check(array_len, "����������");
}
void client_turn()
{
	int recv_len=0, array_len;

	while(recv_len!=sizeof(turn)) // ��Ŷ�� �߷��� �ü��� �����Ƿ� ����ó���� ���� ���ǹ�
	{
		int recv_count;

        recv_count = recv(client_sock,(char *)&turn,sizeof(turn),0);
		//recv_count=read(client_fd, turn, sizeof(turn));
		err_display("recv()");
		//error_check(recv_count, "�����ͼ���");
		if(recv_count==0) break;
		printf("%d ����Ʈ: Ŭ���̾�Ʈ�� �� ������ �����Ͽ����ϴ�\n", recv_count);
		recv_len+=recv_count;
	}
	game_run();
	check_number[turn[0]]=1;

	//array_len=write(client_fd, turn, sizeof(turn));
	array_len = send(client_sock,(char *)&turn,sizeof(turn),0);

	printf("%d ����Ʈ: Ŭ���̾�Ʈ�� �� ������ �����Ͽ����ϴ�\n", array_len);
	err_display("send");
//	error_check(array_len, "����������");
}
void game_run()
{
	board_X(server_board, turn[0]);
	board_X(client_board, turn[0]);
	turn[1]=bingo_check(client_board);
	turn[2]=bingo_check(server_board);

	if(turn[1]>=5&&turn[2]>=5)
		turn[3]=3; //���º�
	else if(turn[1]>=5)
		turn[3]=1; //Ŭ���̾�Ʈ �¸�
	else if(turn[2]>=5)
		turn[3]=2; //���� �¸�
}

int main()
{
    int i,j;
    //sock setting
    sockSetting(SERVERPORT);
    printf("success create socket !");
    // server game init
    serverGameInit();
    // client game inot
    clientGameInit();
    // game print
    gamePrint(0);
    // terning
    for(i=1;i<=BOARD_SIZE*BOARD_SIZE;i++)
	{
	    printf("i :%d\n",i);
		if(i%2==1)
			client_turn();
		else
		{
			Sleep(1000); //������ ü���ϱ����� 1���� ������
			server_turn();
		}

		gamePrint(i);
		for(j=0;j<4;j++) printf("turn[%d]=%d\n", j, turn[j]); //������
		if(turn[3]==1)
		{
			printf("Ŭ���̾�Ʈ �¸�\n");
			break;
		}
		else if(turn[3]==2)
		{
			printf("���� �¸�\n");
			break;
		}
		else if(turn[3]==3)
		{
			printf("���º�\n");
			break;
		}
	}

	closesocket(client_sock);
	closesocket(listen_sock);

    WSACleanup();
	printf("game over\n");

    return 0;
}
