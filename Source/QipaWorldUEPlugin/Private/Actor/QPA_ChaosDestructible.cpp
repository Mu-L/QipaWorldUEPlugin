// QIPAWORLD


#include "Actor/QPA_ChaosDestructible.h"

AQPA_ChaosDestructible::AQPA_ChaosDestructible()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	

	//RootComponent = qp_geometryCollection;
	qp_geometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("qp_geometryCollection"));
	qp_geometryCollection->SetupAttachment(RootComponent);
	qp_geometryCollection->SetNotifyBreaks(true);
	
	qp_geometryCollection->SetSimulatePhysics(true);
	qp_geometryCollection->SetNotifyRigidBodyCollision(true);


	qp_footstepAudio = CreateDefaultSubobject<UAudioComponent>("footstepAudio");
	qp_footstepAudio->SetAutoActivate(false);
	qp_footstepAudio->SetupAttachment(RootComponent);

	qp_playRandomSound = CreateDefaultSubobject<UQPC_PlayRandomSound>("qp_playRandomSound");
	qp_playRandomSound->qp_footstepAudio = qp_footstepAudio;

}

// Called when the game starts or when spawned
void AQPA_ChaosDestructible::BeginPlay()
{
	Super::BeginPlay();
	//qp_geometryCollection->OnPhysicsCollision
	qp_geometryCollection->OnChaosPhysicsCollision.AddDynamic(this, &AQPA_ChaosDestructible::QP_OnChunkHit);
	qp_geometryCollection->OnChaosBreakEvent.AddDynamic(this, &AQPA_ChaosDestructible::QP_OnBroken);
	//OnActorHit.AddDynamic(this, &AQPA_ChaosDestructible::QP_OnActorHit);

}

//void AQPA_ChaosDestructible::QP_OnChunkHit_Bind(const FChaosPhysicsCollisionInfo& BreakEvent)
//{
//	QP_OnChunkHit(BreakEvent);
//}
//void AQPA_ChaosDestructible::QP_OnBroken_Bind(const FChaosBreakEvent& BreakEvent)
//{
//	QP_OnBroken(BreakEvent);
//}

void AQPA_ChaosDestructible::QP_OnBroken(const FChaosBreakEvent& BreakEvent)
{
	qp_autoDestroy = qp_delayDestroy;
	qp_isAutoDestroy = true;
}
//void AQPA_ChaosDestructible::QP_OnActorHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit) {
	//UE_LOG(LogTemp, Warning, TEXT("___!_______%s"), *OtherActor->GetName());
//}
void AQPA_ChaosDestructible::QP_OnChunkHit(const FChaosPhysicsCollisionInfo& CollisionInfo)
{
	//FVector Location = CollisionInfo.ContactLocation;

	//if (CollisionInfo.AccumulatedImpulse.Size() > 10.0f)
	//{
		//UGameplayStatics::PlaySoundAtLocation(this, ImpactSoundCue, Location);
	//}
	//UE_LOG(LogTemp, Warning, TEXT("___2_"));
	float velocity = (CollisionInfo.Velocity.Size() + CollisionInfo.OtherVelocity.Size());
	//UE_LOG(LogTemp, Warning, TEXT("___%f_"), velocity);
	if (velocity > qp_minVelocity) {
		
		qp_autoDestroy = qp_delayDestroy;
		qp_playRandomSound->QP_SetVolume(velocity / qp_maxSpeed * (1 - qp_minVolume) + qp_minVolume);
		qp_playRandomSound->QP_Play(CollisionInfo.OtherComponent->GetOwner());

	}
}