// CryptoAPI.cpp : �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#include "CryptoAPI.h"

#include <WinCrypt.h>

/**
 Crypto API

�E�Ώ̌��Í�������	�F���ʌ��ɂ��Í�����
�E��Ώ̌��Í��������F���J���ɂ��Í�����

	�ECryptoAcquireContext
	�ECryptCreateHash
	�ECryptHashData
	�ECryptDeriveKey
	�ECryptEncrypt
	�ECryptDecrypt

	�P�DCSP(CryptoGraphy Service Provider)����������
	�Q�D�n�b�V���v�Z�̏���������
	�R�D�p�X���[�h����n�b�V���l���Z�o����
	�S�E�v�Z���ꂽ�n�b�V���l����(�Í���)�ɕϊ�����
	�T�D�Ζ؂̃f�[�^���Í���(�܂��͕���)����

 */
class CCrypto {
protected:
	HCRYPTPROV			m_hProvider;
	HCRYPTHASH			m_hHash;
	HCRYPTKEY			m_hKey;

public:
	BOOL	KeyContainer( LPCTSTR lpszProvider = MS_ENHANCED_PROV, int nType = PROV_RSA_FULL ){
		BOOL	bResult = ::CryptAcquireContext(&m_hProvider
									,	NULL
									,	lpszProvider
									,	nType
									,	0);
		if( !bResult ){
			bResult = ::CryptAcquireContext(&m_hProvider
									,	NULL
									,	lpszProvider
									,	nType
									,	CRYPT_NEWKEYSET);
		}
		return bResult;
	}

	BOOL	CreateHash( LPBYTE lpPass, DWORD dwLen ){
		BOOL	bResult	= ::CryptCreateHash(m_hProvider
									,	CALG_SHA
									,	0, 0
									,	&m_hHash);
		if( bResult )
		{
			bResult	= ::CryptHashData(m_hHash, lpPass, dwLen, 0);

		}
		return bResult;
	}
	/* [������]
	�Z�b�V�������𐶐�����B
	��x�����g�p����錮�B�i�Z�L�����e�B�̂��߁j
	*/
	BOOL	GenKey( void ){
		BOOL	bResult	= ::CryptDeriveKey(m_hProvider
									,	CALG_RC4
									,	m_hHash
									,	(0x0080&0x10000)
									,	&m_hKey);
		return bResult;
	}
	/* [�Í���]
	�傫�ȃf�[�^���Í�������ꍇ�A
	��R������FALSE�Ŋ֐��R�[�����A�Ō��TRUE�ňÍ�������B
	*/
	BOOL	Encrypt( LPBYTE lpData, DWORD* lpdwSize ){
		BOOL	bResult	= ::CryptEncrypt(m_hKey
								,		0
								,		TRUE
								,		0
								,		lpData
								,		lpdwSize
								,		*lpdwSize	);
		return bResult;
	}
	/* [����]
	*/
	BOOL	Decrypt( LPBYTE lpData, DWORD* lpdwSize ){
		BOOL	bResult	= ::CryptDecrypt(m_hKey
									,	0
									,	TRUE
									,	0
									,	lpData
									,	lpdwSize);
		return bResult;
	}
	/* [�����[�X]
	�Í����A�����̏�����̃n���h���̃����[�X
	�E���I�u�W�F�N�g
	�E�n�b�V���v�Z
	�ECSP
	*/
	void	Release( void ){
		::CryptDestroyKey( m_hKey );
		::CryptDestroyHash( m_hHash );
		::CryptReleaseContext( m_hProvider, 0 );
	}
};

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

}


