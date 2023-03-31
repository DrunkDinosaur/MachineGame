// Fill out your copyright notice in the Description page of Project Settings.


#include "BoxSelectionHUD.h"

#include "TacticalController.h"


void ABoxSelectionHUD::DrawHUD()
{
	Super::DrawHUD();
	if(bIsDrawing)
	{
		auto PlayerController = GetOwningPlayerController();
		if(PlayerController){
			PlayerController->GetMousePosition(CurrentMousePosition.X, CurrentMousePosition.Y);
			FLinearColor RectColor = FLinearColor::Blue;
            RectColor.A = 0.2;
            double Width = CurrentMousePosition.X - InitialMousePosition.X;
            double Height = CurrentMousePosition.Y - InitialMousePosition.Y;
            DrawRect(RectColor, InitialMousePosition.X, InitialMousePosition.Y, Width, Height);

			TArray<AMachineGameCharacter*> SelectedActors;
			GetActorsInSelectionRectangle<AMachineGameCharacter>(InitialMousePosition, CurrentMousePosition,SelectedActors);
			if( ATacticalController* ctrl = Cast<ATacticalController> (GetOwningPlayerController()) )
			{
				ctrl->SetSelectedCharacters(SelectedActors);
			}
		}
	}
}

// TArray<AMachineGameCharacter*> ABoxSelectionHUD::SelectCharactersInRectangle(const FVector2D BoxStart, const FVector2D BoxEnd)
// {
// 	UE_LOG(LogTemp, Warning, TEXT("DRAWING BOX LOL"));
// 	FLinearColor RectColor = FLinearColor::Blue;
// 	RectColor.A = 0.2;
// 	double Width = BoxEnd.X - BoxStart.X;
// 	double Height = BoxEnd.Y - BoxEnd.Y;
// 	DrawRect(RectColor, BoxStart.X, BoxStart.Y, Width, Height);
// 	TArray<AActor*> SelectedActors;
// 	GetActorsInSelectionRectangle(BoxStart, BoxEnd,SelectedActors);
//
// 	TArray<AMachineGameCharacter*> Result;
// 	for(AActor* Actor : SelectedActors)
// 	{
// 		if(AMachineGameCharacter* IngameCharacterPtr = Cast<AMachineGameCharacter>(Actor))
// 		Result.Add(IngameCharacterPtr);		
// 	}
//
// 	return Result;
// }


void ABoxSelectionHUD::StartDrawing()
{	
	if(!bIsDrawing)
	{
		GetOwningPlayerController()->GetMousePosition(InitialMousePosition.X, InitialMousePosition.Y);
	}

	bIsDrawing = true;
}

void ABoxSelectionHUD::StopDrawing()
{
	bIsDrawing = false;
}
