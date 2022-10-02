// Fill out your copyright notice in the Description page of Project Settings.


#include "Localization/QPGIM_Localization.h"
//#include "Internationalization/PolyglotTextData.h"
#include <Kismet/KismetInternationalizationLibrary.h>

UQPGIM_Localization* UQPGIM_Localization::QP_UQPGIM_Localization = nullptr;


bool UQPGIM_Localization::ShouldCreateSubsystem(UObject* Outer) const
{
	Super::ShouldCreateSubsystem(Outer);
	return true;
}

void UQPGIM_Localization::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	//Collection.InitializeDependency(UQPGameInstanceDataManager::StaticClass());

	QP_UQPGIM_Localization = this;
	//LoadYaml("");
	//qp_gameQPdataBase = NewObject<UQPData>();
}

void UQPGIM_Localization::Deinitialize()
{
	Super::Deinitialize();
}

bool UQPGIM_Localization::QP_SetCurrentCulture(const FString& InCultureName)
{
	return UKismetInternationalizationLibrary::SetCurrentCulture(InCultureName);
}

FString UQPGIM_Localization::QP_GetCurrentLocale()
{
	return UKismetInternationalizationLibrary::GetCurrentLocale();
}

FString UQPGIM_Localization::QP_GetCultureDisplayName(const FString& Culture, const bool Localized)
{
	return UKismetInternationalizationLibrary::GetCultureDisplayName(Culture, Localized);
}

TArray<FString> UQPGIM_Localization::QP_GetLocalizedCultures() const
{
	return UKismetInternationalizationLibrary::GetLocalizedCultures();
}

FString UQPGIM_Localization::QP_GetSuitableCulture(TArray<FString> AvailableCultures, FString CulturetoMatch, FString FallbackCulture)
{
	return UKismetInternationalizationLibrary::GetSuitableCulture(AvailableCultures, CulturetoMatch, FallbackCulture);
}
