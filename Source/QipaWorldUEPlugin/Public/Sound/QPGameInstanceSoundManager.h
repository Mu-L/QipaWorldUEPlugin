// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "QPGameInstanceSoundManager.generated.h"

class UQPData;
/**
 * 
 */
UCLASS()
class QIPAWORLDUEPLUGIN_API UQPGameInstanceSoundManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	/** ���˽�����ݰ��õ� */
	//FDelegateHandle qp_handle;
public:
	static UQPGameInstanceSoundManager* QP_UQPGameInstanceSoundManager;

	/*UFUNCTION(BlueprintCallable,Category = "QipaWorld|Data")
	void QP_Init();*/

	virtual bool ShouldCreateSubsystem(UObject* Outer) const;

	/** Implement this for initialization of instances of the system */
	virtual void Initialize(FSubsystemCollectionBase& Collection);

	/** Implement this for deinitialization of instances of the system */
	virtual void Deinitialize();

public:
	/***/
	UQPData* qp_soundData;
	
	UAudioComponent* qp_bgSound = nullptr;
	UAudioComponent* qp_UISound = nullptr;

	float qp_pitch = 1.0f;
	float qp_volume = 1.0f;
	/**������Ϸ�õ�key�����id*/
	FString qp_SaveSlotName = TEXT("qp_soundSaveGame");;
	uint32 qp_UserIndex = 0;

	/** ����������ӹؿ��Ƿ�������*/
	//UPROPERTY(BlueprintReadWrite)
	//	bool qp_isLoadMap = false;

	///** ����������ӹؿ��Ƿ�������*/
	//UPROPERTY(BlueprintReadWrite)
	//	bool qp_isLoadSubMap = false;

	/////** �������ػ��ߵ�ǰlevel�����֡�*/

	////UPROPERTY(BlueprintReadWrite)
	////FString qp_loadMapName;

	/**���سɹ���Ļص�*/
	//FLatentActionInfo qp_actionInfo;
	/** Subsystem ��û��beginplay��ֻ����ͼ�����*/

	UFUNCTION(BlueprintCallable, Category = "QipaWorld|Sound")
	void QP_BeginPlayInit();

	
	/** ����Դ·�����ñ�������*/
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|Sound")
	void QP_SetBgMusicByPath(const FString soundPath);

	/** ��SoundBase���ñ�������*/
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|Sound")
	void QP_SetBgMusicBySound(USoundBase* sound);

	/** ����Դ·�����ű�������*/
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|Sound")
	void QP_PlayBgMusicByPath(const FString soundPath);

	/** ��SoundBase���ű�������*/
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|Sound")
	void QP_PlayBgMusicBySound(USoundBase* sound);

	/** ���ű�������*/
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|Sound")
	void QP_PlayBgMusic();


	/** ����Դ·������UI�����Ч*/
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|Sound")
	void QP_SetClickUISoundByPath(const FString soundPath);

	/** ��SoundBase����UI�����Ч*/
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|Sound")
	void QP_SetClickUISoundBySound(USoundBase* sound);

	/** ����Դ·������UI�����Ч*/
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|Sound")
	void QP_PlayClickUISoundByPath(const FString soundPath);

	/** ��SoundBase����UI�����Ч*/
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|Sound")
	void QP_PlayClickUISoundBySound(USoundBase* sound);

	/** ����UI�����Ч*/
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|Sound")
	void QP_PlayClickUISound();

	/** ��·������UIһ���Ե���Ч*/
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|Sound")
	void QP_PlayDisposablesUISoundByPath(const FString soundPath);

	/** ��Sound����UIһ���Ե���Ч*/
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|Sound")
	void QP_PlayDisposablesUISoundBySound(USoundBase* sound);

	void QP_CreateAudioComponent(UAudioComponent*& audio, USoundBase* sound);

	void QP_BindSoundData(const UQPData* data);

	void QP_SaveSoundData();
	void QP_SavedDelegate(const FString& SlotName, const int32 UserIndex, bool bSuccess);
	void QP_LoadSoundData();
	USoundBase* QP_CreateSoundBase(FString path);

};
