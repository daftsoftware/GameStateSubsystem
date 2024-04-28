// Copyright (c) 2024 Daft Software
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "Subsystems/Subsystem.h"
#include "GameStateSubsystem.generated.h"

class AExtendableGameStateBase;

/**
 * GameStateSubsystems offer an alternative to UGameStateComponent for compositional systems that need
 * to be replicated and part of the GameState lifetime. With GameStateComponents typically you would use
 * Lyra's Experience system to compose which components are active in certain contexts or you just manually
 * add them as subobjects. This subsystem type is by no means a replacement for GameState Components and
 * you should be aware there is times where they are more suitable.
 *
 * The time when GameStateSubsystems are useful is for systems that are for the most apart unconditionally
 * available and need to be a modular extension of the GameState itself. Since they are automatically registered
 * you only need to create the subsystem and the functionality will run as long as the GameState is alive.
 *
 * It's also worth nothing that GameStateSubsystems are essentially there to serve a similar purpose to world
 * subsystems. However their dependency on the GameState greatly simplifies replication context, since world
 * subsystems existing during obscure times in the world lifecycle, and due to their singleton nature, you can't
 * have multiple instances of them on the server, which would be required if the player is authoritative over
 * their own subsystem (similarly to a player controller). For that functionality I recommend UControllerComponent.
 */

/**
 * UGameStateSubsystem
 * Base class for auto instanced and initialized systems that share the lifetime of modular game state
 * They share the same replication rules as the GameStateBase.
 */
UCLASS(Abstract, MinimalAPI)
class UGameStateSubsystem : public USubsystem
{
	GENERATED_BODY()
public:

	virtual void BeginPlay() {}

	GAMESTATESUBSYSTEM_API AExtendableGameStateBase* GetGameState() const;

	template<typename T>
	T* GetGameState() const
	{
		return Cast<T>(GetGameState());
	}

	GAMESTATESUBSYSTEM_API FTimerManager& GetWorldTimerManager() { return GetWorld()->GetTimerManager(); }

	GAMESTATESUBSYSTEM_API ENetRole GetLocalRole() const;
	GAMESTATESUBSYSTEM_API bool HasAuthority() const;

	GAMESTATESUBSYSTEM_API virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	
	/**
	 * Returns a reference to the UWorld this subsystem is contained within.
	 * @note This should not be called on default object since the method assumes a valid outer world.
	 */
	GAMESTATESUBSYSTEM_API UWorld& GetWorldRef() const;
	
	GAMESTATESUBSYSTEM_API virtual UWorld* GetWorld() const override final;

	//~ Begin UObject
	GAMESTATESUBSYSTEM_API virtual bool IsNameStableForNetworking() const override final;
	GAMESTATESUBSYSTEM_API virtual bool IsFullNameStableForNetworking() const override final;
	GAMESTATESUBSYSTEM_API virtual bool IsSupportedForNetworking() const override final;
	GAMESTATESUBSYSTEM_API virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;
	GAMESTATESUBSYSTEM_API virtual bool CallRemoteFunction(UFunction* Function, void* Parms, FOutParmRec* OutParms, FFrame* Stack) override;
#if UE_WITH_IRIS
	GAMESTATESUBSYSTEM_API virtual void RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context, UE::Net::EFragmentRegistrationFlags RegistrationFlags) override;
#endif
	//~ End UObject
};

/**
 * UTickableGameStateSubsystem
 * GameStateSubsystem that has tick functionality.
 */
UCLASS(Abstract, MinimalAPI)
class UTickableGameStateSubsystem : public UGameStateSubsystem, public FTickableGameObject
{
	GENERATED_BODY()
public:
	
	//~ Begin FTickableGameObject
    virtual UWorld* GetTickableGameObjectWorld() const override { return GetWorld(); }
    GAMESTATESUBSYSTEM_API virtual ETickableTickType GetTickableTickType() const override;
    GAMESTATESUBSYSTEM_API virtual bool IsAllowedToTick() const override final;
    GAMESTATESUBSYSTEM_API virtual void Tick(float DeltaTime) override;
    GAMESTATESUBSYSTEM_API virtual TStatId GetStatId() const override PURE_VIRTUAL(UTickableWorldSubsystem::GetStatId, return TStatId(););
    //~ End FTickableGameObject
	
	//~ Begin USubsystem
	GAMESTATESUBSYSTEM_API virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	GAMESTATESUBSYSTEM_API virtual void Deinitialize() override;
	//~End USubsystem

	bool IsInitialized() const { return bInitialized; }
	
private:
	bool bInitialized = false;
};
