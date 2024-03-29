// Copyright (c) 2024 Daft Software
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "ModularGameStateBase.h"

void AModularGameStateBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	check(!SubsystemCollection.IsInitialized());
	SubsystemCollection.Initialize(this);
}

void AModularGameStateBase::BeginPlay()
{
	Super::BeginPlay();

	for(UGameStateSubsystem* Subsystem : GetSubsystemArray<UGameStateSubsystem>())
	{
		Subsystem->BeginPlay();
	}
}

void AModularGameStateBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	SubsystemCollection.Deinitialize();
	
	Super::EndPlay(EndPlayReason);
}

void AModularGameStateBase::AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector)
{
	AModularGameStateBase* This = CastChecked<AModularGameStateBase>(InThis);
	This->SubsystemCollection.AddReferencedObjects(InThis, Collector);
	Super::AddReferencedObjects(InThis, Collector);
}