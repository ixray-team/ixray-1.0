#pragma once

#include "ui3tbutton.h"
#include "UIOptionsItem.h"

class CUICheckButton : public CUI3tButton, public CUIOptionsItem {
	friend class CUIXmlInit;
	typedef CUI3tButton			inherited;
public:
	CUICheckButton(void);
	virtual ~CUICheckButton(void);

	virtual void Update();

	// CUIOptionsItem
	virtual void	SetCurrentValue();
	virtual void	SaveValue();
	virtual bool	IsChanged();
	virtual void 	SeveBackUpValue			();
	virtual void 	Undo					();

	virtual void Init(Fvector2 pos, Fvector2 size, LPCSTR texture_name);
	virtual void SetTextX(float x) {/*do nothing*/}
	virtual void OnFocusLost();
	virtual void OnFocusReceive();
	virtual void Show(bool status);

	//состояние кнопки
	bool GetCheck()					{return m_eButtonState == BUTTON_PUSHED;}
	void SetCheck(bool ch)			{m_eButtonState = ch ? BUTTON_PUSHED : BUTTON_NORMAL;}

	void SetDependControl(CUIWindow* pWnd);

private:
	bool			b_backup_val;
	void InitTexture(LPCSTR texture_name);
	CUIWindow* m_pDependControl;
};