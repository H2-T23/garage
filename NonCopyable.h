#ifndef __NON_COPYABLE_H__
#define __NON_COPYABLE_H__

#if _MSC_VER >= 1000
#pragma once
#endif

#define	NON_COPYABLE(classname)	private:classname(const classname&); classname& operator=(const classname&);

#endif//__NON_COPYABLE_H__