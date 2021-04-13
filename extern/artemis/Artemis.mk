##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Dynamic_Library
ProjectName            :=Artemis
ConfigurationName      :=Dynamic_Library
WorkspacePath          := "C:\Users\Sidar\Documents\Sidar\C++\ST"
ProjectPath            := "C:\Users\Sidar\Documents\Sidar\C++\ST\Artemis"
IntermediateDirectory  :=./Release
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Sidar
Date                   :=23-10-2012
CodeLitePath           :="C:\Program Files (x86)\CodeLite"
LinkerName             :=g++
SharedObjectLinkerName :=g++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.o.i
DebugSwitch            :=-gstab
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=$(IntermediateDirectory)/artemis.dll
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E 
ObjectsFileList        :="C:\Users\Sidar\Documents\Sidar\C++\ST\Artemis\Artemis.txt"
PCHCompileFlags        :=
MakeDirCommand         :=makedir
LinkOptions            :=  -O3 
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). $(IncludeSwitch)src 
IncludePCH             := 
RcIncludePath          := 
Libs                   := 
ArLibs                 :=  
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := ar rcus
CXX      := g++
CC       := gcc
CXXFLAGS :=  -std=c++11  $(Preprocessors)
CFLAGS   :=   $(Preprocessors)


##
## User defined environment variables
##
CodeLiteDir:=C:\Program Files (x86)\CodeLite
Objects=$(IntermediateDirectory)/src_Component$(ObjectSuffix) $(IntermediateDirectory)/src_ComponentType$(ObjectSuffix) $(IntermediateDirectory)/src_ComponentTypeManager$(ObjectSuffix) $(IntermediateDirectory)/src_EntityProcessingSystem$(ObjectSuffix) $(IntermediateDirectory)/src_EntitySystem$(ObjectSuffix) $(IntermediateDirectory)/src_Entity$(ObjectSuffix) $(IntermediateDirectory)/src_World$(ObjectSuffix) $(IntermediateDirectory)/src_DelayedEntitySystem$(ObjectSuffix) $(IntermediateDirectory)/src_DelayedEntityProcessingSystem$(ObjectSuffix) $(IntermediateDirectory)/src_EntityManager$(ObjectSuffix) \
	$(IntermediateDirectory)/src_SystemBitManager$(ObjectSuffix) $(IntermediateDirectory)/src_SystemManager$(ObjectSuffix) $(IntermediateDirectory)/src_TagManager$(ObjectSuffix) $(IntermediateDirectory)/src_GroupManager$(ObjectSuffix) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects) > $(ObjectsFileList)
	$(SharedObjectLinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)
	@$(MakeDirCommand) "C:\Users\Sidar\Documents\Sidar\C++\ST\.build-dynamic_library"
	@echo rebuilt > "C:\Users\Sidar\Documents\Sidar\C++\ST\.build-dynamic_library\Artemis"

PostBuild:
	@echo Executing Post Build commands ...
	move $(ProjectPath)\Release\artemis.dll $(ProjectPath)\bin
	gendef - $(ProjectPath)\bin\artemis.dll > $(ProjectPath)\bin\artemisdll.def
	dlltool -d $(ProjectPath)\bin\artemisdll.def -l $(ProjectPath)\bin\libartemis.a
	@echo Done

$(IntermediateDirectory)/.d:
	@$(MakeDirCommand) "./Release"

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/src_Component$(ObjectSuffix): src/Component.cpp $(IntermediateDirectory)/src_Component$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Sidar/Documents/Sidar/C++/ST/Artemis/src/Component.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_Component$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Component$(DependSuffix): src/Component.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Component$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Component$(DependSuffix) -MM "C:/Users/Sidar/Documents/Sidar/C++/ST/Artemis/src/Component.cpp"

$(IntermediateDirectory)/src_Component$(PreprocessSuffix): src/Component.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Component$(PreprocessSuffix) "C:/Users/Sidar/Documents/Sidar/C++/ST/Artemis/src/Component.cpp"

$(IntermediateDirectory)/src_ComponentType$(ObjectSuffix): src/ComponentType.cpp $(IntermediateDirectory)/src_ComponentType$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Sidar/Documents/Sidar/C++/ST/Artemis/src/ComponentType.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_ComponentType$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_ComponentType$(DependSuffix): src/ComponentType.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_ComponentType$(ObjectSuffix) -MF$(IntermediateDirectory)/src_ComponentType$(DependSuffix) -MM "C:/Users/Sidar/Documents/Sidar/C++/ST/Artemis/src/ComponentType.cpp"

$(IntermediateDirectory)/src_ComponentType$(PreprocessSuffix): src/ComponentType.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_ComponentType$(PreprocessSuffix) "C:/Users/Sidar/Documents/Sidar/C++/ST/Artemis/src/ComponentType.cpp"

$(IntermediateDirectory)/src_ComponentTypeManager$(ObjectSuffix): src/ComponentTypeManager.cpp $(IntermediateDirectory)/src_ComponentTypeManager$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Sidar/Documents/Sidar/C++/ST/Artemis/src/ComponentTypeManager.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_ComponentTypeManager$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_ComponentTypeManager$(DependSuffix): src/ComponentTypeManager.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_ComponentTypeManager$(ObjectSuffix) -MF$(IntermediateDirectory)/src_ComponentTypeManager$(DependSuffix) -MM "C:/Users/Sidar/Documents/Sidar/C++/ST/Artemis/src/ComponentTypeManager.cpp"

$(IntermediateDirectory)/src_ComponentTypeManager$(PreprocessSuffix): src/ComponentTypeManager.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_ComponentTypeManager$(PreprocessSuffix) "C:/Users/Sidar/Documents/Sidar/C++/ST/Artemis/src/ComponentTypeManager.cpp"

$(IntermediateDirectory)/src_EntityProcessingSystem$(ObjectSuffix): src/EntityProcessingSystem.cpp $(IntermediateDirectory)/src_EntityProcessingSystem$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Sidar/Documents/Sidar/C++/ST/Artemis/src/EntityProcessingSystem.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_EntityProcessingSystem$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_EntityProcessingSystem$(DependSuffix): src/EntityProcessingSystem.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_EntityProcessingSystem$(ObjectSuffix) -MF$(IntermediateDirectory)/src_EntityProcessingSystem$(DependSuffix) -MM "C:/Users/Sidar/Documents/Sidar/C++/ST/Artemis/src/EntityProcessingSystem.cpp"

$(IntermediateDirectory)/src_EntityProcessingSystem$(PreprocessSuffix): src/EntityProcessingSystem.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_EntityProcessingSystem$(PreprocessSuffix) "C:/Users/Sidar/Documents/Sidar/C++/ST/Artemis/src/EntityProcessingSystem.cpp"

$(IntermediateDirectory)/src_EntitySystem$(ObjectSuffix): src/EntitySystem.cpp $(IntermediateDirectory)/src_EntitySystem$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Sidar/Documents/Sidar/C++/ST/Artemis/src/EntitySystem.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_EntitySystem$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_EntitySystem$(DependSuffix): src/EntitySystem.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_EntitySystem$(ObjectSuffix) -MF$(IntermediateDirectory)/src_EntitySystem$(DependSuffix) -MM "C:/Users/Sidar/Documents/Sidar/C++/ST/Artemis/src/EntitySystem.cpp"

$(IntermediateDirectory)/src_EntitySystem$(PreprocessSuffix): src/EntitySystem.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_EntitySystem$(PreprocessSuffix) "C:/Users/Sidar/Documents/Sidar/C++/ST/Artemis/src/EntitySystem.cpp"

$(IntermediateDirectory)/src_Entity$(ObjectSuffix): src/Entity.cpp $(IntermediateDirectory)/src_Entity$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Sidar/Documents/Sidar/C++/ST/Artemis/src/Entity.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_Entity$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_Entity$(DependSuffix): src/Entity.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_Entity$(ObjectSuffix) -MF$(IntermediateDirectory)/src_Entity$(DependSuffix) -MM "C:/Users/Sidar/Documents/Sidar/C++/ST/Artemis/src/Entity.cpp"

$(IntermediateDirectory)/src_Entity$(PreprocessSuffix): src/Entity.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_Entity$(PreprocessSuffix) "C:/Users/Sidar/Documents/Sidar/C++/ST/Artemis/src/Entity.cpp"

$(IntermediateDirectory)/src_World$(ObjectSuffix): src/World.cpp $(IntermediateDirectory)/src_World$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Sidar/Documents/Sidar/C++/ST/Artemis/src/World.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_World$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_World$(DependSuffix): src/World.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_World$(ObjectSuffix) -MF$(IntermediateDirectory)/src_World$(DependSuffix) -MM "C:/Users/Sidar/Documents/Sidar/C++/ST/Artemis/src/World.cpp"

$(IntermediateDirectory)/src_World$(PreprocessSuffix): src/World.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_World$(PreprocessSuffix) "C:/Users/Sidar/Documents/Sidar/C++/ST/Artemis/src/World.cpp"

$(IntermediateDirectory)/src_DelayedEntitySystem$(ObjectSuffix): src/DelayedEntitySystem.cpp $(IntermediateDirectory)/src_DelayedEntitySystem$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Sidar/Documents/Sidar/C++/ST/Artemis/src/DelayedEntitySystem.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_DelayedEntitySystem$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_DelayedEntitySystem$(DependSuffix): src/DelayedEntitySystem.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_DelayedEntitySystem$(ObjectSuffix) -MF$(IntermediateDirectory)/src_DelayedEntitySystem$(DependSuffix) -MM "C:/Users/Sidar/Documents/Sidar/C++/ST/Artemis/src/DelayedEntitySystem.cpp"

$(IntermediateDirectory)/src_DelayedEntitySystem$(PreprocessSuffix): src/DelayedEntitySystem.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_DelayedEntitySystem$(PreprocessSuffix) "C:/Users/Sidar/Documents/Sidar/C++/ST/Artemis/src/DelayedEntitySystem.cpp"

$(IntermediateDirectory)/src_DelayedEntityProcessingSystem$(ObjectSuffix): src/DelayedEntityProcessingSystem.cpp $(IntermediateDirectory)/src_DelayedEntityProcessingSystem$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Sidar/Documents/Sidar/C++/ST/Artemis/src/DelayedEntityProcessingSystem.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_DelayedEntityProcessingSystem$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_DelayedEntityProcessingSystem$(DependSuffix): src/DelayedEntityProcessingSystem.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_DelayedEntityProcessingSystem$(ObjectSuffix) -MF$(IntermediateDirectory)/src_DelayedEntityProcessingSystem$(DependSuffix) -MM "C:/Users/Sidar/Documents/Sidar/C++/ST/Artemis/src/DelayedEntityProcessingSystem.cpp"

$(IntermediateDirectory)/src_DelayedEntityProcessingSystem$(PreprocessSuffix): src/DelayedEntityProcessingSystem.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_DelayedEntityProcessingSystem$(PreprocessSuffix) "C:/Users/Sidar/Documents/Sidar/C++/ST/Artemis/src/DelayedEntityProcessingSystem.cpp"

$(IntermediateDirectory)/src_EntityManager$(ObjectSuffix): src/EntityManager.cpp $(IntermediateDirectory)/src_EntityManager$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Sidar/Documents/Sidar/C++/ST/Artemis/src/EntityManager.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_EntityManager$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_EntityManager$(DependSuffix): src/EntityManager.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_EntityManager$(ObjectSuffix) -MF$(IntermediateDirectory)/src_EntityManager$(DependSuffix) -MM "C:/Users/Sidar/Documents/Sidar/C++/ST/Artemis/src/EntityManager.cpp"

$(IntermediateDirectory)/src_EntityManager$(PreprocessSuffix): src/EntityManager.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_EntityManager$(PreprocessSuffix) "C:/Users/Sidar/Documents/Sidar/C++/ST/Artemis/src/EntityManager.cpp"

$(IntermediateDirectory)/src_SystemBitManager$(ObjectSuffix): src/SystemBitManager.cpp $(IntermediateDirectory)/src_SystemBitManager$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Sidar/Documents/Sidar/C++/ST/Artemis/src/SystemBitManager.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_SystemBitManager$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_SystemBitManager$(DependSuffix): src/SystemBitManager.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_SystemBitManager$(ObjectSuffix) -MF$(IntermediateDirectory)/src_SystemBitManager$(DependSuffix) -MM "C:/Users/Sidar/Documents/Sidar/C++/ST/Artemis/src/SystemBitManager.cpp"

$(IntermediateDirectory)/src_SystemBitManager$(PreprocessSuffix): src/SystemBitManager.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_SystemBitManager$(PreprocessSuffix) "C:/Users/Sidar/Documents/Sidar/C++/ST/Artemis/src/SystemBitManager.cpp"

$(IntermediateDirectory)/src_SystemManager$(ObjectSuffix): src/SystemManager.cpp $(IntermediateDirectory)/src_SystemManager$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Sidar/Documents/Sidar/C++/ST/Artemis/src/SystemManager.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_SystemManager$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_SystemManager$(DependSuffix): src/SystemManager.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_SystemManager$(ObjectSuffix) -MF$(IntermediateDirectory)/src_SystemManager$(DependSuffix) -MM "C:/Users/Sidar/Documents/Sidar/C++/ST/Artemis/src/SystemManager.cpp"

$(IntermediateDirectory)/src_SystemManager$(PreprocessSuffix): src/SystemManager.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_SystemManager$(PreprocessSuffix) "C:/Users/Sidar/Documents/Sidar/C++/ST/Artemis/src/SystemManager.cpp"

$(IntermediateDirectory)/src_TagManager$(ObjectSuffix): src/TagManager.cpp $(IntermediateDirectory)/src_TagManager$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Sidar/Documents/Sidar/C++/ST/Artemis/src/TagManager.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_TagManager$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_TagManager$(DependSuffix): src/TagManager.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_TagManager$(ObjectSuffix) -MF$(IntermediateDirectory)/src_TagManager$(DependSuffix) -MM "C:/Users/Sidar/Documents/Sidar/C++/ST/Artemis/src/TagManager.cpp"

$(IntermediateDirectory)/src_TagManager$(PreprocessSuffix): src/TagManager.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_TagManager$(PreprocessSuffix) "C:/Users/Sidar/Documents/Sidar/C++/ST/Artemis/src/TagManager.cpp"

$(IntermediateDirectory)/src_GroupManager$(ObjectSuffix): src/GroupManager.cpp $(IntermediateDirectory)/src_GroupManager$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Sidar/Documents/Sidar/C++/ST/Artemis/src/GroupManager.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_GroupManager$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_GroupManager$(DependSuffix): src/GroupManager.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_GroupManager$(ObjectSuffix) -MF$(IntermediateDirectory)/src_GroupManager$(DependSuffix) -MM "C:/Users/Sidar/Documents/Sidar/C++/ST/Artemis/src/GroupManager.cpp"

$(IntermediateDirectory)/src_GroupManager$(PreprocessSuffix): src/GroupManager.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_GroupManager$(PreprocessSuffix) "C:/Users/Sidar/Documents/Sidar/C++/ST/Artemis/src/GroupManager.cpp"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) $(IntermediateDirectory)/src_Component$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_Component$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_Component$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_ComponentType$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_ComponentType$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_ComponentType$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_ComponentTypeManager$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_ComponentTypeManager$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_ComponentTypeManager$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_EntityProcessingSystem$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_EntityProcessingSystem$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_EntityProcessingSystem$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_EntitySystem$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_EntitySystem$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_EntitySystem$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_Entity$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_Entity$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_Entity$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_World$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_World$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_World$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_DelayedEntitySystem$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_DelayedEntitySystem$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_DelayedEntitySystem$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_DelayedEntityProcessingSystem$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_DelayedEntityProcessingSystem$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_DelayedEntityProcessingSystem$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_EntityManager$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_EntityManager$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_EntityManager$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_SystemBitManager$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_SystemBitManager$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_SystemBitManager$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_SystemManager$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_SystemManager$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_SystemManager$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_TagManager$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_TagManager$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_TagManager$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/src_GroupManager$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/src_GroupManager$(DependSuffix)
	$(RM) $(IntermediateDirectory)/src_GroupManager$(PreprocessSuffix)
	$(RM) $(OutputFile)
	$(RM) $(OutputFile)
	$(RM) "C:\Users\Sidar\Documents\Sidar\C++\ST\.build-dynamic_library\Artemis"


