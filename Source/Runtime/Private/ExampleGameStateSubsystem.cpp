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

void UExampleGameStateSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	checkf(GetOuter(), TEXT("No Outer."));

	UE_LOGFMT(LogTemp, Display, "Initialized Example Gamestate Subsystem, NetMode - {0}", GetWorld()->GetNetMode());
}

void UExampleGameStateSubsystem::Deinitialize()
{
	Super::Deinitialize();
	
	UE_LOGFMT(LogTemp, Display, "Deinitialized Example Gamestate Subsystem, NetMode - {0}", GetWorld()->GetNetMode());
}

bool UExampleGameStateSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return true;
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
	UE_LOGFMT(LogTemp, Display, "Logging Something on Server! {0}", GetWorld()->GetNetMode());
}

void UExampleGameStateSubsystem::ClientDoSomething_Implementation()
{
	UE_LOGFMT(LogTemp, Display, "Logging Something on Client! {0}", GetWorld()->GetNetMode());
}

void UExampleGameStateSubsystem::OnRep_RepVar2(int32 NewVar2)
{
	UE_LOGFMT(LogTemp, Display, "RepVar2 Changed on Example GameState Subsystem! {0}", NewVar2);
}
