#include "PropertyPanel.h"

/**********************************************************************************
 *
 *
 *
 */
IMPLEMENT_DYNAMIC(CPropertyPanel, CPanel)
/**********************************************************************************
 *
 *
 *
 */
CPropertyPanel::CPropertyPanel()
{
}
/**********************************************************************************
 *
 *
 *
 */
CPropertyPanel::~CPropertyPanel()
{
}
/**********************************************************************************
 *
 *
 *
 */
BEGIN_MESSAGE_MAP(CPropertyPanel, CPanel)
	ON_WM_CREATE()
	ON_COMMAND(CPropertyPanel::IDC_BUTTON1, &CPropertyPanel::OnBtnClick1)
	ON_COMMAND(CPropertyPanel::IDC_BUTTON2, &CPropertyPanel::OnBtnClick2)
END_MESSAGE_MAP()
/**********************************************************************************
 *
 *
 *
 */
/**********************************************************************************
 *
 *
 *
 */
/**********************************************************************************
 *
 *
 *
 */