#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include <string.h>
#include <unistd.h>

#define SERVERIP "hlgf1114.iptime.org"
#define SERVERPORT 9000

#define BOARD_SIZE 5

SOCKET sock;

int board[BOARD_SIZE][BOARD_SIZE]; //������ �迭
int check_number[BOARD_SIZE*BOARD_SIZE+1]={0}; //�ߺ��˻�� �迭
//int socket_fd; //���� ���ϵ�ũ����
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
int recvn(SOCKET s, char *buf, int len, int flags)
{
	int received;
	char *ptr = buf;
	int left = len;

	while(left > 0){
		received = recv(s, ptr, left, flags);
		if(received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if(received == 0)
			break;
		left -= received;
		ptr += received;
	}

	return (len - left);
}



void sockSetting(){
    int retval;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return -1;

	// socket()
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == INVALID_SOCKET) err_quit("socket()");

	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if(retval == SOCKET_ERROR) err_quit("connect()");

}

int value_check(int number) //���� ��ȿ�� �˻�
{
	if(number<1||number>25||check_number[number]==1)
	{
		printf("���� ��ȿ���� �ʽ��ϴ�. �ٽ��Է����ּ��� : ");
		scanf("%d", &number);
		number=value_check(number); //��ȿ�� �Է��Ҷ����� ���ȣ��
	}
	return number;
}

void gameInit()
{
//	int i, j; //ī��Ʈ�� ����
	int recv_len, recv_count;

	recv_len=0;

	while(recv_len!=sizeof(board)) // ��Ŷ�� �߷��� �ü��� �����Ƿ� ����ó���� ���� ���ǹ�
	{
	    recv_count = recv(sock,(char*)&board,sizeof(board),0);
		//recv_count=read(socket_fd, board, sizeof(board));
		//err_display("recv()");
		if(recv_count== -0)
            break;
		printf("%d ����Ʈ: board�� �����Ͽ����ϴ�\n", recv_count);
		recv_len += recv_count;
		//recv_count -= recv_count;
		//printf("recv_len: %d",recv_len);
		//printf("recv_count: %d",recv_count);
	}
	printf("out loop");
	return;
}
void gamePrint(int number, int turn_count)
{
	int i, j;

	system("clear"); //���� ȿ���� ���� ȭ�� �ʱ�ȭ
	printf("%c[1;33m",27); //�͹̳� ���ڻ��� ��������� ����

	printf("@------   Ŭ��   ------@\n");
	printf("���� �ϼ�: %d\n", turn_count);
	printf("+----+----+----+----+----+\n");
	for(i=0; i < BOARD_SIZE; i++)
	{
		for(j=0; j < BOARD_SIZE; j++)
		{
			if(board[i][j]==number)
				board[i][j]=0; //Xǥ ó��
			if(board[i][j]==0)
			{
				printf("| ");
				printf("%c[1;31m",27);
				printf("%2c ", 88);
				printf("%c[1;33m",27);
			}
			else
				printf("| %2d ", board[i][j]);
		}
		printf("|\n");
		printf("+----+----+----+----+----+\n");
	}
	printf("%c[0m", 27); //�͹̳� ���ڻ��� ���������� ����
	if(turn_count!=0)
	{
		printf("����: %d\n", turn[0]);
		printf("�����: %d\n", turn[1]);
	}
}

void server_turn()
{
	int recv_len=0;

	while(recv_len!=sizeof(turn)) // ��Ŷ�� �߷��� �ü��� �����Ƿ� ����ó���� ���� ���ǹ�
	{
		int recv_count;

		recv_count= recv(sock,(char*)&turn,sizeof(turn),0);
		//recv_count=read(socket_fd, turn, sizeof(turn));
		err_display("recv()");
		if(recv_count==0) break;
		printf("%d ����Ʈ: ������ �� ������ �����Ͽ����ϴ�\n", recv_count);
		recv_len+=recv_count;
	}
	check_number[turn[0]]=1;
}
void client_turn(int turn_count)
{
	int array_len, recv_len=0;;

	printf("%d�� ���ڸ� �Է����ּ��� : ", turn_count);
	scanf("%d", &turn[0]);
	turn[0]=value_check(turn[0]);
	check_number[turn[0]]=1;

	//array_len=write(socket_fd, turn, sizeof(turn));
	array_len = send(sock,(char*)&turn,sizeof(turn),0);
	printf("%d ����Ʈ: Ŭ���̾�Ʈ�� �� ������ �����Ͽ����ϴ�\n", array_len);
	err_display("send()");

	while(recv_len!=sizeof(turn))
	{
		int recv_count;

		recv_count=recv(sock,(char*)&turn,sizeof(turn),0);
		//recv_count=read(socket_fd, turn, sizeof(turn));
		err_display("recv()");
		if(recv_count==0) break;
		printf("%d ����Ʈ: ������ �� ������ �����Ͽ����ϴ�\n", recv_count);
		recv_len+=recv_count;
	}
}

int main()
{
    int i;
	//int i, j;

	sockSetting();

	printf("������ �����մϴ�.\n");
	gameInit();
	printf("debug: NO.1");
	gamePrint(0, 0);
    printf("debug: NO.2");
	for(i=1;i<BOARD_SIZE*BOARD_SIZE;i++)
	{
		if(i%2==1)
			client_turn(i);
		else
			server_turn();

		gamePrint(turn[0], i);
		//for(j=0;j<4;j++) printf("turn[%d]=%d\n", j, turn[j]); //������

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
	closesocket(sock);
	//close(socket_fd);
	WSACleanup();

	printf("game over\n");
}
