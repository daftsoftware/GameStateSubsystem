// Copyright (c) 2024 Daft Software
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "GameStateSubsystem.h"
#include "ExampleGameStateSubsystem.generated.h"

UCLASS(MinimalAPI)
class UExampleGameStateSubsystem : public UTickableGameStateSubsystem
{
	GENERATED_BODY()
public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Tick(float DeltaTime) override;
    TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UExampleGameStateSubsystem, STATGROUP_Tickables); }
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable)
	void ServerDoSomething();

	UFUNCTION(Client, Reliable)
	void ClientDoSomething();

	UPROPERTY(Transient, Replicated)
	int32 RepVar1 = 0;

	UPROPERTY(Transient, ReplicatedUsing=OnRep_RepVar2)
	int32 RepVar2 = 0;

	UFUNCTION()
	void OnRep_RepVar2(int32 NewVar2);

	UFUNCTION(BlueprintCallable)
	void BlueprintDoAThing();

private:

	FTimerHandle RPCTestTimer;
};
