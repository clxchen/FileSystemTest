#include "stdafx.h"
#include "TestMoveFile.h"
#include "FileManagement/ApiMoveFile.h"
#include "../File.h"
#include "FileManagement/ApiCreateFile.h"
#include "FileManagement/ApiDeleteFile.h"
#include "DirectoryManagement/ApiCreateDirectory.h"
#include "DirectoryManagement/ApiRemoveDirectory.h"

using namespace std;

void Test_MoveFile_Normal()
{
    DEF_TESTLOG_T("Test_MoveFile_Normal, 기본 파일 이동 테스트");

    const wstring pSrcFileName = GetSrcFileName();
    const wstring pDstFileName = GetDstFileName();

    if(!MakeFile(log, pSrcFileName))
    {
        log.GetStream(TestLog::MT_ERROR) << L"파일을 생성할 수 없습니다." << endl;
        return;
    }

    BOOL result = apiMoveFile(pSrcFileName.c_str(), pDstFileName.c_str());

    if(result)
    {
        CheckFileData(log, pDstFileName);
    }

    if(!DeleteFileOrCheck(log, pSrcFileName))
    {
        log.GetStream(TestLog::MT_ERROR) << L"만들어진 파일을 삭제할 수 없습니다." << endl;
    }
    if(!DeleteFileOrCheck(log, pDstFileName))
    {
        log.GetStream(TestLog::MT_ERROR) << L"만들어진 파일을 삭제할 수 없습니다." << endl;
        return;
    }

    log.Ok();
}

void Test_MoveFile_NoExistingSrc()
{
    DEF_TESTLOG_T("Test_MoveFile_NoExistingSrc, 존재하지 않는 파일을 이동 시도");

    const wstring pSrcFileName = GetSrcFileName();
    const wstring pDstFileName = GetDstFileName();

    if(IsFileExist(pSrcFileName))
    {
        if(!uDeleteFile(pSrcFileName.c_str()))
        {
            log.GetStream(TestLog::MT_ERROR) << L"존재하는 파일을 삭제할 수 없습니다." << endl;
            return;
        }
    }

    BOOL result = apiMoveFile(pSrcFileName.c_str(), pDstFileName.c_str());

    if(result)
    {
        CheckFileData(log, pDstFileName);
    }

    if(!DeleteFileOrCheck(log, pSrcFileName))
    {
        log.GetStream(TestLog::MT_ERROR) << L"만들어진 파일을 삭제할 수 없습니다." << endl;
    }
    if(!DeleteFileOrCheck(log, pDstFileName))
    {
        log.GetStream(TestLog::MT_ERROR) << L"만들어진 파일을 삭제할 수 없습니다." << endl;
        return;
    }

    log.Ok();
}

void Test_MoveFile_SharingViolationSrcShareMode(DWORD dwShareMode)
{
    DEF_TESTLOG_T("Test_MoveFile_SharingViolationSrc, 원본파일의 공유 권한 테스트");
    log.GetStream(TestLog::MT_MESSAGE) << L"dwShareMode " << apiCreateFile.GetString_dwShareMode(dwShareMode);

    const wstring pSrcFileName = GetSrcFileName();
    const wstring pDstFileName = GetDstFileName();

    if(!MakeFile(log, pSrcFileName))
    {
        log.GetStream(TestLog::MT_ERROR) << L"파일을 생성할 수 없습니다." << endl;
        return;
    }

    File f(pSrcFileName, GENERIC_READ, dwShareMode, OPEN_EXISTING);

    if(!f.IsValidHandle())
    {
        log.GetStream(TestLog::MT_ERROR) << L"CreateFile" << endl;
        return;
    }

    BOOL result = apiMoveFile(pSrcFileName.c_str(), pDstFileName.c_str());

    f.CloseHandle();
    if(result)
    {
        CheckFileData(log, pDstFileName);
    }

    uDeleteFile(pSrcFileName.c_str());

    if(!DeleteFileOrCheck(log, pSrcFileName))
    {
        log.GetStream(TestLog::MT_ERROR) << L"만들어진 파일을 삭제할 수 없습니다." << endl;
    }
    if(!DeleteFileOrCheck(log, pDstFileName))
    {
        log.GetStream(TestLog::MT_ERROR) << L"만들어진 파일을 삭제할 수 없습니다." << endl;
        return;
    }

    log.Ok();
}

void Test_MoveFile_SharingViolationSrc()
{
    Test_MoveFile_SharingViolationSrcShareMode(0);
    Test_MoveFile_SharingViolationSrcShareMode(FILE_SHARE_READ);
    Test_MoveFile_SharingViolationSrcShareMode(FILE_SHARE_WRITE);
    Test_MoveFile_SharingViolationSrcShareMode(FILE_SHARE_DELETE);
    Test_MoveFile_SharingViolationSrcShareMode(FILE_SHARE_READ | FILE_SHARE_WRITE);
    Test_MoveFile_SharingViolationSrcShareMode(FILE_SHARE_READ | FILE_SHARE_DELETE);
    Test_MoveFile_SharingViolationSrcShareMode(FILE_SHARE_WRITE | FILE_SHARE_DELETE);
    Test_MoveFile_SharingViolationSrcShareMode(FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE);
}

void Test_MoveFile_ExistingDst()
{
    DEF_TESTLOG_T("Test_MoveFile_ExistingDst, 존재하는 목적파일이 있을 경우");

    const wstring pSrcFileName = GetSrcFileName();
    const wstring pDstFileName = GetDstFileName();

    if(!MakeFile(log, pSrcFileName))
    {
        log.GetStream(TestLog::MT_ERROR) << L"파일을 생성할 수 없습니다." << endl;
        return;
    }

    if(!TouchFile(log, pDstFileName))
    {
        log.GetStream(TestLog::MT_ERROR) << L"파일을 생성할 수 없습니다." << endl;
        return;
    }

    BOOL result = apiMoveFile(pSrcFileName.c_str(), pDstFileName.c_str());

    if(result)
    {
        CheckFileData(log, pDstFileName);
    }

    if(!DeleteFileOrCheck(log, pSrcFileName))
    {
        log.GetStream(TestLog::MT_ERROR) << L"만들어진 파일을 삭제할 수 없습니다." << endl;
    }
    if(!DeleteFileOrCheck(log, pDstFileName))
    {
        log.GetStream(TestLog::MT_ERROR) << L"만들어진 파일을 삭제할 수 없습니다." << endl;
        return;
    }

    log.Ok();
}

void Test_MoveFile_ExistingDstSharingViolationShareMode(DWORD dwShareMode)
{
    DEF_TESTLOG_T("Test_MoveFile_ExistingDstSharingViolationShareMode, 목적파일의 공유 권한에 따른 테스트");
    log.GetStream(TestLog::MT_MESSAGE) << L"dwShareMode " << apiCreateFile.GetString_dwShareMode(dwShareMode);


    const wstring pSrcFileName = GetSrcFileName();
    const wstring pDstFileName = GetDstFileName();

    if(!MakeFile(log, pSrcFileName))
    {
        log.GetStream(TestLog::MT_ERROR) << L"파일을 생성할 수 없습니다." << endl;
        return;
    }

    if(!TouchFile(log, pDstFileName))
    {
        log.GetStream(TestLog::MT_ERROR) << L"파일을 생성할 수 없습니다." << endl;
        return;
    }

    File f(pDstFileName, GENERIC_READ, dwShareMode, OPEN_EXISTING);

    if(!f.IsValidHandle())
    {
        log.GetStream(TestLog::MT_ERROR) << L"CreateFile" << endl;
        return;
    }

    BOOL result = apiMoveFile(pSrcFileName.c_str(), pDstFileName.c_str());

    f.CloseHandle();

    if(result)
    {
        CheckFileData(log, pDstFileName);
    }

    if(!DeleteFileOrCheck(log, pSrcFileName))
    {
        log.GetStream(TestLog::MT_ERROR) << L"만들어진 파일을 삭제할 수 없습니다." << endl;
    }
    if(!DeleteFileOrCheck(log, pDstFileName))
    {
        log.GetStream(TestLog::MT_ERROR) << L"만들어진 파일을 삭제할 수 없습니다." << endl;
        return;
    }

    log.Ok();
}

void Test_MoveFile_ExistingDstSharingViolation()
{
    Test_MoveFile_ExistingDstSharingViolationShareMode(0);
    Test_MoveFile_ExistingDstSharingViolationShareMode(FILE_SHARE_READ);
    Test_MoveFile_ExistingDstSharingViolationShareMode(FILE_SHARE_WRITE);
    Test_MoveFile_ExistingDstSharingViolationShareMode(FILE_SHARE_DELETE);
    Test_MoveFile_ExistingDstSharingViolationShareMode(FILE_SHARE_READ | FILE_SHARE_WRITE);
    Test_MoveFile_ExistingDstSharingViolationShareMode(FILE_SHARE_READ | FILE_SHARE_DELETE);
    Test_MoveFile_ExistingDstSharingViolationShareMode(FILE_SHARE_WRITE | FILE_SHARE_DELETE);
    Test_MoveFile_ExistingDstSharingViolationShareMode(FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE);
}

void Test_RenameFile_ShareAll()
{
    Test_RenameFile_Share(FILE_SHARE_READ);
    Test_RenameFile_Share(FILE_SHARE_WRITE);
    Test_RenameFile_Share(FILE_SHARE_DELETE);
    Test_RenameFile_Share(FILE_SHARE_READ | FILE_SHARE_WRITE);
    Test_RenameFile_Share(FILE_SHARE_READ | FILE_SHARE_DELETE);
    Test_RenameFile_Share(FILE_SHARE_WRITE | FILE_SHARE_DELETE);
    Test_RenameFile_Share(FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE);
}

void Test_RenameFile_Share(DWORD dwShareMode)
{
    DEF_TESTLOG_T("Test_RenameFile_Share, 공유 권한에 따른 이름 변경 테스트");
    log.GetStream(TestLog::MT_MESSAGE) << L"dwShareMode " << apiCreateFile.GetString_dwShareMode(dwShareMode);

    wstring fileName = GetTestFileName(L"Test_RenameFile_Share");
    if(!TouchFile(log, fileName))
    {
        log.GetStream(TestLog::MT_ERROR) << L"파일을 생성할 수 없습니다." << endl;
        return;
    }
    
    BOOL fOk = FALSE;
    {
        File f1(fileName, dwShareMode);
        if((HANDLE)f1 == INVALID_HANDLE_VALUE)
        {
            log.GetStream(TestLog::MT_ERROR) << L"파일을 열 수 없습니다." << endl;
            return;
        }

        fOk = apiMoveFile(fileName.c_str(), (L"RE_" + fileName).c_str());
        if(!fOk)
        {
            log.GetStream(TestLog::MT_ERROR) << L"파일 이름을 변경할 수 없습니다." << endl;
        }
    }

    if(!fOk)
    {
        if(!uDeleteFile(fileName.c_str()))
        {
            log.GetStream(TestLog::MT_ERROR) << L"파일을 삭제할 수 없습니다." << endl;
            return;
        }
    }
    else
    {
        if(!uDeleteFile((L"RE_" + fileName).c_str()))
        {
            log.GetStream(TestLog::MT_ERROR) << L"파일을 삭제할 수 없습니다." << endl;
            return;
        }
    }    

    log.Ok();
}