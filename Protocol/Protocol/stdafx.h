// stdafx.h : �W���̃V�X�e�� �C���N���[�h �t�@�C���̃C���N���[�h �t�@�C���A�܂���
// �Q�Ɖ񐔂������A�����܂�ύX����Ȃ��A�v���W�F�N�g��p�̃C���N���[�h �t�@�C��
// ���L�q���܂��B
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Windows �w�b�_�[����g�p����Ă��Ȃ����������O���܂��B
// Windows �w�b�_�[ �t�@�C��:
#include <windows.h>

// C �����^�C�� �w�b�_�[ �t�@�C��
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <time.h>

#include <sstream>
#include <iostream>

#include <vector>
#include <map>
#include <string>

#include <winsock2.h>
#include <mswsock.h>

#pragma comment(lib,"Ws2_32.lib")

// TODO: �v���O�����ɕK�v�Ȓǉ��w�b�_�[�������ŎQ�Ƃ��Ă��������B
/**************************************************************************
 */
class CTrace {
public:
	static void	Trace( const char* fmt, ... )
	{
		CHAR	buf[256];
		va_list	args;
		va_start(args, fmt);
		if( _vsnprintf(buf,_countof(buf),fmt,args) > 0 ){
			OutputDebugStringA( buf );
		}
		va_end(args);
	}

	static void	Trace( const WCHAR* fmt, ... )
	{
		WCHAR	buf[256];
		va_list	args;
		va_start(args, fmt);
		if( _vsnwprintf(buf,_countof(buf),fmt,args) > 0 ){
			OutputDebugStringW( buf );
		}
		va_end(args);
	}
};
#define	TRACE	CTrace::Trace