// CopyFile.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <stdio.h>
#include <stdlib.h>

#define BUF_SIZE	4096

typedef unsigned long	ulong;
typedef unsigned int	uint;

int ioMessage(ulong lData, uint nCode);
int DoneMessage(ulong lData, uint nCode);

int copyFile(const char* srcFileName, const char* destFileName, int (*pfnMsg)(ulong lData, uint nCode));

int main()
{
	int (*pfnMsg)(ulong lData, uint nCode);
	pfnMsg = ioMessage;	

	copyFile("test.zip", "copied.zip", /*함수를 전달*/pfnMsg);

	pfnMsg = DoneMessage;
	pfnMsg(0, 0);

	return 0;
}


int copyFile(const char* srcFileName, const char* destFileName, int (*pfnMsg)(ulong lData, uint nCode))
{
	// 원본(Source)파일을 특정 사이즈만큼 읽어들이고,
	// 대상(Destination) 파일을 열고 읽어들인만큼 써준다.
	int ret = 1;

	char* buf = NULL;
	size_t readBytes = 0;
	size_t writtenBytes = 0;

	FILE* srcFile = NULL;
	FILE* destFile = NULL;

	errno_t error = 0;

	buf = (char*)malloc(BUF_SIZE);
	if (buf == NULL)
	{
		printf("메모리 할당 실패!!!\n");
		ret = 0;
		goto FREE_AREA;
	}

	error = fopen_s(&srcFile, srcFileName, "r+b");	// 읽기 바이너리 모드로 원본 파일을 연다, 파일이 없으면 실패!!!
	if (error)
	{
		printf("원본 파일을 열 수 없습니다.\n");
		ret = 0;
		goto FREE_AREA;
	}

	//
	error = fopen_s(&destFile, destFileName, "w+b");	// 읽기 바이너리 모드로 원본 파일을 연다, 파일을 항상 새로 만듬
	if (error)
	{
		printf("대상 파일을 열 수 없습니다.\n");
		ret = 0;
		goto FREE_AREA;
	}

	fseek(srcFile, 0, SEEK_SET);
	while (!feof(srcFile))
	{
		readBytes = fread(buf, sizeof(char), BUF_SIZE, srcFile);
		if (readBytes)
		{
			writtenBytes = fwrite(buf, sizeof(char), readBytes, destFile);
			pfnMsg(writtenBytes, 0);	// ioMessage()함수를 포인터로 호출함, CALLBACK (함수의 포인터)
		}
	}

	


FREE_AREA:
	if (buf)
	{
		free(buf);
		buf = NULL;
	}

	if (srcFile)
	{
		fclose(srcFile);
		srcFile = NULL;
	}

	if (destFile)
	{
		fclose(destFile);
		destFile = NULL;
	}

	return ret;
}


int ioMessage(ulong lData, uint nCode)
{
	printf("%u Bytes Copied\n", lData);

	return 1;
}

int DoneMessage(ulong lData, uint nCode)
{
	printf("파일 복사가 완료되었습니다\n");
	return 1;
}