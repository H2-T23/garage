#include "SplitterPanel.h"

/**********************************************************************************
 *
 *
 *
 */
IMPLEMENT_DYNAMIC(CSplitterPanel, CPanel)
/**********************************************************************************
 *
 *
 *
 */
CSplitterPanel::CSplitterPanel()
{
}
/**********************************************************************************
 *
 *
 *
 */
CSplitterPanel::~CSplitterPanel()
{
}
/**********************************************************************************
 *
 *
 *
 */
BEGIN_MESSAGE_MAP(CSplitterPanel, CPanel)
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()
/**********************************************************************************
 *
 *
 *
 */


