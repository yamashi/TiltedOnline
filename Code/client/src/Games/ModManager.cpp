#include <Games/TES.h>

#include <Games/Fallout4/Actor.h>
#include <Games/Fallout4/Misc/NEW_REFR_DATA.h>

#include <Games/Skyrim/Actor.h>

ModManager* ModManager::Get() noexcept
{
    POINTER_FALLOUT4(ModManager*, modManager, 0x1458CF080 - 0x140000000);
    POINTER_SKYRIMSE(ModManager*, modManager, 0x141EBE428 - 0x140000000);
    
    return *modManager.Get();
}

uint32_t Mod::GetFormId(uint32_t aBaseId) const noexcept
{
    if (IsLite())
    {
        aBaseId &= 0xFFF;
        aBaseId |= 0xFE000000;
        aBaseId |= uint32_t(liteId) << 12;
    }
    else
    {
        aBaseId &= 0xFFFFFF;
        aBaseId |= uint32_t(standardId) << 24;
    }

    return aBaseId;
}


#if TP_FALLOUT4

TP_THIS_FUNCTION(TSpawnNewREFR, uint32_t&, ModManager, uint32_t& refHandle, NEW_REFR_DATA* apData);
TSpawnNewREFR* RealSpawnNewREFR;

uint32_t& TP_MAKE_THISCALL(SpawnNewREFR, ModManager, uint32_t& refHandle, NEW_REFR_DATA* apData)
{
    TP_EMPTY_HOOK_PLACEHOLDER;

    return RealSpawnNewREFR(apThis, refHandle, apData);
}

#elif TP_SKYRIM64

TP_THIS_FUNCTION(TSpawnNewREFR, uint32_t&, ModManager, uint32_t& aRefHandleOut, TESForm* apBaseForm, NiPoint3* apPosition, NiPoint3* apRotation, TESObjectCELL* apParentCell, TESWorldSpace* apWorldSpace, Actor* apActor, uintptr_t a9, uintptr_t a10, char aForcePersist, char a12);
TSpawnNewREFR* RealSpawnNewREFR;

uint32_t& TP_MAKE_THISCALL(SpawnNewREFR, ModManager, uint32_t& aRefHandleOut, TESForm* apBaseForm, NiPoint3* apPosition, NiPoint3* apRotation, TESObjectCELL* apParentCell, TESWorldSpace* apWorldSpace, Actor* apActor, uintptr_t a9, uintptr_t a10, char aForcePersist, char a12)
{
    TP_EMPTY_HOOK_PLACEHOLDER;

    return ThisCall(RealSpawnNewREFR, apThis, aRefHandleOut, apBaseForm, apPosition, apRotation, apParentCell, apWorldSpace, apActor, a9, a10, aForcePersist, a12);
}

#else

using TSpawnNewREFR = void(__cdecl)(uint32_t& aRefHandleOut, TESForm* apBaseForm, NiPoint3* apPosition, NiPoint3* apRotation, TESObjectCELL* apParentCell, TESWorldSpace* apWorldSpace, Actor* apActor, uintptr_t a9, uintptr_t a10, char aForcePersist, char a12);
TSpawnNewREFR* RealSpawnNewREFR;

void SpawnNewREFR(uint32_t& aRefHandleOut, TESForm* apBaseForm, NiPoint3* apPosition, NiPoint3* apRotation, TESObjectCELL* apParentCell, TESWorldSpace* apWorldSpace, Actor* apActor, uintptr_t a9, uintptr_t a10, char aForcePersist, char a12)
{
    TP_EMPTY_HOOK_PLACEHOLDER;

    RealSpawnNewREFR(aRefHandleOut, apBaseForm, apPosition, apRotation, apParentCell, apWorldSpace, apActor, a9, a10, aForcePersist, a12);
}

#endif

uint32_t ModManager::Spawn(NiPoint3& aPosition, NiPoint3& aRotation, TESObjectCELL* apParentCell, TESWorldSpace* apWorldSpace, Actor* apCharacter) noexcept
{
    uint32_t refrHandle = 0;

#if TP_FALLOUT4
    NEW_REFR_DATA refrData;
    refrData.postion = aPosition;
    refrData.rotation = aRotation;
    refrData.baseForm = apCharacter->baseForm;
    refrData.refrToPlace = apCharacter;
    refrData.parentCell = apParentCell;
    refrData.worldspace = apWorldSpace;

    ThisCall(RealSpawnNewREFR, this, refrHandle, &refrData);
#elif TP_SKYRIM64
    ThisCall(RealSpawnNewREFR, this, refrHandle, apCharacter->baseForm, &aPosition, &aRotation, apParentCell, apWorldSpace, apCharacter, 0, 0, static_cast<char>(0), static_cast<char>(1));
#else
    RealSpawnNewREFR(refrHandle, apCharacter->baseForm, &aPosition, &aRotation, apParentCell, apWorldSpace, apCharacter, 0, 0, static_cast<char>(0), static_cast<char>(1));
#endif

    return refrHandle;
}

Mod* ModManager::GetByName(const char* acpName) const noexcept
{
    auto pEntry = &mods.entry;

    while(pEntry && pEntry->data)
    {
        if (stricmp(acpName, pEntry->data->filename) == 0)
            return pEntry->data;

        pEntry = pEntry->next;
    }

	return nullptr;
}

static TiltedPhoques::Initializer s_tesHooks([]()
{
    POINTER_FALLOUT4(TSpawnNewREFR, s_realSpawnNewREFR, 0x1401140B0 - 0x140000000);
    POINTER_SKYRIMSE(TSpawnNewREFR, s_realSpawnNewREFR, 0x14016C210 - 0x140000000);

    RealSpawnNewREFR = s_realSpawnNewREFR.Get();

    TP_HOOK(&RealSpawnNewREFR, SpawnNewREFR);
});
