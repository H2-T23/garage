#include "Log.h"

/**********************************************************************************
 *
 *
 *
 */
UINT AFX_CDECL		CLogManager::LogReceiver( LPVOID lpParam )
{
	CLogManager*	pLogManager	= (CLogManager*)lpParam;
	if( pLogManager == NULL )
	{
		return 0;
	}

	TRACE(_T("BeginThread.\n"));
	DWORD	dwExit = 0;
	while( (dwExit = WaitForSingleObject(pLogManager->m_pEventExit->m_hObject, 0)) == WAIT_OBJECT_0 )
	{
		pLogManager->GetLogLlist()->Append( _T("Subject") );
		Sleep( 100 );

		if( WaitForSingleObject( pLogManager->m_pEventSuspend->m_hObject, 0 ) != WAIT_OBJECT_0 )
		{
			TRACE(_T("SuspendThread.\n"));
			WaitForSingleObject( pLogManager->m_pEventSuspend->m_hObject, INFINITE );
			TRACE(_T("ResumeThread.\n"));
		}
	}

	TRACE(_T("ExitThread.\n"));
	::AfxEndThread( 1 );
	return 1;
}
/**********************************************************************************
 *
 *
 *
 */
/*
1. WSAStartup��WinSock�̏��������s���B

2. socket�֐��Ń\�P�b�g�̍쐬���s���B

3. �T�[�o��IP�A�h���X���擾����Binet_addr�֐���IP�A�h���X�̎擾���ł��Ȃ������ꍇ�A
	WSAAsyncGetHostByName�֐��ŃT�[�o��IP�A�h���X�̎擾����B
	���̂Ƃ��T�[�o�̏�񂪎擾�ł�����E�B���h�E���b�Z�[�W�Ƃ��Ēʒm����܂��B

4. WSAAsyncSelect�֐��Ń\�P�b�g�C�x���g���E�B���h�E���b�Z�[�W�ɒʒm�����悤�ɂ��܂��B

5. �쐬���ꂽ�\�P�b�g���g�p���āA�T�[�o��IP�A�h���X�ƃ|�[�g�ԍ����w�肵��connect�֐��ŃT�[�o�ɐڑ�����B

6. ���M�o�b�t�@�ɋ󂫂��ł������Ƃ�ʒm����E�B���h�E���b�Z�[�W��������A
	�\�P�b�g�ɑ΂��āAsend�֐��Őڑ����Ă���T�[�o�Ƀf�[�^�𑗐M����B

7. �T�[�o����
 */