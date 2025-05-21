﻿// QIPAWORLD


#include "Character/QPCharacter.h"

#include <complex.h>
#include <Camera/CameraComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include "Engine/SkeletalMeshSocket.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "Data/QPGIM_Data.h"
#include "UObject/UnrealType.h"
#include "Data/QPData.h"
#include "Character/QPGIM_Character.h"

#include "Notify/QPGIM_AnimNotifyData.h"

// Sets default values
int AQPCharacter::qp_characterMaxNum = 1;

AQPCharacter::AQPCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	qp_characterMaxNum++;
	qp_camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	qp_springArm = CreateDefaultSubobject<USpringArmComponent>("springArm");
	qp_springArm->bUsePawnControlRotation = true;
	qp_materialAutoRestore = CreateDefaultSubobject<UQPC_MaterialAutoRestore>("qp_materialAutoRestore");

	bUseControllerRotationYaw = false;
	qp_springArm->SetupAttachment(RootComponent);
	qp_camera->SetupAttachment(qp_springArm);
	qp_playMontage = CreateDefaultSubobject<UQPC_PlayMontage>("qp_playMontage");
	//qp_attackAnim->SetupAttachment(RootComponent);
	//qp_restoreNg = CreateDefaultSubobject<UNiagaraComponent>("qp_restoreNg");
	//qp_restoreNg->SetupAttachment(RootComponent);
	//qp_aterialDissolve->QP_SetRootComponent(RootComponent);


}
//void AQPCharacter::OnConstruction(const FTransform& Transform) {
//	Super::OnConstruction(Transform);
//
//	if (UQPGIM_Data::qp_staticObject) {
//	}
//}
// Called when the game starts or when spawned
void AQPCharacter::BeginPlay()
{
	Super::BeginPlay();
	QP_GetQPData();

	//QP_InitQPData();
	//if (qp_characterData == nullptr) {
		//qp_characterData = UQPGIM_Data::qp_staticObject->QP_GetQPData("AQPCharacter")->QP_GetUQPData("qp_Character" + qp_characterMaxNum);
	//}
	
	qp_movementC = GetCharacterMovement();
	qp_movementC->bOrientRotationToMovement = true;
	qp_movementC->MaxAcceleration = qp_walkMaxAcceleration;
	qp_movementC->bRunPhysicsWithNoController = true;
	qp_movementC->MaxWalkSpeed = qp_walkSpeed;
	qp_isFixedCamera = false;
	qp_isRun = false;
	qp_maxSpeed = qp_walkSpeed;
	qp_characterData->QP_GetUQPData(UQPGIM_AnimNotifyData::QP_DATA_BASE_NAME)->qp_dataDelegate.AddUObject(this, &AQPCharacter::QP_AnimNotifyEvent);
	
	if (!qp_assetData) {
		UQPGIM_Character::qp_staticObject->QP_InitCharacterData(this);
	}
}

// Called every frame
void AQPCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	qp_deltaTime = DeltaTime;

	if (qp_isAutoCameraLength) {
		
		if (qp_springArm->TargetArmLength>= qp_targetCameraLength) {
			QP_mouseWheelUp();
			if (qp_springArm->TargetArmLength <= qp_targetCameraLength) {
				qp_isAutoCameraLength = false;
			}
		}
		else if (qp_springArm->TargetArmLength <= qp_targetCameraLength) {
			QP_mouseWheelDown();
			if (qp_springArm->TargetArmLength >= qp_targetCameraLength) {
				qp_isAutoCameraLength = false;
			}
		}
		
		 
	}

	if (qp_movementC->IsFalling()) {
		if (qp_characterData->QP_GetFString("characterJump") != "start") {
			if (qp_characterData->QP_GetFString("characterFall") != "start") {
				qp_characterData->QP_AddFString("characterFall", "start");
			}
		}
	}
	QP_UpdateMaxSpeed();
}

// Called to bind functionality to input
void AQPCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(qp_mouseWhellAxis, this, &AQPCharacter::QP_MouseWheelAxis);
	PlayerInputComponent->BindAxis(qp_moveForWard, this, &AQPCharacter::QP_MoveForward);
	PlayerInputComponent->BindAxis(qp_moveRight, this, &AQPCharacter::QP_MoveRight);
	PlayerInputComponent->BindAxis(qp_turn, this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis(qp_lookUp, this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAction(qp_jump, IE_Pressed, this, &AQPCharacter::QP_JumpStart);
	PlayerInputComponent->BindAction(qp_jump, IE_Released, this, &AQPCharacter::QP_JumpEnd);
	PlayerInputComponent->BindAction(qp_run, IE_Pressed, this, &AQPCharacter::QP_Run);
	PlayerInputComponent->BindAction(qp_sneak, IE_Released, this, &AQPCharacter::QP_Sneak);
	PlayerInputComponent->BindAction(qp_attack, IE_Pressed, this, &AQPCharacter::QP_AttackStart);
	PlayerInputComponent->BindAction(qp_attack, IE_Released, this, &AQPCharacter::QP_AttackEnd);
	PlayerInputComponent->BindAction(qp_fixedCamera, IE_Pressed, this, &AQPCharacter::QP_FixedCamera);
	PlayerInputComponent->BindAction(qp_attackTwo, IE_Pressed, this, &AQPCharacter::QP_AttackTwoStart);
	PlayerInputComponent->BindAction(qp_attackTwo, IE_Released, this, &AQPCharacter::QP_AttackTwoEnd);
	PlayerInputComponent->BindAction(qp_mouseWheelUp, IE_Pressed, this, &AQPCharacter::QP_mouseWheelUp);
	PlayerInputComponent->BindAction(qp_mouseWheelDown, IE_Pressed, this, &AQPCharacter::QP_mouseWheelDown);
	PlayerInputComponent->BindAction(qp_changeCharacter, IE_Pressed, this, &AQPCharacter::QP_ChangeCharacter);
	
		
		
}
//void AQPCharacter::QP_InitQPData() {
//	
//	
//}

void AQPCharacter::QP_SetMatAmount(float amount)
{
	//qp_matAmount = amount;
	//qp_matInstance->SetScalarParameterValue("qp_amount", qp_matAmount);
	qp_materialAutoRestore->QP_SetMatAmount(amount);
	

}
UQPData* AQPCharacter::QP_GetQPData(){

	if (!qp_characterData) {
		qp_characterData = UQPGIM_Data::qp_staticObject->QP_GetUQPData("AQPCharacter")->QP_GetUQPData("qp_Character" + qp_characterMaxNum);

	}
	return qp_characterData;
 }
//virtual void QP_MouseWheelAxis(float value);

void AQPCharacter::QP_MouseWheelAxis(float value)
{
	
}

void AQPCharacter::QP_MoveForward(float value)
{
	 qp_forwardV = value;
	if (value != 0) {
		
		FRotator rotator = GetControlRotation();
		rotator.Roll = 0.0f;
		rotator.Pitch = 0.0f;
		AddMovementInput(rotator.Vector(), value);

	}

	// animInst->PlaySlotAnimation(animSequence,"Slime_Anim_Armature_ForwardMove");
}

void AQPCharacter::QP_MoveRight(float value)
{
	qp_rightV = value;
	if (value != 0) {
		
		FRotator rotator = GetControlRotation();
		rotator.Roll = 0.0f;
		rotator.Pitch = 0.0f;

		FVector rightVector = FRotationMatrix(rotator).GetScaledAxis(EAxis::Y);
		AddMovementInput(rightVector, value);
	}
}

//前行的切换
void AQPCharacter::QP_Sneak() {
	qp_isRun = !qp_isRun;
	if (qp_isRun) {
		QP_SneakStart();
	}
	else {
		QP_SneakEnd();
	}
}
void AQPCharacter::QP_SneakStart(){
	qp_maxSpeed = qp_sneakSpeed;
	qp_movementC->MaxAcceleration = qp_sneakMaxAcceleration;
}
void AQPCharacter::QP_SneakEnd(){
	qp_maxSpeed = qp_walkSpeed;
	qp_movementC->MaxAcceleration = qp_walkMaxAcceleration;
}

void AQPCharacter::QP_Run() 
{
	qp_isRun = !qp_isRun;
	if (qp_isRun) {
		QP_RunStart();
	}
	else {
		QP_RunEnd();
	}
}

 void AQPCharacter::QP_RunStart()
 {
	 qp_maxSpeed = qp_runSpeed;
	 qp_movementC->MaxAcceleration = qp_runMaxAcceleration;
	 //qp_movementC->MaxWalkSpeed = qp_maxSpeed;

 }
 void AQPCharacter::QP_RunEnd()
 {
	 qp_maxSpeed = qp_walkSpeed;
	 qp_movementC->MaxAcceleration = qp_walkMaxAcceleration;
	 //qp_movementC->MaxWalkSpeed = qp_maxSpeed;


 }
 //脱离玩家控制时调用
 void AQPCharacter::QP_PlayerExit() {
	 qp_movementMode = GetCharacterMovement()->MovementMode;

	 qp_isExit = true;
	 QP_Reset();
	 
 }
 //进入玩家控制时调用
 void AQPCharacter::QP_PlayerEnter() {
	 if (!qp_isExit) {
		 return;
	 }
	 qp_isExit = false;
	 QP_ReReset();
 }
 void AQPCharacter::QP_Reset() {
	 qp_isReset = true;
	 qp_isFixedCameraLast = qp_isFixedCamera;
	 qp_isRunLast = qp_isRun;
	 if (qp_isFixedCamera) {
		 QP_FixedCamera();
	 }
	 if (qp_isRun) {
		 QP_Run();
	 }
	 if (qp_characterData->QP_GetFString("characterAttack") == "start") {
		 QP_AttackEnd();
	 }
 }
void AQPCharacter::QP_ReReset() {
	if (!qp_isReset) {
		return;
	}
	qp_isReset = false;

	if (qp_isFixedCameraLast) {
		QP_FixedCamera();
	}
	if (qp_isRunLast) {
		QP_Run();
	}
	qp_isFixedCameraLast = false;
	qp_isRunLast = false;
 }
//相机固定与自由的切换
 void AQPCharacter::QP_FixedCamera()
 {
	 qp_isFixedCamera = !qp_isFixedCamera;
	 qp_movementC->bOrientRotationToMovement = !qp_isFixedCamera;
	 qp_movementC->bUseControllerDesiredRotation = qp_isFixedCamera;

	 if (qp_isFixedCamera) {
		 QP_FixedCameraStart();
	 }
	 else {
		 QP_FixedCameraEnd();
	 }
 }

 void AQPCharacter::QP_FixedCameraStart()
 {
	 

 }
 void AQPCharacter::QP_FixedCameraEnd()
 {
 }

//按下鼠标右键
 void AQPCharacter::QP_AttackTwoStart()
 {
 }
//松开鼠标右键
 void AQPCharacter::QP_AttackTwoEnd()
 {
 }

 void AQPCharacter::QP_JumpStart()
 {
	 //如果是真的话，角色跳跃
	 bPressedJump = true;
	 qp_characterData->QP_AddFString("characterJump", "start");

 }
 void AQPCharacter::QP_JumpEnd()
 {
	 //如果是假的话，结束跳跃
	 bPressedJump = false;
	 //GLog->Log("jumpEnd");

 }

 void AQPCharacter::Landed(const FHitResult& Hit) {
	 Super::Landed(Hit);
	 qp_characterData->QP_AddFString("characterJump", "end");
	 qp_characterData->QP_AddFString("characterFall", "notPlay");

 }


 void AQPCharacter::QP_AttackStart()
 {
	 qp_characterData->QP_AddFString("characterAttack", "start");

 }

 void AQPCharacter::QP_AttackEnd()
 {
	 qp_characterData->QP_AddFString("characterAttack", "end");

 }

 void AQPCharacter::QP_ChangeCharacter()
 {
	 //qp_isFixedCameraLast = qp_isFixedCamera;

	 //QP_Reset();
	 //AutoPossessPlayer = EAutoReceiveInput::Type::Disabled;

	 //UQPGIM_Character::qp_staticObject->QP_GetCharacter("FlySlime")->AutoPossessPlayer = EAutoReceiveInput::Type::Player0;
	 UQPGIM_Character::qp_staticObject->QP_Possess(GetController(), qp_changeCharacterName, qp_unchangeMovementMode);
	 //->Possess(QP_GetCharacter());
 //GetController()->Possess(UQPGIM_Character::qp_staticObject->QP_GetCharacter("FlySlime"));
 //GetController()->SetPawn(UQPGIM_Character::qp_staticObject->QP_GetCharacter("FlySlime"));
 }
 //攻击开始
 void  AQPCharacter::QP_mouseWheelUp() {

	 if (qp_springArm->TargetArmLength > qp_minCameraLength) {
		 qp_springArm->TargetArmLength = qp_springArm->TargetArmLength - qp_changeCameraLengthSpeed * GetWorld()->GetDeltaSeconds();
		 if (qp_springArm->TargetArmLength < qp_minCameraLength) {
			 qp_springArm->TargetArmLength = qp_minCameraLength;
		 }
		 if (qp_springArm->TargetArmLength < qp_visibleCameraMinLength) {
			 GetMesh()->SetVisibility(false);
		 }
	 }
	 else {
		 qp_isAutoCameraLength = false;

	 }
 }
 //攻击结束
 void  AQPCharacter::QP_mouseWheelDown() {
	 if (qp_springArm->TargetArmLength < qp_maxCameraLength) {
		 qp_springArm->TargetArmLength = qp_springArm->TargetArmLength + qp_changeCameraLengthSpeed * GetWorld()->GetDeltaSeconds();
		 if (qp_springArm->TargetArmLength > qp_maxCameraLength) {
			 qp_springArm->TargetArmLength = qp_maxCameraLength;
		 }
		 if (qp_springArm->TargetArmLength > qp_visibleCameraMinLength) {
			 GetMesh()->SetVisibility(true);
		 }
	 }
	 else {
		 qp_isAutoCameraLength = false;

	 }
 }

 void AQPCharacter::QP_AnimNotifyEvent(UQPData* data) {
	 //GLog->Log("QP_AnimNotifyFire");
	
	if (data->QP_IsChange<FName, bool>(QP_AnimNotifyFireName, EQPDataValueType::FNAME)) {
		QP_Fire();
	}
	else if (data->QP_IsChange<FName, bool>(QP_AnimNotifyJunmEndName, EQPDataValueType::FNAME)) {
		if (qp_characterData->QP_GetFString("characterAttack") == "start") {
			qp_characterData->QP_AddFString("characterAttack", "start");
		}
	}
	 
 }

 void AQPCharacter::QP_Fire() {
	 
 }