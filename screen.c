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
    INT width = GetSystemMetrics(SM_CXSCREEN);  // 屏幕宽
    INT height = GetSystemMetrics(SM_CYSCREEN);  // 屏幕高
    HDC hdcScreen = GetDC(NULL); // 全屏幕DC
    HDC hdcMemDC = CreateCompatibleDC(hdcScreen); // 创建兼容内存DC
 
    if (!hdcMemDC)
    {
        goto done;
    }
 
    // 通过窗口DC 创建一个兼容位图
    hbmScreen = CreateCompatibleBitmap(hdcScreen, width, height);
 
    if (!hbmScreen)
    {
        goto done;
    }
 
    // 将位图块传送到我们兼容的内存DC中
    SelectObject(hdcMemDC, hbmScreen);
    if (!BitBlt(
                hdcMemDC,    // 目的DC
                0, 0,        // 目的DC的 x,y 坐标
                width, height, // 目的 DC 的宽高
                hdcScreen,   // 来源DC
                0, 0,        // 来源DC的 x,y 坐标
                SRCCOPY))    // 粘贴方式
    {
        goto done;
    }
 
    // 获取位图信息并存放在 bmpScreen 中
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
 
    // 在 32-bit Windows 系统上, GlobalAlloc 和 LocalAlloc 是由 HeapAlloc 封装来的
    // handle 指向进程默认的堆. 所以开销比 HeapAlloc 要大
    hDIB = GlobalAlloc(GHND, dwBmpSize);
    lpbitmap = (char *)GlobalLock(hDIB);
 
    // 获取兼容位图的位并且拷贝结果到一个 lpbitmap 中.
    GetDIBits(
        hdcScreen,  // 设备环境句柄
        hbmScreen,  // 位图句柄
        0,          // 指定检索的第一个扫描线
        (UINT)bmpScreen.bmHeight, // 指定检索的扫描线数
        lpbitmap,   // 指向用来检索位图数据的缓冲区的指针
        (BITMAPINFO *)&bi, // 该结构体保存位图的数据格式
        DIB_RGB_COLORS // 颜色表由红、绿、蓝（RGB）三个直接值构成
    );
 
 
    wsprintf(FilePath, "%s\\%s", dirPath, filename);
 
    // 创建一个文件来保存文件截图
    hFile = CreateFile(
                FilePath,
                GENERIC_WRITE,
                0,
                NULL,
                CREATE_ALWAYS,
                FILE_ATTRIBUTE_NORMAL,
                NULL
            );
 
    // 将 图片头(headers)的大小, 加上位图的大小来获得整个文件的大小
    dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
 
    // 设置 Offset 偏移至位图的位(bitmap bits)实际开始的地方
    bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);
 
    // 文件大小
    bmfHeader.bfSize = dwSizeofDIB;
 
    // 位图的 bfType 必须是字符串 "BM"
    bmfHeader.bfType = 0x4D42; //BM
 
    dwBytesWritten = 0;
    WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
    WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
    WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);
 
    // 解锁堆内存并释放
    GlobalUnlock(hDIB);
    GlobalFree(hDIB);
 
    // 关闭文件句柄
    CloseHandle(hFile);
 
    // 清理资源
done:
    DeleteObject(hbmScreen);
    DeleteObject(hdcMemDC);
    ReleaseDC(NULL, hdcScreen);
 
    return 0;
}