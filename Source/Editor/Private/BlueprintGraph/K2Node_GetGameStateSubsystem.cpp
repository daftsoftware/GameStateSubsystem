// Copyright (c) 2024 Daft Software
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "K2Node_GetGameStateSubsystem.h"
#include "ExtendableGameStateBase.h"
#include "GameStateSubsystemBlueprintLibrary.h"
#include "GameStateSubsystem.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "K2Node_CallFunction.h"
#include "EdGraphSchema_K2.h"
#include "KismetCompiler.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(K2Node_GetGameStateSubsystem)

// ************************************************************************************
//    UK2Node_GetGameStateSubsystem
// ************************************************************************************

void UK2Node_GetGameStateSubsystem::ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	// Skip the UK2Node_GetSubsystem implementation
	UK2Node::ExpandNode(CompilerContext, SourceGraph);
	
	static const FName WorldContextObject_ParamName(TEXT("ContextObject"));
	static const FName Class_ParamName(TEXT("Class"));

	UK2Node_GetSubsystem* GetSubsystemNode = this;
	UEdGraphPin* SpawnWorldContextPin = GetSubsystemNode->GetWorldContextPin();
	UEdGraphPin* SpawnClassPin = GetSubsystemNode->GetClassPin();
	UEdGraphPin* SpawnNodeResult = GetSubsystemNode->GetResultPin();

	UClass* SpawnClass = (SpawnClassPin != nullptr) ? Cast<UClass>(SpawnClassPin->DefaultObject) : nullptr;
	if (SpawnClassPin && (SpawnClassPin->LinkedTo.Num() == 0) && !SpawnClass)
	{
		CompilerContext.MessageLog.Error(*NSLOCTEXT("K2Node", "GetSubsystem_Error", "Node @@ must have a class specified.").ToString(), GetSubsystemNode);
		GetSubsystemNode->BreakAllNodeLinks();
		return;
	}

	// Choose appropriate underlying Getter
	FName Get_FunctionName;
	if (CustomClass->IsChildOf<UGameStateSubsystem>())
	{
		Get_FunctionName = GET_FUNCTION_NAME_CHECKED(UGameStateSubsystemBlueprintLibrary, GetGameStateSubsystem);
	}
	else
	{
		CompilerContext.MessageLog.Error(*NSLOCTEXT("K2Node", "GetSubsystem_Error", "Node @@ must have a class specified.").ToString(), GetSubsystemNode);
		GetSubsystemNode->BreakAllNodeLinks();
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	// create 'UGameStateSubsystemBlueprintLibrary::GetGameStateSubsystem' call node
	UK2Node_CallFunction* CallGetNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(GetSubsystemNode, SourceGraph);
	CallGetNode->FunctionReference.SetExternalMember(Get_FunctionName, UGameStateSubsystemBlueprintLibrary::StaticClass());
	CallGetNode->AllocateDefaultPins();

	UEdGraphPin* CallCreateWorldContextPin = CallGetNode->FindPinChecked(WorldContextObject_ParamName);
	UEdGraphPin* CallCreateClassTypePin = CallGetNode->FindPinChecked(Class_ParamName);
	UEdGraphPin* CallCreateResult = CallGetNode->GetReturnValuePin();

	if (SpawnClassPin && SpawnClassPin->LinkedTo.Num() > 0)
	{
		// Copy the 'class' connection from the spawn node to 'UGameStateSubsystemBlueprintLibrary::GetGameStateSubsystem'
		CompilerContext.MovePinLinksToIntermediate(*SpawnClassPin, *CallCreateClassTypePin);
	}
	else
	{
		// Copy class literal onto 'UGameStateSubsystemBlueprintLibrary::GetGameStateSubsystem' call
		CallCreateClassTypePin->DefaultObject = *CustomClass;
	}

	// Copy the world context connection from the spawn node to 'UGameStateSubsystemBlueprintLibrary::GetGameStateSubsystem' if necessary
	if (SpawnWorldContextPin)
	{
		CompilerContext.MovePinLinksToIntermediate(*SpawnWorldContextPin, *CallCreateWorldContextPin);
	}

	// Move result connection from spawn node to 'UGameStateSubsystemBlueprintLibrary::GetGameStateSubsystem'
	CallCreateResult->PinType = SpawnNodeResult->PinType;
	CompilerContext.MovePinLinksToIntermediate(*SpawnNodeResult, *CallCreateResult);

	//////////////////////////////////////////////////////////////////////////

	// Break any links to the expanded node
	GetSubsystemNode->BreakAllNodeLinks();
}

FText UK2Node_GetGameStateSubsystem::GetTooltipText() const
{
	if (CustomClass)
	{
		FText SubsystemTypeText;
		if (CustomClass->IsChildOf<UGameStateSubsystem>())
		{
			SubsystemTypeText =  NSLOCTEXT("K2Node", "GetSubsystem_GameStateSubsystemTooltip", "GameState Subsystem");
		}
		return FText::FormatNamed(NSLOCTEXT("K2Node", "GetSubsystem_TooltipFormat", "Get {ClassName} ({SubsystemType})\n\n{ClassTooltip}"),
			TEXT("ClassName"), CustomClass->GetDisplayNameText(),
			TEXT("SubsystemType"), SubsystemTypeText,
			TEXT("ClassTooltip"), CustomClass->GetToolTipText(/*bShortTooltip=*/ true));
	}

	return NSLOCTEXT("K2Node", "GetSubsystem_InvalidSubsystemTypeTooltip", "Invalid Subsystem Type");
}

void UK2Node_GetGameStateSubsystem::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	static TArray<UClass*> Subclasses;
	Subclasses.Reset();
	GetDerivedClasses(UGameStateSubsystem::StaticClass(), Subclasses);

	auto CustomizeCallback = [](UEdGraphNode* Node, bool bIsTemplateNode, UClass* Subclass)
	{
		auto TypedNode = CastChecked<UK2Node_GetGameStateSubsystem>(Node);
		TypedNode->Initialize(Subclass);
	};

	UClass* ActionKey = GetClass();
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		for (UClass* Iter : Subclasses)
		{
			if (!UEdGraphSchema_K2::IsAllowableBlueprintVariableType(Iter, true))
			{
				continue;
			}

			UBlueprintNodeSpawner* Spawner = UBlueprintNodeSpawner::Create(ActionKey);
			check(Spawner);

			Spawner->CustomizeNodeDelegate = UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateStatic(CustomizeCallback, Iter);
			ActionRegistrar.AddBlueprintAction(ActionKey, Spawner);
		}
	}
}

FText UK2Node_GetGameStateSubsystem::GetMenuCategory() const
{
	return NSLOCTEXT("K2Node", "GetGameStateSubsystem_MenuCategory", "GameState|GameState Subsystems");
}

void UK2Node_GetSubsystemFromGS::AllocateDefaultPins()
{
	// If required add the world context pin
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, AExtendableGameStateBase::StaticClass(), TEXT("GameState"));

	// Add blueprint pin
	if (!CustomClass)
	{
		CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Class, USubsystem::StaticClass(), TEXT("Class"));
	}

	// Result pin
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Object, (CustomClass ? (UClass*)CustomClass : UGameStateSubsystem::StaticClass()), UEdGraphSchema_K2::PN_ReturnValue);

	// Skip the UK2Node_GetSubsystem implementation
	UK2Node::AllocateDefaultPins();
}

// ************************************************************************************
//    UK2Node_GetSubsystemFromGS
// ************************************************************************************

void UK2Node_GetSubsystemFromGS::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	// Skip the UK2Node_GetGameStateSubsystem implementation
	UK2Node::ExpandNode(CompilerContext, SourceGraph);

	static const FName GameState_ParamName(TEXT("GameState"));
	static const FName Class_ParamName(TEXT("Class"));

	UK2Node_GetSubsystemFromGS* GetSubsystemFromGSNode = this;
	UEdGraphPin* SpawnGameStatePin = GetSubsystemFromGSNode->GetGameStatePin();
	UEdGraphPin* SpawnClassPin = GetSubsystemFromGSNode->GetClassPin();
	UEdGraphPin* SpawnNodeResult = GetSubsystemFromGSNode->GetResultPin();

	UClass* SpawnClass = (SpawnClassPin != nullptr) ? Cast<UClass>(SpawnClassPin->DefaultObject) : nullptr;
	if (SpawnClassPin && (SpawnClassPin->LinkedTo.Num() == 0) && !SpawnClass)
	{
		CompilerContext.MessageLog.Error(*NSLOCTEXT("K2Node", "GetSubsystem_Error", "Node @@ must have a class specified.").ToString(), GetSubsystemFromGSNode);
		GetSubsystemFromGSNode->BreakAllNodeLinks();
		return;
	}

	// Choose appropriate underlying Getter
	FName Get_FunctionName;
	if (CustomClass->IsChildOf<UGameStateSubsystem>())
	{
		Get_FunctionName = GET_FUNCTION_NAME_CHECKED(UGameStateSubsystemBlueprintLibrary, GetGameStateSubsystemFromGameState);
	}
	else
	{
		CompilerContext.MessageLog.Error(*NSLOCTEXT("K2Node", "GetSubsystem_Error", "Node @@ must have a class specified.").ToString(), GetSubsystemFromGSNode);
		GetSubsystemFromGSNode->BreakAllNodeLinks();
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	// create 'UGameStateSubsystemBlueprintLibrary::GetGameStateSubsystemFromGameState' call node
	UK2Node_CallFunction* CallGetNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(GetSubsystemFromGSNode, SourceGraph);
	CallGetNode->FunctionReference.SetExternalMember(Get_FunctionName, UGameStateSubsystemBlueprintLibrary::StaticClass());
	CallGetNode->AllocateDefaultPins();

	UEdGraphPin* CallCreateGameStatePin = CallGetNode->FindPinChecked(GameState_ParamName);
	UEdGraphPin* CallCreateClassTypePin = CallGetNode->FindPinChecked(Class_ParamName);
	UEdGraphPin* CallCreateResult = CallGetNode->GetReturnValuePin();

	if (SpawnClassPin && SpawnClassPin->LinkedTo.Num() > 0)
	{
		// Copy the 'class' connection from the spawn node to 'UGameStateSubsystemBlueprintLibrary::GetGameStateSubsystemFromGameState'
		CompilerContext.MovePinLinksToIntermediate(*SpawnClassPin, *CallCreateClassTypePin);
	}
	else
	{
		// Copy class literal onto 'USubsystemBlueprintLibrary::GetLocalPlayerSubSystemFromPlayerController' call
		CallCreateClassTypePin->DefaultObject = *CustomClass;
	}

	// Copy the world context connection from the spawn node to 'USubsystemBlueprintLibrary::GetLocalPlayerSubSystemFromPlayerController' if necessary
	if (SpawnGameStatePin)
	{
		CompilerContext.MovePinLinksToIntermediate(*SpawnGameStatePin, *CallCreateGameStatePin);
	}

	// Move result connection from spawn node to 'UGameStateSubsystemBlueprintLibrary::GetGameStateSubsystem'
	CallCreateResult->PinType = SpawnNodeResult->PinType;
	CompilerContext.MovePinLinksToIntermediate(*SpawnNodeResult, *CallCreateResult);

	//////////////////////////////////////////////////////////////////////////

	// Break any links to the expanded node
	GetSubsystemFromGSNode->BreakAllNodeLinks();
}

FText UK2Node_GetSubsystemFromGS::GetTooltipText() const
{
	if (CustomClass)
	{
		return FText::FormatNamed(NSLOCTEXT("K2Node", "GetSubsystemFromGS_TooltipFormat", "Get {ClassName} from GameState"), TEXT("ClassName"), CustomClass->GetDisplayNameText());
	}

	return NSLOCTEXT("K2Node", "GetSubsystemFromGS_InvalidSubsystemTypeTooltip", "Invalid Subsystem Type");
}

void UK2Node_GetSubsystemFromGS::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	static TArray<UClass*> Subclasses;
	Subclasses.Reset();
	GetDerivedClasses(UGameStateSubsystem::StaticClass(), Subclasses);

	auto CustomizeCallback = [](UEdGraphNode* Node, bool bIsTemplateNode, UClass* Subclass)
	{
		auto TypedNode = CastChecked<UK2Node_GetSubsystemFromGS>(Node);
		TypedNode->Initialize(Subclass);
	};

	UClass* ActionKey = GetClass();
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		for (UClass* Iter : Subclasses)
		{
			if (!UEdGraphSchema_K2::IsAllowableBlueprintVariableType(Iter, true))
			{
				continue;
			}

			UBlueprintNodeSpawner* Spawner = UBlueprintNodeSpawner::Create(ActionKey);
			check(Spawner);

			Spawner->CustomizeNodeDelegate = UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateStatic(CustomizeCallback, Iter);
			ActionRegistrar.AddBlueprintAction(ActionKey, Spawner);
		}
	}
}

FText UK2Node_GetSubsystemFromGS::GetMenuCategory() const
{
	return NSLOCTEXT("K2Node", "GetSubsystemFromGS_MenuCategory", "GameState|GameState Subsystems");
}

UEdGraphPin* UK2Node_GetSubsystemFromGS::GetGameStatePin() const
{
	UEdGraphPin* Pin = FindPin(TEXT("GameState"));
 	check(Pin == NULL || Pin->Direction == EGPD_Input);
 	return Pin;
}