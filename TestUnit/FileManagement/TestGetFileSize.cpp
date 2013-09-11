#include "stdafx.h"
#include "TestGetFileSize.h"
#include "FileManagement/ApiGetFileSize.h"
#include "../File.h"
#include "FileManagement/ApiDeleteFile.h"

using namespace std;

void Test_GetFileSize()
{
    DEF_TESTLOG_T("GetFileSize, Get file size a specific file");

    wstring fileName = GetTestFileName(L"Test_GetFileSize");
    if(!MakeFile(log, fileName.c_str()))
    {
        log.GetStream(TestLog::MT_ERROR) << L"파일을 생성할 수 없습니다." << endl;
        return;
    }

    DWORD dwSizeHigh;

    File f(fileName);
    DWORD dwSizeLow = apiGetFileSize(f, &dwSizeHigh);

    if(f.IsValidHandle())
    {
        f.CloseHandle();
    }

    if(dwSizeLow == INVALID_FILE_SIZE)
    {
        log.GetStream(TestLog::MT_ERROR) << L"GetFileSize API가 실패했습니다." << endl;

        BOOL fOk = uDeleteFile(fileName.c_str());

        if(!fOk)
        {
            log.GetStream(TestLog::MT_ERROR) << L"파일을 삭제할 수 없습니다." << endl;
            return;
        }

        return;
    }

    BOOL fOk = uDeleteFile(fileName.c_str());

    if(!fOk)
    {
        log.GetStream(TestLog::MT_ERROR) << L"파일을 삭제할 수 없습니다." << endl;
        return;
    }   

    log.Ok();
}