// QIPAWORLD

#pragma once

#include "CoreMinimal.h"
#include "DataAsset/QPDataAsset.h"
//#include "SoundControlBus.h"
#include "SoundControlBusMix.h"
#include "QPDS_DataAsset.generated.h"

/**
 * qp plugin setting asset
 */
UCLASS()
class QIPAWORLDUEPLUGIN_API UQPDS_DataAsset : public UQPDataAsset
{
	GENERATED_BODY()
public:

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "QipaWorld|defaultSetting")
	TSoftObjectPtr<UWorld> QP_DefaultStartMap;
	/**�����ؽ������ĵ�ͼ·��*/
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "QipaWorld|defaultSetting")
	TSoftObjectPtr<UWorld> QP_DefaultLoadingMap;
	/**Ĭ�ϵ�ui��ͼ·��*/
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "QipaWorld|defaultSetting")
	TSoftClassPtr<UUserWidget> QP_DefaultMainUserInterface ;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QipaWorld|defaultSetting", meta = ( LongPackageName))
	FDirectoryPath QP_DefaultCharacterDataPath;

	/*�����Ĭ�ϵĵ���ui������key��������ڶ������������������¼������Ұ󶨰����������Զ������͹ر�ui*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QipaWorld|defaultSetting")
	FString QP_DefaultUserInterfaceActionKey = "DefaultUserInterfaceAction";
	/*������Ϊ�棬�����key��Ч*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QipaWorld|defaultSetting")
	bool QP_UserInterfaceAutoPop = true;

	/*Ĭ����Ч����*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QipaWorld|defaultSetting")
	TSoftObjectPtr<USoundControlBusMix> QP_DefaultUserAudioSetting ;

	/*Open UE DynamicResolutionEnabled*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QipaWorld|defaultSetting")
	bool QP_DynamicResolutionEnabled = true;

	/**UUID*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QipaWorld|defaultSetting")
	TMap<FString, int32> QP_UUID;
};
