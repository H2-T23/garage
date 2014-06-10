#pragma once

#pragma comment(lib,"Crypt32.lib")
#pragma comment(lib,"Secur32.lib")

#define	SECURITY_WIN32
#include <Security.h>
#include <schnlsp.h>


namespace SSL {

//	typedef unsigned		HANDLE;

//	typedef HANDLE			CredHandle;
//	typedef HANDLE			CtxtHandle;

	typedef	std::string		String;

	class CSSL {
	private:
		PCCERT_CONTEXT				m_pCertificate;

		CredHandle					m_hCredHandle;
		CtxtHandle					m_hContextHandle;
		SecPkgContext_StreamSizes	m_contextSizes;
		SecPkgContext_Lifepan		m_lifeSpan;
		bool						m_contextInitated;
		bool						m_handshakeInProgress;
		bool						m_isClient;

	public:
		CSSL( void )
		 : m_pCertificate(NULL)
		 , m_contextInitiated(false)
		 , m_isClient(true)
		 , m_handshakeInProgress(false)
		{
			SecInvalidateHandle( &m_hCredHandle );
			SecInvalidateHandle( &m_hContextHandle );
		}

		~CSSL( void );

		bool	init( bool isClient, PCCERT_CONTEXT pCertificate ){
			m_isClient	= isClient;
			m_pCertificate	= pCertificate ? pCertificate : NULL;

			SCHANNEL_CRED		schannel;
			ZeroMemory(&schannel, sizeof(schannel));

			schannel.dwVersion	= SCHANNEL_CRED_VERSION;
			schannel.dwFlags	= SCH_CRED_NO_DEFAULT_CREADS
								| SCH_CRED_NO_SYSTEM_MAPPER
								| SCH_CRED_REVOCATION_CHECK_CHAIN;
			schannel.cCreds		= 1;
			schannel.paCred		= &m_pCertificate;

			if( !m_isClient ){
				schannel.dwMinimumCipherStrength	= 128;
			}

			DWORD	dwErr	= 0;
			if( dwErr != SEC_E_OK ){
				::SetLastError( dwErr );
				return false;
			}

			return(m_handshakeInProgress = true);
		}

		void	close( void ){
			if( SecIsValidHandle( &m_hCredHandle ) ){
				FreeCredentialHandle( &m_hCredHandle );
			}

			if( SecIsValidHandle( &m_hContextHandle ) ){
				FreeCredentialHandle( &m_hContextHandle );
			}

			m_contextInitiated	= false;
		}

		String		getCertificateInfoString( void ) const throw() {
			String		strResult;

			PCCERT_CONTEXT	pRemoteCertContxt	= NULL;

			SECURITY_STATUS status	= QueryContextAttributes( (PCtxtHandle)&m_hContextHandle, SECPKG_ATTR_REMOTE_CERT_CONTEXT, (PVOID)&pRemoteCertContext );
			if( status != SEC_E_OK ){
				return strResult;
			}

			DWORD	dwReq	= CertGetNameString( pRemoteCertContext, CERT_NAME_FRIENDLY_DISPLAY_TYPE, 0, NULL, NULL, 0);
			strResult.resize( dwReq );

			CertGetNameString( pRemoteCertContext, CERT_NAME_FRENDLY_DISPLAY_TYPE, 0, NULL, &strResult.at( 0 ), dwReq );
			CertFreeCertificateContext( pRemoteCertContext );

			return strResult;
		}
	
	public:
		bool		Decode();
		bool		Encode();
	};


}// end of "SSL".