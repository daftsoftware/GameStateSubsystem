// Copyright (c) 2024 Daft Software
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "ExampleGameStateSubsystem.h"
#include "Logging/StructuredLog.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ExampleGameStateSubsystem)

static FString NetModeToString(ENetMode InNetMode)
{
	switch(InNetMode)
	{
		case NM_Standalone:
			return TEXT("Standalone");
		case NM_DedicatedServer:
			return TEXT("DedicatedServer");
		case NM_ListenServer:
			return TEXT("ListenServer");
		case NM_Client:
			return TEXT("Client");
		default:
			checkf(false, TEXT("Invalid NetMode"));
			return TEXT("Invalid");
	}
}

void UExampleGameStateSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	checkf(GetOuter(), TEXT("No Outer."));

	GetWorldTimerManager().SetTimer(RPCTestTimer, FTimerDelegate::CreateWeakLambda(this, [this]()
	{
		if(HasAuthority())
		{
			ServerDoSomething();
		}
		else
		{
			ClientDoSomething();
		}
	}), 5.0f, true);

	UE_LOGFMT(LogNet, Display, "Initialized Example Gamestate Subsystem, NetMode - {0}", NetModeToString(GetWorld()->GetNetMode()));
}

void UExampleGameStateSubsystem::Deinitialize()
{
	Super::Deinitialize();

	RPCTestTimer.Invalidate();
	
	UE_LOGFMT(LogNet, Display, "Deinitialized Example Gamestate Subsystem, NetMode - {0}", NetModeToString(GetWorld()->GetNetMode()));
}

bool UExampleGameStateSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	constexpr bool CreateExampleSubsystem = false;
	return Super::ShouldCreateSubsystem(Outer) && CreateExampleSubsystem;
}

void UExampleGameStateSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(HasAuthority())
	{
		RepVar2 += 1;
	}
}

void UExampleGameStateSubsystem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UExampleGameStateSubsystem, RepVar1, COND_OwnerOnly);
	DOREPLIFETIME(UExampleGameStateSubsystem, RepVar2);
}

void UExampleGameStateSubsystem::ServerDoSomething_Implementation()
{
	RepVar1 = FMath::Rand();
	UE_LOGFMT(LogNet, Display, "Logging Something on Server! {0}", NetModeToString(GetWorld()->GetNetMode()));
}

void UExampleGameStateSubsystem::ClientDoSomething_Implementation()
{
	UE_LOGFMT(LogNet, Display, "Logging Something on Client! {0}", NetModeToString(GetWorld()->GetNetMode()));
}

void UExampleGameStateSubsystem::OnRep_RepVar2(int32 NewVar2)
{
	if(!HasAuthority())
	{
		UE_LOGFMT(LogNet, Display, "RepVar2 Changed on Example GameState Subsystem! {0}", NewVar2);
	}
}

void UExampleGameStateSubsystem::BlueprintDoAThing()
{
	UE_LOGFMT(LogTemp, Display, "Do a thing called from Blueprint.");
}
