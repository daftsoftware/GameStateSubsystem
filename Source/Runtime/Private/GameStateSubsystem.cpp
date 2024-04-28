// Copyright (c) 2024 Daft Software
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "GameStateSubsystem.h"
#include "ExtendableGameStateBase.h"
#if UE_WITH_IRIS
#include "Iris/ReplicationSystem/ReplicationFragmentUtil.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameStateSubsystem)

AExtendableGameStateBase* UGameStateSubsystem::GetGameState() const
{
	return CastChecked<AExtendableGameStateBase>(GetOuter());
}

ENetRole UGameStateSubsystem::GetLocalRole() const
{
	return GetGameState()->GetLocalRole();
}

bool UGameStateSubsystem::HasAuthority() const
{
	return (GetLocalRole() == ROLE_Authority);
}

bool UGameStateSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	const UWorld* World = Outer->GetWorld();
	return World && World->IsGameWorld();
}

UWorld& UGameStateSubsystem::GetWorldRef() const
{
	checkf(GetOuter()->GetWorld(), TEXT("Invalid world on outer."));
	return *GetOuter()->GetWorld();
}

UWorld* UGameStateSubsystem::GetWorld() const
{
	return GetOuter()->GetWorld();
}

bool UGameStateSubsystem::IsNameStableForNetworking() const
{
	return true;
}

bool UGameStateSubsystem::IsFullNameStableForNetworking() const
{
	return true;
}

bool UGameStateSubsystem::IsSupportedForNetworking() const
{
	return true;
}

int32 UGameStateSubsystem::GetFunctionCallspace(UFunction* Function, FFrame* Stack)
{
	return GetGameState()->GetFunctionCallspace(Function, Stack);
}

bool UGameStateSubsystem::CallRemoteFunction(UFunction* Function, void* Parms, FOutParmRec* OutParms, FFrame* Stack)
{
	if(UNetDriver* NetDriver = GetGameState()->GetNetDriver())
	{
		NetDriver->ProcessRemoteFunction(GetGameState(), Function, Parms, OutParms, Stack, this);
		return true;
	}
	return false;
}

#if UE_WITH_IRIS
void UGameStateSubsystem::RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context, UE::Net::EFragmentRegistrationFlags RegistrationFlags)
{
	Super::RegisterReplicationFragments(Context, RegistrationFlags);
	
	UE::Net::FReplicationFragmentUtil::CreateAndRegisterFragmentsForObject(this, Context, RegistrationFlags);
}
#endif

ETickableTickType UTickableGameStateSubsystem::GetTickableTickType() const
{
	// By default (if the child class doesn't override GetTickableTickType), don't let CDOs ever tick: 
	return IsTemplate() ? ETickableTickType::Never : FTickableGameObject::GetTickableTickType(); 
}

bool UTickableGameStateSubsystem::IsAllowedToTick() const
{
	// No matter what IsTickable says, don't let CDOs or uninitialized subsystems tick.
	// Note: even if GetTickableTickType was overridden by the child class and returns something else than ETickableTickType::Never for CDOs, 
	//  it's probably a mistake, so by default, don't allow ticking. If the child class really intends its CDO to tick, it can always override IsAllowedToTick...
	return !IsTemplate() && bInitialized;
}

void UTickableGameStateSubsystem::Tick(float DeltaTime)
{
	checkf(IsInitialized(), TEXT("Ticking should have been disabled for an uninitialized subsystem : remember to call IsInitialized in the subsystem's IsTickable, IsTickableInEditor and/or IsTickableWhenPaused implementation"));
}

void UTickableGameStateSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	check(!bInitialized);
	bInitialized = true;
}

void UTickableGameStateSubsystem::Deinitialize()
{
	check(bInitialized);
	bInitialized = false;
}
