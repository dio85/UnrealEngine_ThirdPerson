// Copyright 2018 wevet works All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class WevetTarget : TargetRules
{
	public WevetTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
        CppStandard = CppStandardVersion.Cpp20;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.AddRange( new string[] { "Wevet" } );
	}
}
