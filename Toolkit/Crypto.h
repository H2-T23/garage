#pragma once

#include <windows.h>

#pragma comment(lib, "advapi32.lib")


namespace CRYPTO {
	class CCrypto {
	protected:
		HCRYPTPROV		m_hProvider;

	public:
		CCrypt( void ) : m_hProvider(NULL) {
		}

		BOOL	Startup( void ){
			return ::CryptAcquireContent( &m_hProvider, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT );
		}

		void	Cleanup( void ){
			::CryptReleaseContext( m_hProvider, 0 );
		}

		DWORD	GenRandom( void ){
			DWROD			dwRandom	= 0;
			const DWORD		dwLength	= sizeof(DWORD);

			if( !::CryptGenRandom(m_hProvider, dwLength, &dwRandom) ){
				dwRandom	= 0;
			}

			return dwRandom;
		}
	};
}// end of "CRYPT".
