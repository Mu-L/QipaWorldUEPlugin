// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "QPGameInstanceMapManager.generated.h"

/**
 * 
 */
UCLASS()
class QIPAWORLDUEPLUGIN_API UQPGameInstanceMapManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static UQPGameInstanceMapManager* QP_UQPGameInstanceMapManager;

	/*UFUNCTION(BlueprintCallable,Category = "QipaWorld|Data")
	void QP_Init();*/

	virtual bool ShouldCreateSubsystem(UObject* Outer) const;

	/** Implement this for initialization of instances of the system */
	virtual void Initialize(FSubsystemCollectionBase& Collection);

	/** Implement this for deinitialization of instances of the system */
	virtual void Deinitialize();

public:
	/** ����������ӹؿ��Ƿ�������*/
	UPROPERTY(BlueprintReadWrite)
	bool qp_isLoadMap = false;

	/** ����������ӹؿ��Ƿ�������*/
	UPROPERTY(BlueprintReadWrite)
	bool qp_isLoadSubMap = false;

	/** �������ص�level�����֡�*/
	FString qp_readyOpenMapName;

	FString qp_loadingMapName;

	/**���سɹ���Ļص�*/
	FLatentActionInfo qp_actionInfo;

	/** ��ж�����ڵ�level��ֱ�Ӽ����µ�*/
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|Map")
	void QP_LoadMap(const FString MapName, const FVector Location, const FRotator Rotation);

	/** ���ֱ���л���ͼ��*/
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|Map")
	void QP_OpenMap(const FString MapName);

	/** ��������loading������֮�����л���ͼ��*/
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|Map")
	void QP_LoadingAndOpenMap(const FString MapName);
	/** ������qp_readyOpenMapName�󣬿���������л���ͼ��*/
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|Map")
	void QP_OpenReadyMap();


	/** �����ӵ�ͼ*/
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|Map")
	void QP_LoadSubMap(const FString MapName);

	/*UFUNCTION(BlueprintCallable, Category = "QipaWorld|Map")*/
	void QP_LoadMapEnd();
private:
	

};
