// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "QPWorldObjectManager.generated.h"
/**
 * 
 */
UCLASS()
class QIPAWORLDUEPLUGIN_API UQPWorldObjectManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	static UQPWorldObjectManager* QP_UQPWorldObjectManager;

	/*UFUNCTION(BlueprintCallable,Category = "QipaWorld|Data")
	void QP_Init();*/

	virtual bool ShouldCreateSubsystem(UObject* Outer) const;

	/** Implement this for initialization of instances of the system */
	virtual void Initialize(FSubsystemCollectionBase& Collection);

	/** Implement this for deinitialization of instances of the system */
	virtual void Deinitialize();

public:
	TMap<FString,TArray<UObject*>> qp_objData;

	/**���ò����Ķ���ŵ��������*/
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|Object")
	void QP_CollectObject(FString key, UObject* obj);

	UFUNCTION(BlueprintCallable, Category = "QipaWorld|Object")
	UObject* QP_GetObject(FString key);


	/**�첽����*/
	/*UFUNCTION(BlueprintCallable, Category = "QipaWorld|Assets")
	void QP_RequestAsyncLoad(UClass* InBaseClass,FString key, UQPData* data);

	UFUNCTION(BlueprintCallable, Category = "QipaWorld|Assets")
	void QP_RequestAsyncLoad(UClass* InBaseClass, FSoftObjectPath& keys, UQPData* data);


	UFUNCTION(BlueprintCallable, Category = "QipaWorld|Assets")
	void QP_RequestAsyncLoad(UClass* InBaseClass, TArray<FSoftObjectPath> keys, UQPData* data);

	void QP_RequestAsyncLoadDeferred();*/
};
