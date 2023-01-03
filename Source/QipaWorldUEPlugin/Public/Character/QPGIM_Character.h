﻿// QIPAWORLD

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Character/QPCharacter.h"

#include "QPGIM_Character.generated.h"

/**
 * 
 */
UCLASS()
class QIPAWORLDUEPLUGIN_API UQPGIM_Character : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	TMap<FString, ACharacter*> qp_characterMap;
	static UQPGIM_Character* QP_UQPGIM_Character;
	//Slime.Slime'
	FString qp_assetPath = "/Script/QipaWorld3DUE.QPDA_Character'/Game/QipaWorld3D/Data/Character/";
	/*UFUNCTION(BlueprintCallable,Category = "QipaWorld|Data")
	void QP_Init();*/

	virtual bool ShouldCreateSubsystem(UObject* Outer) const;

	/** Implement this for initialization of instances of the system */
	virtual void Initialize(FSubsystemCollectionBase& Collection);

	/** Implement this for deinitialization of instances of the system */
	virtual void Deinitialize();
	/**向管理器里添加场景里已有对象的引用
	* @param qp_name - 对象类型。
	* @param c - 对象。
	*/
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|Character")
		void QP_AddCharacter(FString qp_name, ACharacter* c);

	UFUNCTION(BlueprintCallable, Category = "QipaWorld|Character")
		void QP_Possess(AController* controller, FString qp_name);

	/**通过类型获得场景里对象的引用
	* @param qp_name - 对象类型。
	*/
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|Character")
		ACharacter* QP_GetCharacter(FString qp_name);

	/**通过类型在对象池里面取出对象
	* @param qp_name - 对象类型。
	*/
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|Character")
		ACharacter* QP_GetNewCharacter(FString qp_name, FTransform T);

	/**把用不到的对象放到这个里面，里面是个数组。
	* @param key - 对象类型。
	* @param T - 位置和旋转。
	* @param character - 对象。
	* @warning 每个类型可以存储多个对象。
	*/
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|Character")
	void QP_CollectCharacter(FString key, ACharacter* character);
	/**用一个新的角色替换现在的角色。
	* @param collkey - 回收的对象类型。
	* @param character - 回收的对象。
	* @param qp_name - 你想换取的对象类型。
	* @param T - 位置和旋转。
	* @warning 每个类型可以存储多个对象。
	*/
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|Character")
		ACharacter* QP_ChangeMainCharacter(FString collkey, ACharacter* character,FString qp_name, FTransform T);
};
