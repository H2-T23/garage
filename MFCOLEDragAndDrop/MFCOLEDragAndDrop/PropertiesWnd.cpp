
#include "stdafx.h"

#include "PropertiesWnd.h"
#include "Resource.h"
#include "MainFrm.h"
#include "MFCOLEDragAndDrop.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar

CPropertiesWnd::CPropertiesWnd()
{
	m_nComboHeight = 0;
}

CPropertiesWnd::~CPropertiesWnd()
{
}

BEGIN_MESSAGE_MAP(CPropertiesWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_EXPAND_ALL, OnExpandAllProperties)
	ON_UPDATE_COMMAND_UI(ID_EXPAND_ALL, OnUpdateExpandAllProperties)
	ON_COMMAND(ID_SORTPROPERTIES, OnSortProperties)
	ON_UPDATE_COMMAND_UI(ID_SORTPROPERTIES, OnUpdateSortProperties)
	ON_COMMAND(ID_PROPERTIES1, OnProperties1)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES1, OnUpdateProperties1)
	ON_COMMAND(ID_PROPERTIES2, OnProperties2)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES2, OnUpdateProperties2)
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar ���b�Z�[�W �n���h���[

void CPropertiesWnd::AdjustLayout()
{
	if (GetSafeHwnd () == NULL || (AfxGetMainWnd() != NULL && AfxGetMainWnd()->IsIconic()))
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndObjectCombo.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), m_nComboHeight, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top + m_nComboHeight, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndPropList.SetWindowPos(NULL, rectClient.left, rectClient.top + m_nComboHeight + cyTlb, rectClient.Width(), rectClient.Height() -(m_nComboHeight+cyTlb), SWP_NOACTIVATE | SWP_NOZORDER);
}

int CPropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// �R���{ �{�b�N�X�̍쐬:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER | CBS_SORT | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (!m_wndObjectCombo.Create(dwViewStyle, rectDummy, this, 1))
	{
		TRACE0("�v���p�e�B �R���{ �{�b�N�X���쐬�ł��܂���ł���\n");
		return -1;      // �쐬�ł��Ȃ��ꍇ
	}

	m_wndObjectCombo.AddString(_T("�A�v���P�[�V����"));
	m_wndObjectCombo.AddString(_T("�v���p�e�B �E�B���h�E"));
	m_wndObjectCombo.SetCurSel(0);

	CRect rectCombo;
	m_wndObjectCombo.GetClientRect (&rectCombo);

	m_nComboHeight = rectCombo.Height();

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("�v���p�e�B �O���b�h���쐬�ł��܂���ł���\n");
		return -1;      // �쐬�ł��Ȃ��ꍇ
	}

	InitPropList();

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROPERTIES);
	m_wndToolBar.LoadToolBar(IDR_PROPERTIES, 0, 0, TRUE /* ���b�N����Ă��܂�*/);
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_PROPERTIES_HC : IDR_PROPERTIES, 0, 0, TRUE /* ���b�N����܂���*/);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndToolBar.SetOwner(this);

	// ���ׂẴR�}���h���A�e�t���[���o�R�ł͂Ȃ����̃R���g���[���o�R�œn����܂�:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	AdjustLayout();
	return 0;
}

void CPropertiesWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CPropertiesWnd::OnExpandAllProperties()
{
	m_wndPropList.ExpandAll();
}

void CPropertiesWnd::OnUpdateExpandAllProperties(CCmdUI* /* pCmdUI */)
{
}

void CPropertiesWnd::OnSortProperties()
{
	m_wndPropList.SetAlphabeticMode(!m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnUpdateSortProperties(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnProperties1()
{
	// TODO: �����ɃR�}���h �n���h���[ �R�[�h��ǉ����܂�
}

void CPropertiesWnd::OnUpdateProperties1(CCmdUI* /*pCmdUI*/)
{
	// TODO:�����ɃR�}���h�X�V UI �n���h���[ �R�[�h��ǉ����܂�
}

void CPropertiesWnd::OnProperties2()
{
	// TODO: �����ɃR�}���h �n���h���[ �R�[�h��ǉ����܂�
}

void CPropertiesWnd::OnUpdateProperties2(CCmdUI* /*pCmdUI*/)
{
	// TODO:�����ɃR�}���h�X�V UI �n���h���[ �R�[�h��ǉ����܂�
}

void CPropertiesWnd::InitPropList()
{
	SetPropListFont();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("�\��"));

	pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("3D �\��"), (_variant_t) false, _T("�E�B���h�E�̃t�H���g�������ȊO�ɂȂ�A�܂��A�R���g���[���� 3D �{�[�_�[�ŕ`�悳��܂�")));

	CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("�r��"), _T("�_�C�A���O�g"), _T("���̂����̂ǂꂩ�ł� : �Ȃ��A�טg�A�T�C�Y�ύX�\�g�A�_�C�A���O�g"));
	pProp->AddOption(_T("�Ȃ�"));
	pProp->AddOption(_T("�טg"));
	pProp->AddOption(_T("�T�C�Y�ύX�\�g"));
	pProp->AddOption(_T("�_�C�A���O�g"));
	pProp->AllowEdit(FALSE);

	pGroup1->AddSubItem(pProp);
	pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("�L���v�V����"), (_variant_t) _T("�o�[�W�������"), _T("�E�B���h�E�̃^�C�g�� �o�[�ɕ\�������e�L�X�g���w�肵�܂�")));

	m_wndPropList.AddProperty(pGroup1);

	CMFCPropertyGridProperty* pSize = new CMFCPropertyGridProperty(_T("�E�B���h�E �T�C�Y"), 0, TRUE);

	pProp = new CMFCPropertyGridProperty(_T("����"), (_variant_t) 250l, _T("�E�B���h�E�̍������w�肵�܂�"));
	pProp->EnableSpinControl(TRUE, 50, 300);
	pSize->AddSubItem(pProp);

	pProp = new CMFCPropertyGridProperty( _T("��"), (_variant_t) 150l, _T("�E�B���h�E�̕����w�肵�܂�"));
	pProp->EnableSpinControl(TRUE, 50, 200);
	pSize->AddSubItem(pProp);

	m_wndPropList.AddProperty(pSize);

	CMFCPropertyGridProperty* pGroup2 = new CMFCPropertyGridProperty(_T("�t�H���g"));

	LOGFONT lf;
	CFont* font = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	font->GetLogFont(&lf);

	lstrcpy(lf.lfFaceName, _T("�l�r �o�S�V�b�N"));

	pGroup2->AddSubItem(new CMFCPropertyGridFontProperty(_T("�t�H���g"), lf, CF_EFFECTS | CF_SCREENFONTS, _T("�E�B���h�E�̊���t�H���g���w�肵�܂�")));
	pGroup2->AddSubItem(new CMFCPropertyGridProperty(_T("�V�X�e�� �t�H���g���g�p����"), (_variant_t) true, _T("�E�B���h�E�� MS Shell Dlg �t�H���g���g�p����悤�Ɏw�肵�܂�")));

	m_wndPropList.AddProperty(pGroup2);

	CMFCPropertyGridProperty* pGroup3 = new CMFCPropertyGridProperty(_T("���̑�"));
	pProp = new CMFCPropertyGridProperty(_T("(���O)"), _T("�A�v���P�[�V����"));
	pProp->Enable(FALSE);
	pGroup3->AddSubItem(pProp);

	CMFCPropertyGridColorProperty* pColorProp = new CMFCPropertyGridColorProperty(_T("�E�B���h�E�̐F"), RGB(210, 192, 254), NULL, _T("�E�B���h�E�̊���̐F���w�肵�܂�"));
	pColorProp->EnableOtherButton(_T("���̑�..."));
	pColorProp->EnableAutomaticButton(_T("����l"), ::GetSysColor(COLOR_3DFACE));
	pGroup3->AddSubItem(pColorProp);

	static const TCHAR szFilter[] = _T("�A�C�R�� �t�@�C�� (*.ico)|*.ico|���ׂẴt�@�C�� (*.*)|*.*||");
	pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("�A�C�R��"), TRUE, _T(""), _T("ico"), 0, szFilter, _T("�E�B���h�E �A�C�R�����w�肵�܂�")));

	pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("�t�H���_�["), _T("c:\\")));

	m_wndPropList.AddProperty(pGroup3);

	CMFCPropertyGridProperty* pGroup4 = new CMFCPropertyGridProperty(_T("�K�w"));

	CMFCPropertyGridProperty* pGroup41 = new CMFCPropertyGridProperty(_T("1 �Ԗڂ̃T�u���x��"));
	pGroup4->AddSubItem(pGroup41);

	CMFCPropertyGridProperty* pGroup411 = new CMFCPropertyGridProperty(_T("2 �Ԗڂ̃T�u���x��"));
	pGroup41->AddSubItem(pGroup411);

	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("���� 1"), (_variant_t) _T("�l 1"), _T("����͐����ł�")));
	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("���� 2"), (_variant_t) _T("�l 2"), _T("����͐����ł�")));
	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("���� 3"), (_variant_t) _T("�l 3"), _T("����͐����ł�")));

	pGroup4->Expand(FALSE);
	m_wndPropList.AddProperty(pGroup4);
}

void CPropertiesWnd::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndPropList.SetFocus();
}

void CPropertiesWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CDockablePane::OnSettingChange(uFlags, lpszSection);
	SetPropListFont();
}

void CPropertiesWnd::SetPropListFont()
{
	::DeleteObject(m_fntPropList.Detach());

	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	afxGlobalData.GetNonClientMetrics(info);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_fntPropList.CreateFontIndirect(&lf);

	m_wndPropList.SetFont(&m_fntPropList);
	m_wndObjectCombo.SetFont(&m_fntPropList);
}
