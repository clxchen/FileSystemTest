#include "stdafx.h"
#include "TestDeleteFile.h"
#include "FileManagement/ApiDeleteFile.h"
#include "../File.h"
#include "FileManagement/ApiSetFileAttributes.h"

using namespace std;

/**
- 그냥 지우는 경우
- 다른 곳에서 DELETE 권한 비허용으로 파일을 열고 있을 때 삭제 시도
*/
void Test_DeleteFile_Normal()
{
    DEF_TESTLOG_T("Test_DeleteFile_Normal");
    wstring fileName = GetTestFileName();

    bool fOk = MakeFile(log, fileName.c_str());

    if(!fOk)
    {
        log.GetStream(TestLog::MT_ERROR) << L"파일을 생성할 수 없습니다." << endl;
        return;
    }

    BOOL iOk = apiDeleteFile(fileName.c_str());

    if(!iOk)
    {
        if(!DeleteFileOrCheck(log, fileName.c_str()))
        {
            log.GetStream(TestLog::MT_ERROR) << L"만들어진 파일을 삭제할 수 없습니다." << endl;
            return;
        }
    }

    log.Ok();
}

// 읽기 전용 파일을 삭제
void Test_DeleteFile_ReadOnly()
{
    DEF_TESTLOG_T("Test_DeleteFile_ReadOnly");
    wstring fileName = GetTestFileName();

    bool fOk = MakeFile(log, fileName.c_str());

    if(!fOk)
    {
        log.GetStream(TestLog::MT_ERROR) << L"파일을 생성할 수 없습니다." << endl;
        return;
    }

    apiSetFileAttributes(fileName.c_str(), FILE_ATTRIBUTE_READONLY);
    BOOL iOk = apiDeleteFile(fileName.c_str());

    if(!iOk)
    {
        if(!DeleteFileOrCheck(log, fileName.c_str()))
        {
            log.GetStream(TestLog::MT_ERROR) << L"읽기 전용 파일을 삭제할 수 없습니다. -삭제할 수 있어야 한다." << endl;
            return;
        }
    }

    log.Ok();
}

void Test_DeleteFile_OpenFile()
{
    DEF_TESTLOG_T("Test_DeleteFile_OpenFile");
    wstring fileName = GetTestFileName(L"Test_DeleteFile_OpenFile");

    bool fOk = MakeFile(log, fileName.c_str());

    if(!fOk)
    {
        log.GetStream(TestLog::MT_ERROR) << L"파일을 생성할 수 없습니다." << endl;
        return;
    }

    BOOL iOk = FALSE;
    {
        File f(fileName, GENERIC_READ, FILE_SHARE_DELETE, OPEN_EXISTING);

        if(!f.IsValidHandle())
        {
            log.GetStream(TestLog::MT_ERROR) << L"CreateFile" << endl;
            if(!DeleteFileOrCheck(log, fileName.c_str()))
            {
                log.GetStream(TestLog::MT_ERROR) << L"만들어진 파일을 삭제할 수 없습니다." << endl;
            }
            return;
        }

        iOk = apiDeleteFile(fileName.c_str());

        BOOL result = f.CloseHandle();
        if(!result)
        {
            // 거의 발생하지 않는다.
            log.GetStream(TestLog::MT_ERROR) << L"CloseHandle" << endl;
        }
    }

    if(!iOk)
    {
        if(!DeleteFileOrCheck(log, fileName.c_str()))
        {
            log.GetStream(TestLog::MT_ERROR) << L"만들어진 파일을 삭제할 수 없습니다." << endl;
            return;
        }
    }

    log.Ok();
}

void Test_DeleteFile_SharingViolation()
{
    DEF_TESTLOG_T("Test_DeleteFile_SharingViolation");
    wstring fileName = GetTestFileName();

    bool fOk = MakeFile(log, fileName.c_str());

    if(!fOk)
    {
        log.GetStream(TestLog::MT_ERROR) << L"파일을 생성할 수 없습니다." << endl;
        return;
    }

    BOOL iOk = FALSE;
    {
        File f(fileName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, OPEN_EXISTING);

        if(!f.IsValidHandle())
        {
            log.GetStream(TestLog::MT_ERROR) << L"CreateFile" << endl;
            if(!DeleteFileOrCheck(log, fileName.c_str()))
            {
                log.GetStream(TestLog::MT_ERROR) << L"만들어진 파일을 삭제할 수 없습니다." << endl;
            }
            return;
        }

        iOk = apiDeleteFile(fileName.c_str());

        BOOL result = f.CloseHandle();
        if(!result)
        {
            // 거의 발생하지 않는다.
            log.GetStream(TestLog::MT_ERROR) << L"CloseHandle" << endl;
        }
    }

    if(!iOk)
    {
        if(!DeleteFileOrCheck(log, fileName.c_str()))
        {
            log.GetStream(TestLog::MT_ERROR) << L"만들어진 파일을 삭제할 수 없습니다." << endl;
            return;
        }
    }

    log.Ok();
}

void Test_DeleteFile_NoExist()
{
    DEF_TESTLOG_T("Test_DeleteFile_NoExist");
    wstring fileName = GetTestFileName();

    BOOL iOk = apiDeleteFile(fileName.c_str());

    if(!iOk)
    {
        if(!DeleteFileOrCheck(log, fileName.c_str()))
        {
            log.GetStream(TestLog::MT_ERROR) << L"만들어진 파일을 삭제할 수 없습니다." << endl;
            return;
        }
    }

    log.Ok();
}

void Test_DeleteFile_DeletedFile()
{
    DEF_TESTLOG_T("Test_DeleteFile_DeletedFile");
    wstring fileName = GetTestFileName();

    bool fOk = MakeFile(log, fileName.c_str());

    if(!fOk)
    {
        log.GetStream(TestLog::MT_ERROR) << L"파일을 생성할 수 없습니다." << endl;
        return;
    }

    BOOL iOk = FALSE;
    {
        File f(fileName, FILE_GENERIC_READ | FILE_GENERIC_WRITE, FILE_SHARE_DELETE, OPEN_EXISTING);

        if(!f.IsValidHandle())
        {
            log.GetStream(TestLog::MT_ERROR) << L"CreateFile, LastError = " <<
                GetLastError() << L"(" << GetErrorDefineString(GetLastError()) << L")" << endl;
            if(!DeleteFileOrCheck(log, fileName.c_str()))
            {
                log.GetStream(TestLog::MT_ERROR) << L"만들어진 파일을 삭제할 수 없습니다." << endl;
            }
            return;
        }

        DeleteFileW(fileName.c_str());
        //// delete pending

        iOk = apiDeleteFile(fileName.c_str());

        // 내부적으로 DELETE_PENDING 결과적으로 ERROR_ACCESS_DENIED 발생
        BOOL result = f.CloseHandle();
        if(!result)
        {
            // 거의 발생하지 않는다.
            log.GetStream(TestLog::MT_ERROR) << L"CloseHandle" << endl;
        }
    }

    if(!iOk)
    {
        if(!DeleteFileOrCheck(log, fileName.c_str()))
        {
            log.GetStream(TestLog::MT_ERROR) << L"만들어진 파일을 삭제할 수 없습니다." << endl;
            return;
        }
    }

    log.Ok();
}

void Test_DeleteFile_DeletedFileSharingViolation()
{
    DEF_TESTLOG_T("Test_DeleteFile_DeletedFileSharingViolation");
    wstring fileName = GetTestFileName();

    bool fOk = MakeFile(log, fileName.c_str());

    if(!fOk)
    {
        log.GetStream(TestLog::MT_ERROR) << L"파일을 생성할 수 없습니다." << endl;
        return;
    }

    BOOL iOk = FALSE;
    {
        File f(fileName, FILE_GENERIC_READ | FILE_GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, OPEN_EXISTING);

        if(!f.IsValidHandle())
        {
            log.GetStream(TestLog::MT_ERROR) << L"CreateFile, LastError = " <<
                GetLastError() << L"(" << GetErrorDefineString(GetLastError()) << L")" << endl;
            if(!DeleteFileOrCheck(log, fileName.c_str()))
            {
                log.GetStream(TestLog::MT_ERROR) << L"만들어진 파일을 삭제할 수 없습니다." << endl;
            }
            return;
        }

        DeleteFileW(fileName.c_str());
        //// delete pending

        iOk = apiDeleteFile(fileName.c_str());

        // 내부적으로 DELETE_PENDING 결과적으로 ERROR_ACCESS_DENIED 발생
        // ERROR_SHARING_VIOLATION 에러가 발생하지 않는다.
        // 즉, 파일이 지워졌다는 것이 먼저 처리된다.
        BOOL result = f.CloseHandle();
        if(!result)
        {
            // 거의 발생하지 않는다.
            log.GetStream(TestLog::MT_ERROR) << L"CloseHandle" << endl;
        }
    }

    if(!iOk)
    {
        if(!DeleteFileOrCheck(log, fileName.c_str()))
        {
            log.GetStream(TestLog::MT_ERROR) << L"만들어진 파일을 삭제할 수 없습니다." << endl;
            return;
        }
    }

    log.Ok();
}