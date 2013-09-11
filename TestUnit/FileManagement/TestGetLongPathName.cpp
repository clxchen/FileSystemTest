#include "stdafx.h"
#include "TestGetLongPathName.h"
#include "FileManagement/ApiGetLongPathName.h"

using namespace std;

void Test_GetLongPathName_DoesntExistFile()
{
    DEF_TESTLOG_T("GetLongPathName, 존재하지 않는 파일에 대해서 쿼리");
    wstring fileName = GetTestFileName();

    DWORD e;
    if(Test_GetLongPathName(log, fileName, e))
    {
        log.Ok();
    }
}

void Test_GetLongPathName_Normal()
{
    DEF_TESTLOG_T("GetLongPathName, ");
    wstring fileName = GetTestFileName();

    if(!TouchFile(log, fileName))
    {
        log.GetStream(TestLog::MT_ERROR) << L"파일을 생성하는데 실패했습니다." << endl;
        return;
    }

    DWORD e;
    if(Test_GetLongPathName(log, fileName, e))
    {
        log.Ok();
    }    
}

BOOL Test_GetLongPathName(TestLog& log, const std::wstring& sfilename, DWORD& lastError)
{
    TCHAR lpszLongPath[MAX_PATH] = { 0 };

    DWORD dw = apiGetLongPathName(sfilename.c_str(), lpszLongPath, _countof(lpszLongPath));
    lastError = GetLastError();

    if(dw == 0)
    {
        log.GetStream(TestLog::MT_ERROR) << L"GetLongPathName Function failed." << endl;
        return FALSE;
    }

    if(!uDeleteFile(sfilename.c_str()))
    {
        log.GetStream(TestLog::MT_ERROR) << L"파일 삭제가 실패했습니다." << endl;
        return FALSE;
    }

    return TRUE;
}