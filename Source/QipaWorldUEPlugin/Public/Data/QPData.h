// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UObject/NoExportTypes.h"
//#include "list"
//#include "map"
#include "QPUtil.h"
#include "string"
//#include "vector"
#include "QPData.generated.h"

class UQPData;
DECLARE_MULTICAST_DELEGATE_OneParam(QP_DataDelegate, UQPData*);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FQP_DataDynamicDelegate, UQPData*,QPData);



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
	virtual void QP_RestChangeMap() = 0;
	virtual void QP_GetMapKeys(TArray<FName>& qp_outArray) = 0;
	virtual void QP_GetMapKeys(TArray<FString>& qp_outArray) = 0;
	virtual void QP_GetMapKeys(TArray<int32>& qp_outArray) = 0;

	virtual ~IQPBaseData() {};
};
template<typename K, typename V>
class QPBaseData:IQPBaseData {
public:
	QPBaseData(EQPDataValueType vt, EQPDataKeyType kt) {
		qp_valueType = vt;
		qp_keyType = kt;
	}
	TMap<K, EQPDataChangeType> qp_changeMap;
	TMap<K, V> qp_ValueMap;
	virtual ~QPBaseData()override {
		if constexpr (std::is_same<V, UQPData>::value) {
			for (auto v : qp_ValueMap) {
				v.Value->RemoveFromRoot();
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

	//template<typename K, typename V>
	inline void QP_AddValue(K k,const V& v) {
		if (qp_ValueMap.Contains(k)) {
			qp_changeMap.Emplace(k, EQPDataChangeType::CHANGE);
		}
		else {
			qp_changeMap.Emplace(k, EQPDataChangeType::ADD);
		}
		qp_ValueMap.Emplace(k,v);

	}
	inline V QP_GetValue(K k) {
		return qp_ValueMap.FindOrAdd(k);
	}
	 inline bool QP_Contains(K k) {
		return qp_ValueMap.Contains(k);
	}
	inline bool QP_Remove(K k,bool isLog = false) {
		if (qp_ValueMap.Contains(k)) {
			qp_changeMap.Emplace(k, EQPDataChangeType::REMOVE);
			qp_ValueMap.Remove(k);
			return true;
		}
		else {
			if (isLog) {
				UQPUtil::QP_LOG_EX<K>("QPBaseData QP_Remove is not have key ",k);
			}

			return false;
		}
	}
	inline void QP_Clear() {
		//if (qp_ValueMap.Num() > 0) {
			for (auto v : qp_ValueMap) {
				qp_changeMap.Emplace(v.Key, EQPDataChangeType::CLEAR);
				qp_ValueMap.Reset();
				return;
			}
		//}
	}

	inline bool QP_IsChange(K k) {
		return qp_changeMap.Contains(k);
	}
	inline EQPDataChangeType QP_GetChangeType(K k) {
		return qp_changeMap.FindOrAdd(k);
	}
};


UENUM(BlueprintType, Category = "QipaWorld|Data")
enum class EQPDataKeyType :uint8
{
	INT32               UMETA(DisplayName = "Init32"),
	FNAME               UMETA(DisplayName = "FName"),
	//FSTRING               UMETA(DisplayName = "FString"),
	VOID               UMETA(DisplayName = "Void*"),

};

UENUM(BlueprintType, Category = "QipaWorld|Data")
enum class EQPDataValueType :uint8
{
	INT32              UMETA(DisplayName = "Init32"),
	FNAME              UMETA(DisplayName = "FName"),
	FSTRING            UMETA(DisplayName = "FString"),
	UQPDATA            UMETA(DisplayName = "UQPData"),
	UOBJECT            UMETA(DisplayName = "UObject"),
	FLOAT              UMETA(DisplayName = "float"),
	BOOL               UMETA(DisplayName = "bool"),
	FTEXT              UMETA(DisplayName = "FText"),
	TARRAY               UMETA(DisplayName = "TArray"),
	FVECTOR               UMETA(DisplayName = "FVector"),

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


#define QP_ADDVALUE {\
	QP_CheckQPBaseData<K,V>(vt,kt);\
	if constexpr (std::is_same<V, UQPData*>::value) {\
		if (((QPBaseData<K, V>*)qp_ValueMap[kt][vt])->QP_Contains(k)) {\
			UQPUtil::QP_LOG_EX<K>("QPData QP_AddUQPData do not repeat add key ! " , k);\
		}\
		if (v == nullptr)\
		{\
			v = NewObject<UQPData>();\
			v->QP_Init();\
		}\
	}\
	((QPBaseData<K,V>*)qp_ValueMap[kt][vt])->QP_AddValue(k,v);\
	QP_needSyncBroadcast(sync);\
}\


#define QP_GETVALUE {\
	QP_CheckQPBaseData<K,V>(vt,kt);\
	if constexpr (std::is_same<V, UQPData*>::value) {\
		if ( !((QPBaseData<K, V>*)qp_ValueMap[kt][vt])->QP_Contains(k)) {\
			QP_AddValue<K,V>(k,nullptr,vt,kt); \
		}\
		return ((QPBaseData<K, V>*)qp_ValueMap[kt][vt])->QP_GetValue(k); \
	}\
	else {\
		return ((QPBaseData<K, V>*)qp_ValueMap[kt][vt])->QP_GetValue(k); \
	}\
}

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
	TMap<EQPDataKeyType, TMap<EQPDataValueType,void*>> qp_ValueMap;

	
	/** 这个是C++用的代理，---
	 *把你监听这个数据的方法绑定上，数据改变时会自动调用-----
	 *数据内容包括是否是初始化，更改的内容的key，以及数据本身。----
	 */
	QP_DataDelegate qp_dataDelegate;
	/** 这个是蓝图中用的代理，因为速度很慢，所以不推荐C++中使用----
	 *把你监听这个数据的方法绑定上，数据改变时会自动调用----
	 *数据内容包括是否是初始化，更改的内容的key，以及数据本身。----
	 */
	UPROPERTY(BlueprintAssignable, Category = "QipaWorld|QPData")
	FQP_DataDynamicDelegate qp_dataDynamicDelegate;

	//FName qp_dataName = "No settings";
	
	bool qp_isAddToManager = false;


	virtual void BeginDestroy() override;

	inline void QP_Init();
	
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
	void QP_needSyncBroadcast(bool sync);
	
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_BroadcastNow();

	template<typename K, typename V>
	inline bool QP_IsChange(K k,EQPDataValueType vt, EQPDataKeyType kt = EQPDataKeyType::FNAME ) {
		if (qp_ValueMap.FindOrAdd(kt).Contains(vt)) {
			return ((QPBaseData<K, V>*)qp_ValueMap[kt][vt])->QP_IsChange(k);
		}

		return false;
	}
	template<typename K, typename V>
	inline EQPDataChangeType QP_GetChangeType(K k,EQPDataValueType vt, EQPDataKeyType kt = EQPDataKeyType::FNAME ) {
		if (qp_ValueMap.FindOrAdd(kt).Contains(vt)) {
			return ((QPBaseData<K, V>*)qp_ValueMap[kt][vt])->QP_GetChangeType(k);
		}
		return EQPDataChangeType::NOT;
	}

	template<typename K, typename V>
	inline void QP_CheckQPBaseData(EQPDataValueType vt, EQPDataKeyType kt = EQPDataKeyType::FNAME) {
		if (!qp_ValueMap.FindOrAdd(kt).Contains(vt)) {
			qp_ValueMap[kt].Emplace(vt, new QPBaseData<K, V>(vt, kt));
		}
	}

	template<typename K, typename V>
	void QP_AddValue(K k,V v, EQPDataValueType vt, EQPDataKeyType kt = EQPDataKeyType::FNAME, bool sync = false)
	QP_ADDVALUE
		//这个传进来的是 value 是引用
	template<typename K, typename V>
	void QP_AddValueR(K k, V& v, EQPDataValueType vt, EQPDataKeyType kt = EQPDataKeyType::FNAME, bool sync = false)
	QP_ADDVALUE
	
	template<typename K, typename V>
	V QP_GetValue(K k, EQPDataValueType vt, EQPDataKeyType kt = EQPDataKeyType::FNAME)
	QP_GETVALUE

	//这个返回的是 value 是引用
	template<typename K, typename V>
	V& QP_GetValueR(K k, EQPDataValueType vt, EQPDataKeyType kt = EQPDataKeyType::FNAME)
	QP_GETVALUE

	template<typename K, typename V>
	bool QP_Contains(K k, EQPDataValueType vt, EQPDataKeyType kt = EQPDataKeyType::FNAME) {
		if (qp_ValueMap.FindOrAdd(kt).Contains(vt)) {
			return ((QPBaseData<K,V>*)qp_ValueMap[kt][vt])->QP_Contains(k);
		}
		return false;
	}

	template<typename K, typename V>
	bool QP_RemoveValue(K k, EQPDataValueType vt, EQPDataKeyType kt = EQPDataKeyType::FNAME, bool sync = false)
	{
		if (qp_ValueMap.FindOrAdd(kt).Contains(vt)) {
			if constexpr (std::is_same<V, UQPData*>::value) {
					if (((QPBaseData<K, V>*)qp_ValueMap[kt][vt])->QP_Contains(k)) {
						
						((QPBaseData<K, V>*)qp_ValueMap[kt][vt])->QP_GetValue(k)->RemoveFromRoot();
						((QPBaseData<K, V>*)qp_ValueMap[kt][vt])->QP_Remove(k); 
						QP_needSyncBroadcast(sync); 
						return true; 
					}
			}
			else {
				if (((QPBaseData<K, V>*)qp_ValueMap[kt][vt])->QP_Remove(k)) {
					QP_needSyncBroadcast(sync);
					return true;
				}
			}
		}
		
		UQPUtil::QP_LOG_EX<K>("QPData QP_Remove is not have key ", k);
		return false;
		
	}
	template<typename K, typename V>
	void QP_ClearValue( EQPDataValueType vt, EQPDataKeyType kt = EQPDataKeyType::FNAME, bool sync = false) {
		if (qp_ValueMap.FindOrAdd(kt).Contains(vt)) {
			if constexpr (std::is_same<V, UQPData*>::value) {
				for (auto v : ((QPBaseData<K, V>*)qp_ValueMap[kt][vt])->qp_ValueMap) {
					v.Value->RemoveFromRoot();
				}
			}
			((QPBaseData<K, V>*)qp_ValueMap[kt][vt])->QP_Clear();
			
		}
		QP_needSyncBroadcast(sync);
	}

	
	

	/**这个千万不要轻易调用，----
	*你直接get，没有会自动帮你创建，----
	*你要是用这个，如果之前这个key下如果有个data，会直接被替换！！！----*/
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_AddUQPData(FName key, UQPData* v = nullptr, bool sync = false);
	UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
	UQPData* QP_GetUQPData(FName key);
	UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
	bool QP_ContainsUQPData(FName key);

	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	bool QP_RemoveUQPData(FName key, bool sync = false);
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_ClearUQPData(bool sync = false);


	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_AddUQPDataExI(int32 key, UQPData* v = nullptr, bool sync = false);
	UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
	UQPData* QP_GetUQPDataExI(int32 key);
	UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
	bool QP_ContainsUQPDataExI(int32 key);
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	bool QP_RemoveUQPDataExI(int32 key, bool sync = false);
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_ClearUQPDataExI( bool sync = false);


	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_AddUQPDataExO(UObject* key, UQPData* v = nullptr, bool sync = false);
	UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
	UQPData* QP_GetUQPDataExO(UObject* key);
	UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
	bool QP_ContainsUQPDataExO(UObject* key);
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	bool QP_RemoveUQPDataExO(UObject* key, bool sync = false);
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_ClearUQPDataExO( bool sync = false);

	//------------------------------------------------------

	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_AddUObject(FName key, UObject* v, bool sync = false);

	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_Addint32(FName key, int32 v, bool sync = false);
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_Addfloat(FName key, float v, bool sync = false);
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_Addbool(FName key, bool v, bool sync = false);
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_AddFString(FName key, const FString& v, bool sync = false);
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_AddFText(FName key,const FText& v, bool sync = false);

	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_AddFName(FName key, FName v, bool sync = false);
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_AddFVector(FName key, const FVector& v, bool sync = false);

	//---------------------------------------------------------------


	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_RemoveUObject(FName key, bool sync = false);
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_Removeint32(FName key, bool sync = false);
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_Removefloat(FName key, bool sync = false);
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_Removebool(FName key, bool sync = false);
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_RemoveFString(FName key, bool sync = false);
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_RemoveFText(FName key, bool sync = false);
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_RemoveFName(FName key, bool sync = false);
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_RemoveFVector(FName key, bool sync = false);
	//UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")

	//---------------------------------------------------------------

	UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
	UObject* QP_GetUObject(FName key);
	UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
	int32 QP_Getint32(FName key);
	UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
	float QP_Getfloat(FName key);
	UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
	bool QP_Getbool(FName key);
	UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
	FString& QP_GetFString(FName key);
	UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
	FText& QP_GetFText(FName key);
	UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
	FName QP_GetFName(FName key);
	UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
	FVector& QP_GetFVector(FName key);

	//---------------------------------------------------------------

	UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
	bool QP_ContainsUObject(FName key);
	UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
	bool QP_Containsint32(FName key);
	UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
	bool QP_Containsfloat(FName key);
	UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
	bool QP_Containsbool(FName key);
	UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
	bool QP_ContainsFString(FName key);
	UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
	bool QP_ContainsFText(FName key);
	UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
	bool QP_ContainsFName(FName key);
	UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
	bool QP_ContainsFVector(FName key);



	//---------------------------------------------------------------
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_ClearUObject( bool sync = false);
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_Clearint32( bool sync = false);
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_Clearfloat( bool sync = false);
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_Clearbool( bool sync = false);
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_ClearFString( bool sync = false);
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_ClearFText( bool sync = false);

	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_ClearFName( bool sync = false);
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_ClearFVector( bool sync = false);
	//---------------------------------------------------------------


	
	// 
	//---------------------------------------------------------------

	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_AddUObjectExI(int32 key, UObject* v, bool sync = false);
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_Addint32ExI(int32 key, int32 v, bool sync = false);
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_AddfloatExI(int32 key, float v, bool sync = false);
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_AddboolExI(int32 key, bool v, bool sync = false);
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_AddFStringExI(int32 key, const FString& v, bool sync = false);
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_AddFTextExI(int32 key, const FText& v, bool sync = false);

	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_AddFNameExI(int32 key, FName v, bool sync = false);
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_AddFVectorExI(int32 key,const FVector& v, bool sync = false);

	//---------------------------------------------------------------

	/*UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_ClearUObjectExI(bool sync = false);
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_Clearint32ExI(bool sync = false);
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_ClearfloatExI(bool sync = false);
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_ClearboolExI(bool sync = false);
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_ClearFStringExI(bool sync = false);
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_ClearFTextExI(bool sync = false);
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_ClearFNameExI(bool sync = false);
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_ClearFVectorExI(bool sync = false);*/
	

		//---------------------------------------------------------------

	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_RemoveUObjectExI(int32 key, bool sync = false);
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_Removeint32ExI(int32 key, bool sync = false);
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_RemovefloatExI(int32 key, bool sync = false);
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_RemoveboolExI(int32 key, bool sync = false);
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_RemoveFStringExI(int32 key, bool sync = false);
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_RemoveFTextExI(int32 key, bool sync = false);
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_RemoveFNameExI(int32 key, bool sync = false);
	UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
	void QP_RemoveFVectorExI(int32 key, bool sync = false);
	//---------------------------------------------------------------

	UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
	UObject* QP_GetUObjectExI(int32 key);
	UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
	int32 QP_Getint32ExI(int32 key);
	UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
	float QP_GetfloatExI(int32 key);
	UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
	bool QP_GetboolExI(int32 key);
	UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
	FString& QP_GetFStringExI(int32 key);
	UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
	FText& QP_GetFTextExI(int32 key);

	UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
	FName QP_GetFNameExI(int32 key);
	UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
	FVector& QP_GetFVectorExI(int32 key);

//---------------------------------------------------------------
UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_ClearUObjectExI( bool sync = false);
UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_Clearint32ExI( bool sync = false);
UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_ClearfloatExI( bool sync = false);
UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_ClearboolExI( bool sync = false);
UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_ClearFStringExI( bool sync = false);
UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_ClearFTextExI( bool sync = false);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_ClearFNameExI( bool sync = false);
UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_ClearFVectorExI( bool sync = false);

//---------------------------------------------------------------

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_ContainsUObjectExI(int32 key);
UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_Containsint32ExI(int32 key);
UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_ContainsfloatExI(int32 key);
UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_ContainsboolExI(int32 key);
UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_ContainsFStringExI(int32 key);
UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_ContainsFTextExI(int32 key);
UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_ContainsFNameExI(int32 key);
UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_ContainsFVectorExI(int32 key);




UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_AddUObjectExO(UObject* key, UObject* v, bool sync = false);
UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_Addint32ExO(UObject* key, int32 v, bool sync = false);
UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_AddfloatExO(UObject* key, float v, bool sync = false);
UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_AddboolExO(UObject* key, bool v, bool sync = false);
UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_AddFStringExO(UObject* key, const FString& v, bool sync = false);
UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_AddFTextExO(UObject* key, const FText& v, bool sync = false);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_AddFNameExO(UObject* key, FName v, bool sync = false);
UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_AddFVectorExO(UObject* key, const FVector& v, bool sync = false);

//---------------------------------------------------------------

//UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
//void QP_ClearUObjectExO(bool sync = false);
//UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
//void QP_Clearint32ExO(bool sync = false);
//UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
//void QP_ClearfloatExO(bool sync = false);
//UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
//void QP_ClearboolExO(bool sync = false);
//UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
//void QP_ClearFStringExO(bool sync = false);
//UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
//void QP_ClearFTextExO(bool sync = false);
//UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
//void QP_ClearFNameExO(bool sync = false);
//UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
//void QP_ClearFVectorExO(bool sync = false);


//---------------------------------------------------------------

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_RemoveUObjectExO(UObject* key, bool sync = false);
UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_Removeint32ExO(UObject* key, bool sync = false);
UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_RemovefloatExO(UObject* key, bool sync = false);
UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_RemoveboolExO(UObject* key, bool sync = false);
UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_RemoveFStringExO(UObject* key, bool sync = false);
UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_RemoveFTextExO(UObject* key, bool sync = false);
UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_RemoveFNameExO(UObject* key, bool sync = false);
UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_RemoveFVectorExO(UObject* key, bool sync = false);
//---------------------------------------------------------------

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
UObject* QP_GetUObjectExO(UObject* k = nullptr);
UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
int32 QP_Getint32ExO(UObject* k = nullptr);
UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
float QP_GetfloatExO(UObject* k = nullptr);
UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_GetboolExO(UObject* k = nullptr);
UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
FString& QP_GetFStringExO(UObject* k = nullptr);
UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
FText& QP_GetFTextExO(UObject* k = nullptr);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
FName QP_GetFNameExO(UObject* k = nullptr);
UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
FVector& QP_GetFVectorExO(UObject* k = nullptr);

//---------------------------------------------------------------
UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_ClearUObjectExO( bool sync = false);
UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_Clearint32ExO( bool sync = false);
UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_ClearfloatExO( bool sync = false);
UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_ClearboolExO( bool sync = false);
UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_ClearFStringExO( bool sync = false);
UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_ClearFTextExO( bool sync = false);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_ClearFNameExO( bool sync = false);
UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_ClearFVectorExO( bool sync = false);

//---------------------------------------------------------------

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_ContainsUObjectExO(UObject* key);
UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_Containsint32ExO(UObject* key);
UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_ContainsfloatExO(UObject* key);
UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_ContainsboolExO(UObject* key);
UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_ContainsFStringExO(UObject* key);
UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_ContainsFTextExO(UObject* key);
UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_ContainsFNameExO(UObject* key);
UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_ContainsFVectorExO(UObject* key);



//++++++++++++++++++++++++++++++++++++++++++++++++++++

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_Adddouble(FName key, double v, bool sync = false);
UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_AdddoubleExI(int32 key, double v, bool sync = false);
UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_AdddoubleExO(UObject* key, double v, bool sync = false);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_ContainsdoubleExI(int32 key);
UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_ContainsdoubleExO(UObject* key);
UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_Containsdouble(FName key);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
double QP_Getdouble(FName key);
UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
double QP_GetdoubleExI(int32 key);
UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
double QP_GetdoubleExO(UObject* k = nullptr);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_RemovedoubleExO(UObject* key, bool sync = false);
UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_Removedouble(FName key, bool sync = false);
UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_RemovedoubleExI(int32 key, bool sync = false);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_CleardoubleExI(bool sync = false);
UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_CleardoubleExO(bool sync = false);
UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_Cleardouble(bool sync = false);


UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_Addint64(FName key, int64 v, bool sync = false);
UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_Addint64ExI(int32 key, int64 v, bool sync = false);
UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_Addint64ExO(UObject* key, int64 v, bool sync = false);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_Containsint64ExI(int32 key);
UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_Containsint64ExO(UObject* key);
UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
bool QP_Containsint64(FName key);

UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
int64 QP_Getint64(FName key);
UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
int64 QP_Getint64ExI(int32 key);
UFUNCTION(BlueprintPure, Category = "QipaWorld|QPData")
int64 QP_Getint64ExO(UObject* k = nullptr);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_Removeint64ExO(UObject* key, bool sync = false);
UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_Removeint64(FName key, bool sync = false);
UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_Removeint64ExI(int32 key, bool sync = false);

UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_Clearint64ExI(bool sync = false);
UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_Clearint64ExO(bool sync = false);
UFUNCTION(BlueprintCallable, Category = "QipaWorld|QPData")
void QP_Clearint64(bool sync = false);



};