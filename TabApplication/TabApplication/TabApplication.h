
// TabApplication.h : TabApplication �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C��
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH �ɑ΂��Ă��̃t�@�C�����C���N���[�h����O�� 'stdafx.h' ���C���N���[�h���Ă�������"
#endif

#include "resource.h"       // ���C�� �V���{��


// CTabApplicationApp:
// ���̃N���X�̎����ɂ��ẮATabApplication.cpp ���Q�Ƃ��Ă��������B
//

class CTabApplicationApp : public CWinApp
{
public:
	CTabApplicationApp();


// �I�[�o�[���C�h
public:
	virtual BOOL InitInstance();

// ����

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CTabApplicationApp theApp;
