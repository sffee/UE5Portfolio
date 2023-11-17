
#include "BOTWEditor.h"
#include "Modules/ModuleManager.h"
#include "Modules/ModuleInterface.h"

IMPLEMENT_GAME_MODULE(FBOTWEditorModule, BOTWEditor);

DEFINE_LOG_CATEGORY(BOTWEditor)

#define LOCTEXT_NAMESPACE "BOTWEditor"

void FBOTWEditorModule::StartupModule()
{
    
}

void FBOTWEditorModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE