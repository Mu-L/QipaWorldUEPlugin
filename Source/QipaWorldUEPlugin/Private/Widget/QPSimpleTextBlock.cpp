// QIPAWORLD


#include "Widget/QPSimpleTextBlock.h"

void UQPSimpleTextBlock::QP_ChangeFontSlate() {

	if (qp_changeSlate && MyTextBlock.IsValid()) {
		
		FSlateFontInfo fontinfoBase = GetFont();
		const FSlateFontInfo fontinfo = FSlateFontInfo(fontinfoBase.FontObject, fontinfoBase.Size,* UQPGIM_Localization::QP_UQPGIM_Localization->QP_GetCurrentLanguage()); // Font, Font Size  for the chatbox
		SetFont(fontinfo);
	}
}