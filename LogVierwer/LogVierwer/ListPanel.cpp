#include "StdAfx.h"
#include "ListPanel.h"

/********************************************************************************************************************************************************************
 *
 *
 *
 */
IMPLEMENT_DYNAMIC(CListPanel, CSplitterPanel)
/********************************************************************************************************************************************************************
 *
 *
 *
 */
CListPanel::CListPanel( void )
{
}
/********************************************************************************************************************************************************************
 *
 *
 *
 */
CListPanel::~CListPanel( void )
{
}
/********************************************************************************************************************************************************************
 *
 *
 *
 */
BEGIN_MESSAGE_MAP(CListPanel,CSplitterPanel)
	ON_WM_CREATE()
	ON_WM_SIZE()
//	ON_MESSAGE(WM_SET_MULTITEXT, &CListTab::OnSetText)
END_MESSAGE_MAP()
/********************************************************************************************************************************************************************
 *
 *
 *
 */
/********************************************************************************************************************************************************************
 *
 *
 *
 */
/********************************************************************************************************************************************************************
 *
 *
 *
 */
