#pragma once

/**********************************************************************************
 *
 *
 *
 */
class NonCopyable {
protected:
	NonCopyable( void ){}
	~NonCopyable( void ){}
private:
	NonCopyable( const NonCopyable& );
	NonCopyable& operator=( const NonCopyable& );
};
