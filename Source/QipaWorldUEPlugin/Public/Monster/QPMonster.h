// QIPAWORLD

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/QPI_GetQPData.h"
#include "Animation/QPC_PlayMontage.h"
#include "Interface/QPI_GetAnimData.h"
#include "Interface/QPI_AnimNotify.h"
#include "Interface/QPI_AnimNotifyFootstep.h"
#include "Components/SphereComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Sound/QPC_PlayRandomSound.h"
//#include "Sound/QP_RandomSoundData.h"
#include "QPMonster.generated.h"




UCLASS()
class QIPAWORLDUEPLUGIN_API AQPMonster : public ACharacter, public IQPI_GetQPData, public IQPI_GetAnimData , public IQPI_AnimNotify, public IQPI_AnimNotifyFootstep
{
	GENERATED_BODY()
public:
	static int qp_characterDataMaxNum;
	/**��������*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QipaWorld|QPCharacter")
	FName qp_assetDataName = "assetDataName";
	/**��������*/
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QipaWorld|QPCharacter")
	//FName qp_playerDataKey = "qp_playerData";
	/**����ħ���ڵ����¼�����*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QipaWorld|QPCharacter")
	FName QP_AnimNotifyFireName = "QP_AnimNotifyFireName";
	/**������Ծ���¼�����*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QipaWorld|QPCharacter")
	FName QP_AnimNotifyJunmEndName = "qp_slimeJunmEnd";
	/**�������*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QipaWorld|QPCharacter")
	UQPData* qp_characterData = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QipaWorld|QPCharacter")
	UQPData* qp_animData = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QipaWorld|QPCharacter")
	UQPData* qp_saveData = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QipaWorld|QPCharacter")
	FString qp_saveName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QipaWorld|QPCharacter")
	UQPC_PlayMontage* qp_playMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QipaWorld|QPCharacter")
	float qp_walkSpeed = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QipaWorld|QPCharacter")
	float qp_sneakSpeed = 150.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QipaWorld|QPCharacter")
	float qp_runSpeed = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QipaWorld|QPCharacter")
	float qp_sneakMaxAcceleration = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QipaWorld|QPCharacter")
	float qp_walkMaxAcceleration = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QipaWorld|QPCharacter")
	float qp_runMaxAcceleration = 600.0f;

	/**����ٶ���run�����ܣ���walk������·*/
	float qp_maxSpeed = 250.0f;
	/**�Ƿ�����*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QipaWorld|QPCharacter")
	bool qp_isRun = false;
	/**�Ƿ���ǰ��*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QipaWorld|QPCharacter")
	bool qp_isSneak = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	USphereComponent* qp_sphereTrigger;
	//UCapsuleComponent* qp_capsuleTrigger;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QipaWorld|QPCharacter")
	//TMap<EQPFootstepType, FQP_SoundData > qp_footstepSounds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QipaWorld|QPCharacter")
	float qp_moveInterpSpeed = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QipaWorld|QPCharacter")

	bool qp_isAttacking = false;
	UCharacterMovementComponent* qp_movementC;
	float qp_forwardV = 0.0f;
	float qp_rightV = 0.0f;
	float qp_deltaTime = 0.f;
	class UQPDA_Character* qp_assetData = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QipaWorld|QPCharacter")
	class UQPC_MaterialAutoRestore* qp_materialAutoRestore = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QipaWorld|QPCharacter")
	UAudioComponent* qp_footstepAudio = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QipaWorld|QPCharacter")
	UQPC_PlayRandomSound* qp_playRandomSound = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QipaWorld|QPCharacter")
	bool qp_isPlayFootstepAudio = false;

	

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QipaWorld|QPCharacter")
	//TMap<EQPFootstepType, TArray<float> > qp_footstepSoundsVolume;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QipaWorld|QPCharacter")
	//FName QP_AnimNotifyFootsetpAudio = "QP_AnimNotifyFootsetpAudio";
public:
	// Sets default values for this character's properties
	AQPMonster();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual class UQPData* QP_GetQPData() override;
	virtual class UQPData* QP_GetAnimData() override;
	//if you want set save data,rewrite this function
	virtual void QP_ChangeSaveData();
	void QP_InitSaveData();
	virtual void QPI_AnimNotify(const FName& k) override;
	virtual void QPI_AnimNotifyFootstep(EQPFootstepType, FVector,float) override;

	virtual void QP_PlayFootstepAudio();
	virtual void QP_PlayAnim(FName name, FName StartSectionName = NAME_None);

	UFUNCTION()
	virtual void QP_OnTriggerOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void QP_OnTriggerOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	/**����ħ������*/
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPCharacter")
	virtual void QP_Fire();

	TArray<AActor*> qp_waters;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QipaWorld|QPBlind")
	TObjectPtr<USceneComponent> qp_deepWaterPoint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QipaWorld|QPBlind")
	TObjectPtr<USceneComponent> qp_underWaterPoint;

	//EQPFootstepType qp_waterFootstep = EQPFootstepType::WATER;
	//TObjectPtr<UCapsuleComponent> qp_capsuleComponent;
	UFUNCTION()
	virtual void QP_OnCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void QP_OnCapsuleEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};
