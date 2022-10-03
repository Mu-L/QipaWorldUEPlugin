// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "QPGIM_Sound.generated.h"


class UQPData;
/**
 * 
 */
UCLASS()
class QIPAWORLDUEPLUGIN_API UQPGIM_Sound : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	
public:
	static UQPGIM_Sound* QP_UQPGIM_Sound;

	/*UFUNCTION(BlueprintCallable,Category = "QipaWorld|Data")
	void QP_Init();*/

	virtual bool ShouldCreateSubsystem(UObject* Outer) const;

	/** Implement this for initialization of instances of the system */
	virtual void Initialize(FSubsystemCollectionBase& Collection);

	/** Implement this for deinitialization of instances of the system */
	virtual void Deinitialize();

public:
	/**´æ·ÅÁËÒôÀÖµÄÊý¾Ý*/
	UPROPERTY(BlueprintReadWrite)
	UQPData* qp_soundData;
	
	UAudioComponent* qp_bgSound = nullptr;
	UAudioComponent* qp_UISound = nullptr;

	float qp_musicVolume = 1.0f;
	float qp_soundVolume = 1.0f;
	/**保存声音设置的文件名字*/
	FString qp_SaveSlotName = TEXT("qp_soundSaveGame");
	/**保存声音设置的玩家id*/
	uint32 qp_UserIndex = 0;

	
	/** Subsystem 没有BeginPlay，这个必须beginplay里调用，没有world，不能播放声音*/

	UFUNCTION(BlueprintCallable, Category = "QipaWorld|Sound")
	void QP_BeginPlayInit();

	
	/** 用路径设置背景音乐*/
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|Sound")
	void QP_SetBgMusicByPath(const FString soundPath);

	/** 用sound设置背景音乐*/
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|Sound")
	void QP_SetBgMusicBySound(USoundBase* sound);

	/** 用路径播放背景音乐*/
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|Sound")
	void QP_PlayBgMusicByPath(const FString soundPath);

	/** 用sound播放背景音乐*/
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|Sound")
	void QP_PlayBgMusicBySound(USoundBase* sound);

	/** 播放背景音乐*/
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|Sound")
	void QP_PlayBgMusic();


	/** 用路径设置点击ui的音效*/
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|Sound")
	void QP_SetClickUISoundByPath(const FString soundPath);

	/** 用sound设置点击ui的音效*/
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|Sound")
	void QP_SetClickUISoundBySound(USoundBase* sound);

	/** 用路径播放点击ui的音效*/
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|Sound")
	void QP_PlayClickUISoundByPath(const FString soundPath);

	/** 用sound播放点击ui的音效*/
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|Sound")
	void QP_PlayClickUISoundBySound(USoundBase* sound);

	/** 播放点击ui的音效*/
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|Sound")
	void QP_PlayClickUISound();

	/** 用路径播放一次性音效*/
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|Sound")
	void QP_PlayDisposablesUISoundByPath(const FString soundPath);

	/** 用sound播放一次性音效*/
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|Sound")
	void QP_PlayDisposablesUISoundBySound(USoundBase* sound);

	void QP_CreateAudioComponent(UAudioComponent*& audio,float volume, USoundBase* sound);

	void QP_BindSoundData(const UQPData* data);

	void QP_SaveSoundData();
	void QP_SavedDelegate(const FString& SlotName, const int32 UserIndex, bool bSuccess);
	void QP_LoadSoundData();
	USoundBase* QP_CreateSoundBase(FString path);

};
