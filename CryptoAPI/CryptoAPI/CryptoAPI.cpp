// CryptoAPI.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "CryptoAPI.h"

#include <WinCrypt.h>

/**
 Crypto API

・対称鍵暗号化方式	：共通鍵による暗号方式
・非対称鍵暗号化方式：公開鍵による暗号方式

	・CryptoAcquireContext
	・CryptCreateHash
	・CryptHashData
	・CryptDeriveKey
	・CryptEncrypt
	・CryptDecrypt

	１．CSP(CryptoGraphy Service Provider)を準備する
	２．ハッシュ計算の準備をする
	３．パスワードからハッシュ値を算出する
	４・計算されたハッシュ値を鍵(暗号鍵)に変換する
	５．茂木のデータを暗号化(または復号)する

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
	/* [鍵生成]
	セッション鍵を生成する。
	一度だけ使用される鍵。（セキュリティのため）
	*/
	BOOL	GenKey( void ){
		BOOL	bResult	= ::CryptDeriveKey(m_hProvider
									,	CALG_RC4
									,	m_hHash
									,	(0x0080&0x10000)
									,	&m_hKey);
		return bResult;
	}
	/* [暗号化]
	大きなデータを暗号化する場合、
	第３引数をFALSEで関数コールし、最後にTRUEで暗号化する。
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
	/* [複号]
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
	/* [リリース]
	暗号化、複号の処理後のハンドラのリリース
	・鍵オブジェクト
	・ハッシュ計算
	・CSP
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


