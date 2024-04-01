// Copyright (c) 2024 Daft Software
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "GameStateSubsystemBlueprintLibrary.h"
#include "ExtendableGameStateBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameStateSubsystemBlueprintLibrary)

UGameStateSubsystem* UGameStateSubsystemBlueprintLibrary::GetGameStateSubsystem(UObject* ContextObject, TSubclassOf<UGameStateSubsystem> Class)
{
   	if(const auto* GameState = Cast<AExtendableGameStateBase>(ContextObject))
   	{
   		return GameState->GetSubsystemBase(Class);
   	}
    return nullptr;
}

UGameStateSubsystem* UGameStateSubsystemBlueprintLibrary::GetGameStateSubsystemFromGameState(AExtendableGameStateBase* GameState, TSubclassOf<UGameStateSubsystem> Class)
{
    return GameState ? GameState->GetSubsystemBase(Class) : nullptr;
}
