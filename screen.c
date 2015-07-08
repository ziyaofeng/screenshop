#include <windows.h>
#include <stdio.h>
#include <time.h>
int CaptureImage(HWND hWnd, CHAR *dirPath, CHAR *filename);
char* gettime();
int main()
{        
    char msg[2000]="D:\\screen\\";
    char filename[30];
	while(1)
	{
		strcpy(filename,gettime());
		Sleep(10000);
		CaptureImage(GetDesktopWindow(), msg, filename);
		filename[0]=0;
	}
	return 0;
}
char* gettime()
{
	int i=0;
	char temp[30];
	time_t rawtime;
	struct tm * timeinfo;
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	strcpy(temp,asctime(timeinfo));
	temp[19]=0;
	strcat(temp,".gif");
	while(temp[i]!=0)
	{
		if(temp[i]==' ')temp[i]='_';
		if(temp[i]==':')temp[i]='_';
		i++;
	}
	return temp;
}
int CaptureImage(HWND hwnd, CHAR *dirPath, CHAR *filename)
{
    HANDLE hDIB;
    HANDLE hFile;
    DWORD dwBmpSize;
    DWORD dwSizeofDIB;
    DWORD dwBytesWritten;
    CHAR FilePath[MAX_PATH];
    HBITMAP hbmScreen = NULL;
    BITMAP bmpScreen;
    BITMAPFILEHEADER bmfHeader;
    BITMAPINFOHEADER bi;
    CHAR *lpbitmap;
    INT width = GetSystemMetrics(SM_CXSCREEN);  // ��Ļ��
    INT height = GetSystemMetrics(SM_CYSCREEN);  // ��Ļ��
    HDC hdcScreen = GetDC(NULL); // ȫ��ĻDC
    HDC hdcMemDC = CreateCompatibleDC(hdcScreen); // ���������ڴ�DC
 
    if (!hdcMemDC)
    {
        goto done;
    }
 
    // ͨ������DC ����һ������λͼ
    hbmScreen = CreateCompatibleBitmap(hdcScreen, width, height);
 
    if (!hbmScreen)
    {
        goto done;
    }
 
    // ��λͼ�鴫�͵����Ǽ��ݵ��ڴ�DC��
    SelectObject(hdcMemDC, hbmScreen);
    if (!BitBlt(
                hdcMemDC,    // Ŀ��DC
                0, 0,        // Ŀ��DC�� x,y ����
                width, height, // Ŀ�� DC �Ŀ��
                hdcScreen,   // ��ԴDC
                0, 0,        // ��ԴDC�� x,y ����
                SRCCOPY))    // ճ����ʽ
    {
        goto done;
    }
 
    // ��ȡλͼ��Ϣ������� bmpScreen ��
    GetObject(hbmScreen, sizeof(BITMAP), &bmpScreen);
 
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bmpScreen.bmWidth;
    bi.biHeight = bmpScreen.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;
 
    dwBmpSize = ((bmpScreen.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmpScreen.bmHeight;
 
    // �� 32-bit Windows ϵͳ��, GlobalAlloc �� LocalAlloc ���� HeapAlloc ��װ����
    // handle ָ�����Ĭ�ϵĶ�. ���Կ����� HeapAlloc Ҫ��
    hDIB = GlobalAlloc(GHND, dwBmpSize);
    lpbitmap = (char *)GlobalLock(hDIB);
 
    // ��ȡ����λͼ��λ���ҿ��������һ�� lpbitmap ��.
    GetDIBits(
        hdcScreen,  // �豸�������
        hbmScreen,  // λͼ���
        0,          // ָ�������ĵ�һ��ɨ����
        (UINT)bmpScreen.bmHeight, // ָ��������ɨ������
        lpbitmap,   // ָ����������λͼ���ݵĻ�������ָ��
        (BITMAPINFO *)&bi, // �ýṹ�屣��λͼ�����ݸ�ʽ
        DIB_RGB_COLORS // ��ɫ���ɺ졢�̡�����RGB������ֱ��ֵ����
    );
 
 
    wsprintf(FilePath, "%s\\%s", dirPath, filename);
 
    // ����һ���ļ��������ļ���ͼ
    hFile = CreateFile(
                FilePath,
                GENERIC_WRITE,
                0,
                NULL,
                CREATE_ALWAYS,
                FILE_ATTRIBUTE_NORMAL,
                NULL
            );
 
    // �� ͼƬͷ(headers)�Ĵ�С, ����λͼ�Ĵ�С����������ļ��Ĵ�С
    dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
 
    // ���� Offset ƫ����λͼ��λ(bitmap bits)ʵ�ʿ�ʼ�ĵط�
    bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);
 
    // �ļ���С
    bmfHeader.bfSize = dwSizeofDIB;
 
    // λͼ�� bfType �������ַ��� "BM"
    bmfHeader.bfType = 0x4D42; //BM
 
    dwBytesWritten = 0;
    WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
    WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
    WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);
 
    // �������ڴ沢�ͷ�
    GlobalUnlock(hDIB);
    GlobalFree(hDIB);
 
    // �ر��ļ����
    CloseHandle(hFile);
 
    // ������Դ
done:
    DeleteObject(hbmScreen);
    DeleteObject(hdcMemDC);
    ReleaseDC(NULL, hdcScreen);
 
    return 0;
}