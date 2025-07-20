﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UObject/NoExportTypes.h"

#include "QPUtil.h"
#include "string"
#include <functional>
#include "Misc/AES.h"
#include "QPData.generated.h"

class UQPData;
DECLARE_MULTICAST_DELEGATE_OneParam(QP_DataDelegate, UQPData*);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FQP_DataDynamicDelegate, UQPData*,QPData);


UENUM(BlueprintType, Category = "QipaWorld|Data")
enum class EQPDataBroadcastType :uint8
{
	DEFAULT              UMETA(DisplayName = "default"),
	SYNC              UMETA(DisplayName = "sync"),
	NONE            UMETA(DisplayName = "none"),
	NONE_EX            UMETA(DisplayName = "noneEx"), //none broadcast and not add to changemap

};

UENUM(BlueprintType, Category = "QipaWorld|Data")
enum class EQPDataChangeType :uint8
{
	NOT               UMETA(DisplayName = "Not"),
	INIT               UMETA(DisplayName = "Init"),
	ADD               UMETA(DisplayName = "Add"),
	CHANGE               UMETA(DisplayName = "Change"),
	REMOVE               UMETA(DisplayName = "Remove"),
	CLEAR               UMETA(DisplayName = "Clear"),
};

class IQPBaseData {
public:
	EQPDataValueType qp_valueType;
	EQPDataKeyType qp_keyType;
	bool qp_isRemoveLog = false;
	bool qp_isSave = true;

	virtual void QP_RestChangeMap() = 0;
	virtual void QP_GetMapKeys(TArray<FName>& qp_outArray) = 0;
	virtual void QP_GetMapKeys(TArray<FString>& qp_outArray) = 0;
	virtual void QP_GetMapKeys(TArray<int32>& qp_outArray) = 0;
	virtual void Serialize(FArchive& Ar)=0;

	IQPBaseData(EQPDataValueType vt, EQPDataKeyType kt)
		: qp_valueType(vt)
		, qp_keyType(kt)
	{
	}
	virtual ~IQPBaseData() = default;
};
template<typename K, typename V>
class QPBaseData: public IQPBaseData {
public:
	using IQPBaseData::IQPBaseData;
	
	TMap<K, EQPDataChangeType> qp_changeMap;
	TMap<K, V> qp_ValueMap;
	
	virtual ~QPBaseData()override {
		if constexpr (std::is_same<V, UQPData*>::value) {
			for (auto v : qp_ValueMap) {
				v.Value->RemoveFromRoot();
			}
		}
	};
	TMap<K, V>& QP_GetTMap() {
		return qp_ValueMap;
	}
	virtual void Serialize(FArchive& Ar) override {
		//Ar << qp_valueType;
		//Ar << qp_keyType;
		if constexpr (!std::is_same<K, void*>::value) {

			if constexpr (!std::is_same<V, void*>::value || !std::is_same<V, UQPData*>::value || !std::is_same<V, std::string>::value || !std::is_same<V, UObject*>::value) {
				Ar << qp_isRemoveLog;
				Ar << qp_isSave;
				Ar << qp_ValueMap;
			}
		}
	};

	virtual void QP_GetMapKeys(TArray<FName>& qp_outArray) override {
		
		if constexpr (std::is_same<K, FName>::value) {
			for (auto v : qp_ValueMap) {
				qp_outArray.Add(v.Key);
			}
		}
		else if constexpr (std::is_same<K, int32>::value) {
			for (auto v : qp_ValueMap) {
				qp_outArray.Add(FName(*FString::FromInt(v.Key)));
			}
		}
	}
	virtual void QP_GetMapKeys(TArray<FString>& qp_outArray) override {
		
		if constexpr (std::is_same<K, FName>::value || std::is_same<K, int32>::value) {
			for (auto v : qp_ValueMap) {
				qp_outArray.Emplace(LexToString(v.Key));
			}
		}
	}
	virtual void QP_GetMapKeys(TArray<int32>& qp_outArray) override {
		if constexpr (std::is_same<K, int32>::value) {
			for (auto v : qp_ValueMap) {
				qp_outArray.Emplace(v.Key);
			}
		}

	}
	virtual void QP_RestChangeMap()override {
		qp_changeMap.Reset();
	}

	inline void QP_AddValue(const K& k, const V& v, EQPDataBroadcastType bType) {
		if (bType != EQPDataBroadcastType::NONE_EX) {

			if (qp_ValueMap.Contains(k)) {
				qp_changeMap.Emplace(k, EQPDataChangeType::CHANGE);
			}
			else {
				qp_changeMap.Emplace(k, EQPDataChangeType::ADD);
			}
		}
		qp_ValueMap.Emplace(k, v);

	}
	inline V& QP_GetValue(const K& k) {
		return qp_ValueMap.FindOrAdd(k);
	}
	
	inline bool QP_Contains(const K& k) {
		return qp_ValueMap.Contains(k);
	}
	/*inline bool QP_SetRemoveLog(bool b) {
		qp_isRemoveLog = b;
	}*/
	inline bool QP_Remove(const K& k, EQPDataBroadcastType bType) {

		if (qp_ValueMap.Contains(k)) {
			if (bType != EQPDataBroadcastType::NONE_EX) {
				qp_changeMap.Emplace(k, EQPDataChangeType::REMOVE);
			}
			qp_ValueMap.Remove(k);
			return true;
		}
		else {
			if (qp_isRemoveLog) {
				UQPUtil::QP_LOG_EX<K>("QPBaseData QP_Remove is not have key ", k);
			}

			return false;
		}
	}
	inline void QP_Clear(EQPDataBroadcastType bType) {
		//if (qp_ValueMap.Num() > 0) {
			for (auto v : qp_ValueMap) {
				if (bType != EQPDataBroadcastType::NONE_EX) {
					qp_changeMap.Emplace(v.Key, EQPDataChangeType::CLEAR);
				}
				qp_ValueMap.Reset();
				return;
			}
		//}
	}

	inline bool QP_IsChange(const K& k) {
		return qp_changeMap.Contains(k);
	}

	inline EQPDataChangeType QP_GetChangeType(const K& k) {
		return qp_changeMap.FindOrAdd(k);
	}
	void QP_LoopData(std::function<void(K&,V&)> func) {
		for (auto& data: qp_ValueMap)
		{
			func(data.Key,data.Value);
		}
	}
};


UENUM(BlueprintType, Category = "QipaWorld|Data")
enum class EQPDataKeyType :uint8
{
	INT32               UMETA(DisplayName = "Int32"),
	FNAME               UMETA(DisplayName = "FName"),
	//FSTRING               UMETA(DisplayName = "FString"),
	VOID               UMETA(DisplayName = "Void*"),

};

UENUM(BlueprintType, Category = "QipaWorld|Data")
enum class EQPDataValueType :uint8
{
	INT32              UMETA(DisplayName = "Int32"),
	FNAME              UMETA(DisplayName = "FName"),
	FSTRING            UMETA(DisplayName = "FString"),
	UQPDATA            UMETA(DisplayName = "UQPData"),
	UOBJECT            UMETA(DisplayName = "UObject"),
	FLOAT              UMETA(DisplayName = "float"),
	BOOL               UMETA(DisplayName = "bool"),
	FTEXT              UMETA(DisplayName = "FText"),
	TARRAY               UMETA(DisplayName = "TArray"),
	FVECTOR               UMETA(DisplayName = "FVector"),
	FROTATOR               UMETA(DisplayName = "FRotator"),
	FTRANSFORM              UMETA(DisplayName = "FTransform"),
	FQUAT              UMETA(DisplayName = "FQuat"),

	VOID               UMETA(DisplayName = "void*"),
	INT8               UMETA(DisplayName = "int8"),
	INT16               UMETA(DisplayName = "int16"),
	INT64               UMETA(DisplayName = "int64"),
	UINT8               UMETA(DisplayName = "uint8"),
	UINT16               UMETA(DisplayName = "uint16"),
	UINT32               UMETA(DisplayName = "uint32"),
	UINT64               UMETA(DisplayName = "uint64"),
	DOUBLE               UMETA(DisplayName = "double"),
	STRING               UMETA(DisplayName = "string"),

};




//USTRUCT(BlueprintType, Category = "QipaWorld|Data")
//struct FChangeConfig
//{
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QipaWorld|Data")
//	EQPDataChangeType qp_changeType;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QipaWorld|Data")
//	FString qp_valueType;
//};
/**
 * 数据类，自动发送改动消息。----
 */
UCLASS(Blueprintable,Category = "QipaWorld|Data")

class QIPAWORLDUEPLUGIN_API UQPData : public UObject
{
	GENERATED_BODY()
public:
	TMap<int16, void*> qp_ValueMap;
	//TArray<int16> qp_keys;
	/** 这个是C++用的代理，---
	 *把你监听这个数据的方法绑定上，数据改变时会自动调用-----
	 *数据内容包括是否是初始化，更改的内容的key，以及数据本身。----
	 */
	QP_DataDelegate qp_dataDelegate;
	/** 这个是蓝图中用的代理，因为速度很慢，所以不推荐C++中使用----
	 *把你监听这个数据的方法绑定上，数据改变时会自动调用----
	 *数据内容包括是否是初始化，更改的内容的key，以及数据本身。----
	 */
	UPROPERTY(Transient ,BlueprintAssignable, Category = "QipaWorld|QPData")
	FQP_DataDynamicDelegate qp_dataDynamicDelegate;

	//FName qp_dataName = "No settings";
	
	bool qp_isAddToManager = false;

	virtual void Serialize(FArchive& Ar) override;

	virtual void BeginDestroy() override;

	inline int16 QP_EncodeKey(EQPDataKeyType keyE, EQPDataValueType valueE)
	{
		return (static_cast<int8>(keyE)) | (static_cast<int8>(valueE) << 8);
	} 

	inline void QP_DecodeKey(int16 Encoded, EQPDataKeyType& OutKey, EQPDataValueType& OutValue)
	{
		OutKey = static_cast<EQPDataKeyType>(Encoded & 0xFF);       // 低8位
		OutValue = static_cast<EQPDataValueType>((Encoded >> 8) & 0xFF); // 高8位
	}

	inline UQPData* QP_Init() {
		AddToRoot();
		return this;
	}
	

	inline void QP_SetIsSave(EQPDataKeyType keyE, EQPDataValueType valueE, bool b);
	//发送事件----
	void QP_DelegateBroadcast();
	//发送事件----
	void QP_DynamicDelegateBroadcast();
	// 发送所有事件----
	void QP_BroadcastAll();

	//UQPGameInstanceDataManager* QP_GetDataManager();
	//把事件添加到管理器，下一帧发送----
	void QP_AddBroadcastToDataManager();
	//把事件添加到管理器，下一帧发送----
	void QP_RemoveBroadcastToDataManager();
	//如果需要立刻发送，则立刻发送事件。----
	void QP_needSyncBroadcast(EQPDataBroadcastType bType);
	

	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_BroadcastNow();


	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	bool QP_SaveData(const FString& name);

	//UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	//bool QP_SaveDataByName(const FString& name);

	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	bool QP_LoadData(const FString& name);
	
	

	//UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	//bool QP_SaveDataByName(const FString& name);


	//UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	//bool QP_SaveDataByName(const FString& name);
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")

	bool QP_SaveDataFAES(const FString& name, const FString& FAES_key);
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	bool QP_LoadDataFAES(const FString& name, const FString& FAES_key);

	//FString qp_saveName;
	void QP_AsyncSaveData(const FString& name, const FString& FAES_key, std::function<void(bool /*bSuccess*/)> Callback = nullptr);

	
	void QP_AsyncLoadData(const FString& name, const FString& FAES_key, std::function<void(bool /*bSuccess*/)> Callback = nullptr);


	template<typename K, typename V>
	inline bool QP_IsChange(const K& k,EQPDataValueType vt, EQPDataKeyType kt = EQPDataKeyType::FNAME ) {
		QP_CheckQPBaseData<K, V>(vt, kt);
		return ((QPBaseData<K, V>*)qp_ValueMap[QP_EncodeKey(kt, vt)])->QP_IsChange(k);
		
	}
	template<typename K, typename V>
	inline EQPDataChangeType QP_GetChangeType(const K& k,EQPDataValueType vt, EQPDataKeyType kt = EQPDataKeyType::FNAME ) {
		QP_CheckQPBaseData<K, V>(vt, kt);
		return ((QPBaseData<K, V>*)qp_ValueMap[QP_EncodeKey(kt, vt)])->QP_GetChangeType(k);
		
	}

	template<typename K, typename V>
	inline void QP_CheckQPBaseData(EQPDataValueType vt, EQPDataKeyType kt = EQPDataKeyType::FNAME) {
		if (!qp_ValueMap.Contains(QP_EncodeKey(kt, vt))) {
			qp_ValueMap.Emplace(QP_EncodeKey(kt, vt), new QPBaseData<K, V>(vt, kt));
		}
	}
	
	template<typename K, typename V>
	void QP_AddValue(const K& k, const V& v, EQPDataValueType vt, EQPDataKeyType kt = EQPDataKeyType::FNAME, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT)
	{
		QP_CheckQPBaseData<K, V>(vt, kt); 
		if constexpr (std::is_same<V, UQPData*>::value) {
				
			if (((QPBaseData<K, V>*)qp_ValueMap[QP_EncodeKey(kt, vt)])->QP_Contains(k)) {
				UQPUtil::QP_LOG_EX<K>("QPData QP_AddUQPData do not repeat add key ! ", k);
				return;
			}
			if (v == nullptr)
			{
				((QPBaseData<K, V>*)qp_ValueMap[QP_EncodeKey(kt, vt)])->QP_AddValue(k, NewObject<UQPData>()->QP_Init(), bType);
				QP_needSyncBroadcast(bType);
				return;
			}
		}
		
		((QPBaseData<K, V>*)qp_ValueMap[QP_EncodeKey(kt, vt)])->QP_AddValue(k, v, bType);
		QP_needSyncBroadcast(bType); 
	}

	template<typename K, typename V>
	V& QP_GetValue(const K& k, EQPDataValueType vt, EQPDataKeyType kt = EQPDataKeyType::FNAME)
	{
		QP_CheckQPBaseData<K, V>(vt, kt); 
		if constexpr (std::is_same<V, UQPData*>::value) 
		{
			if (!((QPBaseData<K, V>*)qp_ValueMap[QP_EncodeKey(kt, vt)])->QP_Contains(k))
			{
				QP_AddValue<K, V>(k, nullptr, vt, kt);
			}
		}
		return ((QPBaseData<K, V>*)qp_ValueMap[QP_EncodeKey(kt, vt)])->QP_GetValue(k);
	}
	


	template<typename K, typename V>
	bool QP_Contains(const K& k, EQPDataValueType vt, EQPDataKeyType kt = EQPDataKeyType::FNAME) {
		QP_CheckQPBaseData<K, V>(vt, kt);
		return ((QPBaseData<K, V>*)qp_ValueMap[QP_EncodeKey(kt, vt)])->QP_Contains(k);
	}
	
	template<typename K, typename V>
	bool QP_RemoveValue(const K& k, EQPDataValueType vt, EQPDataKeyType kt = EQPDataKeyType::FNAME, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT)
	{
		QP_CheckQPBaseData<K, V>(vt, kt);
		
		int16 mK = QP_EncodeKey(kt, vt);
		if (((QPBaseData<K, V>*)qp_ValueMap[mK])->QP_Contains(k)) {
			if constexpr (std::is_same<V, UQPData*>::value) 
			{
				((QPBaseData<K, V>*)qp_ValueMap[mK])->QP_GetValue(k)->RemoveFromRoot();
			}
			((QPBaseData<K, V>*)qp_ValueMap[mK])->QP_Remove(k, bType);
			QP_needSyncBroadcast(bType);
			return true;
		}

		UQPUtil::QP_LOG_EX<K>("QPData QP_Remove is not have key ", k);
		return false;

	}
	template<typename K, typename V>
	void QP_ClearValue(EQPDataValueType vt, EQPDataKeyType kt = EQPDataKeyType::FNAME, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT) {
		QP_CheckQPBaseData<K, V>(vt, kt);
		int16 mK = QP_EncodeKey(kt, vt);
		if constexpr (std::is_same<V, UQPData*>::value) {
			for (auto v : ((QPBaseData<K, V>*)qp_ValueMap[mK])->qp_ValueMap) {
				v.Value->RemoveFromRoot();
			}
		}
		((QPBaseData<K, V>*)qp_ValueMap[mK])->QP_Clear( bType);
		QP_needSyncBroadcast(bType);
	}
	template<typename K, typename V>
	QPBaseData<K,V>* QP_GetBaseData(EQPDataValueType vt, EQPDataKeyType kt = EQPDataKeyType::FNAME) {
		QP_CheckQPBaseData<K, V>(vt, kt);
		return  ((QPBaseData<K, V>*)qp_ValueMap[QP_EncodeKey(kt, vt)]);
	}

	template<typename K, typename V>
	TMap<K, EQPDataChangeType>& QP_GetChangeMap(EQPDataValueType vt, EQPDataKeyType kt = EQPDataKeyType::FNAME) {
		QP_CheckQPBaseData<K, V>(vt, kt);
		return  ((QPBaseData<K, V>*)qp_ValueMap[QP_EncodeKey(kt, vt)])->qp_changeMap;
	}


//------------auto generator start

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_Adddouble(FName  key, double v, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_Containsdouble(FName  key);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
double QP_Getdouble(FName  key);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
bool QP_Removedouble(FName  key, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_Cleardouble( EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);


UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_AdddoubleExI(int32  key, double v, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_ContainsdoubleExI(int32  key);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
double QP_GetdoubleExI(int32  key);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
bool QP_RemovedoubleExI(int32  key, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_CleardoubleExI( EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);


UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_AdddoubleExO(UObject*  key, double v, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_ContainsdoubleExO(UObject*  key);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
double QP_GetdoubleExO(UObject*  key);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
bool QP_RemovedoubleExO(UObject*  key, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_CleardoubleExO( EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);


UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_Addint64(FName  key, int64 v, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_Containsint64(FName  key);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
int64 QP_Getint64(FName  key);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
bool QP_Removeint64(FName  key, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_Clearint64( EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);


UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_Addint64ExI(int32  key, int64 v, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_Containsint64ExI(int32  key);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
int64 QP_Getint64ExI(int32  key);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
bool QP_Removeint64ExI(int32  key, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_Clearint64ExI( EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);


UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_Addint64ExO(UObject*  key, int64 v, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_Containsint64ExO(UObject*  key);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
int64 QP_Getint64ExO(UObject*  key);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
bool QP_Removeint64ExO(UObject*  key, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_Clearint64ExO( EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);


UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_AddFVector(FName  key, const FVector& v, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_ContainsFVector(FName  key);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
FVector& QP_GetFVector(FName  key);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
bool QP_RemoveFVector(FName  key, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_ClearFVector( EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);


UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_AddFVectorExI(int32  key, const FVector& v, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_ContainsFVectorExI(int32  key);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
FVector& QP_GetFVectorExI(int32  key);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
bool QP_RemoveFVectorExI(int32  key, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_ClearFVectorExI( EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);


UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_AddFVectorExO(UObject*  key, const FVector& v, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_ContainsFVectorExO(UObject*  key);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
FVector& QP_GetFVectorExO(UObject*  key);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
bool QP_RemoveFVectorExO(UObject*  key, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_ClearFVectorExO( EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);


UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_AddFName(FName  key, const FName v, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_ContainsFName(FName  key);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
FName& QP_GetFName(FName  key);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
bool QP_RemoveFName(FName  key, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_ClearFName( EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);


UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_AddFNameExI(int32  key, const FName v, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_ContainsFNameExI(int32  key);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
FName& QP_GetFNameExI(int32  key);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
bool QP_RemoveFNameExI(int32  key, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_ClearFNameExI( EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);


UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_AddFNameExO(UObject*  key, const FName v, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_ContainsFNameExO(UObject*  key);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
FName& QP_GetFNameExO(UObject*  key);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
bool QP_RemoveFNameExO(UObject*  key, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_ClearFNameExO( EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);


UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_AddFText(FName  key, const FText& v, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_ContainsFText(FName  key);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
FText& QP_GetFText(FName  key);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
bool QP_RemoveFText(FName  key, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_ClearFText( EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);


UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_AddFTextExI(int32  key, const FText& v, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_ContainsFTextExI(int32  key);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
FText& QP_GetFTextExI(int32  key);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
bool QP_RemoveFTextExI(int32  key, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_ClearFTextExI( EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);


UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_AddFTextExO(UObject*  key, const FText& v, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_ContainsFTextExO(UObject*  key);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
FText& QP_GetFTextExO(UObject*  key);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
bool QP_RemoveFTextExO(UObject*  key, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_ClearFTextExO( EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);


UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_AddFString(FName  key, const FString& v, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_ContainsFString(FName  key);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
FString& QP_GetFString(FName  key);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
bool QP_RemoveFString(FName  key, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_ClearFString( EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);


UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_AddFStringExI(int32  key, const FString& v, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_ContainsFStringExI(int32  key);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
FString& QP_GetFStringExI(int32  key);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
bool QP_RemoveFStringExI(int32  key, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_ClearFStringExI( EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);


UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_AddFStringExO(UObject*  key, const FString& v, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_ContainsFStringExO(UObject*  key);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
FString& QP_GetFStringExO(UObject*  key);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
bool QP_RemoveFStringExO(UObject*  key, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_ClearFStringExO( EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);


UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_Addbool(FName  key, bool v, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_Containsbool(FName  key);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_Getbool(FName  key);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
bool QP_Removebool(FName  key, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_Clearbool( EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);


UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_AddboolExI(int32  key, bool v, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_ContainsboolExI(int32  key);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_GetboolExI(int32  key);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
bool QP_RemoveboolExI(int32  key, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_ClearboolExI( EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);


UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_AddboolExO(UObject*  key, bool v, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_ContainsboolExO(UObject*  key);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_GetboolExO(UObject*  key);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
bool QP_RemoveboolExO(UObject*  key, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_ClearboolExO( EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);


UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_Addfloat(FName  key, float v, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_Containsfloat(FName  key);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
float QP_Getfloat(FName  key);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
bool QP_Removefloat(FName  key, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_Clearfloat( EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);


UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_AddfloatExI(int32  key, float v, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_ContainsfloatExI(int32  key);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
float QP_GetfloatExI(int32  key);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
bool QP_RemovefloatExI(int32  key, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_ClearfloatExI( EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);


UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_AddfloatExO(UObject*  key, float v, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_ContainsfloatExO(UObject*  key);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
float QP_GetfloatExO(UObject*  key);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
bool QP_RemovefloatExO(UObject*  key, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_ClearfloatExO( EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);


UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_Addint32(FName  key, int32 v, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_Containsint32(FName  key);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
int32 QP_Getint32(FName  key);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
bool QP_Removeint32(FName  key, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_Clearint32( EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);


UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_Addint32ExI(int32  key, int32 v, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_Containsint32ExI(int32  key);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
int32 QP_Getint32ExI(int32  key);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
bool QP_Removeint32ExI(int32  key, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_Clearint32ExI( EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);


UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_Addint32ExO(UObject*  key, int32 v, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_Containsint32ExO(UObject*  key);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
int32 QP_Getint32ExO(UObject*  key);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
bool QP_Removeint32ExO(UObject*  key, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_Clearint32ExO( EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);


UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_AddUObject(FName  key, UObject* v, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_ContainsUObject(FName  key);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
UObject* QP_GetUObject(FName  key);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
bool QP_RemoveUObject(FName  key, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_ClearUObject( EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);


UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_AddUObjectExI(int32  key, UObject* v, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_ContainsUObjectExI(int32  key);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
UObject* QP_GetUObjectExI(int32  key);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
bool QP_RemoveUObjectExI(int32  key, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_ClearUObjectExI( EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);


UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_AddUObjectExO(UObject*  key, UObject* v, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_ContainsUObjectExO(UObject*  key);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
UObject* QP_GetUObjectExO(UObject*  key);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
bool QP_RemoveUObjectExO(UObject*  key, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_ClearUObjectExO( EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);


UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_AddUQPData(FName  key, UQPData* v, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_ContainsUQPData(FName  key);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
UQPData* QP_GetUQPData(FName  key);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
bool QP_RemoveUQPData(FName  key, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_ClearUQPData( EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);


UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_AddUQPDataExI(int32  key, UQPData* v, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_ContainsUQPDataExI(int32  key);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
UQPData* QP_GetUQPDataExI(int32  key);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
bool QP_RemoveUQPDataExI(int32  key, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_ClearUQPDataExI( EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);


UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_AddUQPDataExO(UObject*  key, UQPData* v, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_ContainsUQPDataExO(UObject*  key);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
UQPData* QP_GetUQPDataExO(UObject*  key);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
bool QP_RemoveUQPDataExO(UObject*  key, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_ClearUQPDataExO( EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);


UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_AddFRotator(FName  key, const FRotator& v, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_ContainsFRotator(FName  key);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
FRotator& QP_GetFRotator(FName  key);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
bool QP_RemoveFRotator(FName  key, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_ClearFRotator( EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);


UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_AddFRotatorExI(int32  key, const FRotator& v, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_ContainsFRotatorExI(int32  key);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
FRotator& QP_GetFRotatorExI(int32  key);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
bool QP_RemoveFRotatorExI(int32  key, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_ClearFRotatorExI( EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);


UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_AddFRotatorExO(UObject*  key, const FRotator& v, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_ContainsFRotatorExO(UObject*  key);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
FRotator& QP_GetFRotatorExO(UObject*  key);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
bool QP_RemoveFRotatorExO(UObject*  key, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_ClearFRotatorExO( EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);


UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_AddFTransform(FName  key, const FTransform& v, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_ContainsFTransform(FName  key);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
FTransform& QP_GetFTransform(FName  key);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
bool QP_RemoveFTransform(FName  key, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_ClearFTransform( EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);


UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_AddFTransformExI(int32  key, const FTransform& v, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_ContainsFTransformExI(int32  key);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
FTransform& QP_GetFTransformExI(int32  key);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
bool QP_RemoveFTransformExI(int32  key, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_ClearFTransformExI( EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);


UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_AddFTransformExO(UObject*  key, const FTransform& v, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_ContainsFTransformExO(UObject*  key);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
FTransform& QP_GetFTransformExO(UObject*  key);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
bool QP_RemoveFTransformExO(UObject*  key, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_ClearFTransformExO( EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);


UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_AddFQuat(FName  key, const FQuat& v, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_ContainsFQuat(FName  key);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
FQuat& QP_GetFQuat(FName  key);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
bool QP_RemoveFQuat(FName  key, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_ClearFQuat( EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);


UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_AddFQuatExI(int32  key, const FQuat& v, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_ContainsFQuatExI(int32  key);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
FQuat& QP_GetFQuatExI(int32  key);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
bool QP_RemoveFQuatExI(int32  key, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_ClearFQuatExI( EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);


UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_AddFQuatExO(UObject*  key, const FQuat& v, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_ContainsFQuatExO(UObject*  key);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
FQuat& QP_GetFQuatExO(UObject*  key);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
bool QP_RemoveFQuatExO(UObject*  key, EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_ClearFQuatExO( EQPDataBroadcastType bType = EQPDataBroadcastType::DEFAULT);


//------------auto generator end


};