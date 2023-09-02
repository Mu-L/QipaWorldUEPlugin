// QIPAWORLD

#pragma once

#include "CoreMinimal.h"
#include "DataAsset/QPDataAsset.h"
#include "QPDS_DataAsset.generated.h"

/**
 * 
 */
UCLASS()
class QIPAWORLDUEPLUGIN_API UQPDS_DataAsset : public UQPDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "QipaWorld|defaultSetting")
	FString QP_DefaultStartMap = "None";
	/**�����ؽ������ĵ�ͼ·��*/
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "QipaWorld|defaultSetting")
	FString QP_DefaultLoadingMap = "None";
	/**Ĭ�ϵ�ui��ͼ·��*/
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "QipaWorld|defaultSetting")
	FString QP_DefaultMainUserInterfacePath = "None";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QipaWorld|defaultSetting")
	FString QP_DefaultCharacterDataPath = "None";

	/*�����Ĭ�ϵĵ���ui������key��������ڶ������������������¼������Ұ󶨰����������Զ������͹ر�ui*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QipaWorld|defaultSetting")
	FString QP_DefaultUserInterfaceActionKey = "DefaultUserInterfaceAction";
	/*������Ϊ�棬�����key��Ч*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QipaWorld|defaultSetting")
	bool QP_UserInterfaceAutoPop = true;
	/**UUID*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QipaWorld|defaultSetting")
	TMap<FString, int32> QP_UUID;
};
