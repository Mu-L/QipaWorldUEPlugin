// QIPAWORLD


#include "Widget/QPC_Text3dL10N.h"

void UQPC_Text3dL10N::BeginPlay() {
	Super::BeginPlay();
	if (UQPGIM_Localization::QP_UQPGIM_Localization != nullptr) {

		//qp_text3dComponent = GetOwner();
		qp_text3dComponent = Cast<UText3DComponent>(GetOwner()->GetComponentsByClass(UText3DComponent::StaticClass())[0]);
		
		qp_fontPath = qp_text3dComponent->Font.GetPathName();
		UQPGIM_Localization::QP_UQPGIM_Localization->qp_localizationData->qp_dataDelegate.AddUObject(this, &UQPC_Text3dL10N::QP_L10NChange);
		QP_ChangeFontSlate();
	}
}



void UQPC_Text3dL10N::QP_L10NChange(const UQPData* data) {

	QP_ChangeFontSlate();
}
void UQPC_Text3dL10N::QP_ChangeFontSlate() {
	if (!GetOwner()->IsHidden()) {

		//const FSlateFontInfo fontinfoBase = qp_text3dComponent->Font->GetFont();

		UFont* font = LoadObject<UFont>(nullptr, *UQPGIM_Localization::QP_UQPGIM_Localization->QP_GetL10NAssetsPath(qp_fontPath));
		//qp_text3dComponent->get
		//const FSlateFontInfo fontinfo = FSlateFontInfo(font,1); // Font, Font Size  for the chatbox
		qp_text3dComponent->SetFont(font);
	}
}