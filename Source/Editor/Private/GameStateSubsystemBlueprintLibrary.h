// Copyright (c) 2024 Daft Software
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "GameStateSubsystemBlueprintLibrary.generated.h"

class AExtendableGameStateBase;
class UGameStateSubsystem;

UCLASS(MinimalAPI)
class UGameStateSubsystemBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	/** Get a GameState Subsystem from the GameState associated with the provided context */
    UFUNCTION(BlueprintPure, Category = "GameState Subsystems", meta = (WorldContext = "ContextObject", BlueprintInternalUseOnly = "true"))
    static GAMESTATESUBSYSTEMED_API UGameStateSubsystem* GetGameStateSubsystem(UObject* ContextObject, TSubclassOf<UGameStateSubsystem> Class);

	/** 
     * Get a GameState Subsystem from the GameState associated with the provided context
     * If the player GameState isn't valid a nullptr is returned
     */
    UFUNCTION(BlueprintPure, Category = "GameState Subsystems", meta = (BlueprintInternalUseOnly = "true"))
    static GAMESTATESUBSYSTEMED_API UGameStateSubsystem* GetGameStateSubsystemFromGameState(AExtendableGameStateBase* GameState, TSubclassOf<UGameStateSubsystem> Class);
};