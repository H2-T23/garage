
// MFCGUIThread.h : MFCGUIThread �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C��
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH �ɑ΂��Ă��̃t�@�C�����C���N���[�h����O�� 'stdafx.h' ���C���N���[�h���Ă�������"
#endif

#include "resource.h"       // ���C�� �V���{��

class CGUIThread;

// CMFCGUIThreadApp:
// ���̃N���X�̎����ɂ��ẮAMFCGUIThread.cpp ���Q�Ƃ��Ă��������B
//

class CMFCGUIThreadApp : public CWinApp
{
public:
	CMFCGUIThreadApp();

// �I�[�o�[���C�h
public:
	virtual BOOL	InitInstance();
	virtual int		ExitInstance();

// ����
	CGUIThread*		m_pGUIThread;

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CMFCGUIThreadApp theApp;
