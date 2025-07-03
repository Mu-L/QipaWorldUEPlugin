// QIPAWORLD

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Data/QPData.h"
#include "QPI_GetQPData.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UQPI_GetQPData : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class QIPAWORLDUEPLUGIN_API IQPI_GetQPData
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/**�������*/
	//UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPInterface")
	virtual UQPData* QP_GetQPData()=0;

	/**��ʼ������*/
	//UFUNCTION()
	//virtual void QP_InitQPData() = 0;
	//void QP_InitQPData();

//protected:
	/**�������*/
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "QipaWorld|QPInterface")
	//UQPData* qp_data = nullptr;
};
