#pragma once

#define GET_TIME FString(__TIME__)

#define GET_CALL_INFO FString::Printf(TEXT("function: %s(), line: %d, file: %s"), *FString(__FUNCTION__), __LINE__, TEXT(__FILE__))

#define PRINT(Format, ...) \
		UE_LOG(LogTemp, Display, TEXT("[%s] %s"), *GET_TIME, *FString::Printf(TEXT(Format), ##__VA_ARGS__)); \
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, FString::Printf(TEXT(Format), ##__VA_ARGS__), false);

#define PRINT_WARNING(Format, ...) \
		UE_LOG(LogTemp, Warning, TEXT("[%s] %s *%s"), *GET_TIME, *FString::Printf(TEXT(Format), ##__VA_ARGS__), *GET_CALL_INFO); \
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT(Format), ##__VA_ARGS__), false);

#define PRINT_ERROR(Format, ...) \
		UE_LOG(LogTemp, Error, TEXT("[%s] %s *%s"), *GET_TIME, *FString::Printf(TEXT(Format), ##__VA_ARGS__), *GET_CALL_INFO); \
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT(Format), ##__VA_ARGS__), false);