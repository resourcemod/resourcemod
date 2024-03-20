//#include <unordered_map>
#include "tier1/utlmap.h"
#include "tier0/memdbgon.h"
#include "cbaseentity.h"
#include "cschemasystem.h"

extern CSchemaSystem* g_SchemaSystem;
extern IVEngineServer2* g_SourceEngine;

bool Schema::IsFieldNetworked(SchemaClassFieldData_t &field)
{
    for (int i = 0; i < field.m_metadata_size; i++)
    {
        static auto networkEnabled = hash_32_fnv1a_const("MNetworkEnable");
        if (networkEnabled == hash_32_fnv1a_const(field.m_metadata[i].m_name))
            return true;
    }

    return false;
}

bool Schema::InitSchemaFieldsForClass(SchemaTableMap_t *tableMap, const char *className, uint32_t classKey)
{
    std::string module = "server";
    std::string prefix = "";
    CSchemaSystemTypeScope *pType = g_SchemaSystem->FindTypeScopeForModule(prefix.append(MODULE_PREFIX).append(module.c_str()).append(MODULE_EXT).c_str());

    if (!pType)
        return false;

    SchemaClassInfoData_t *pClassInfo = pType->FindDeclaredClass(className);

    if (!pClassInfo)
    {
        SchemaKeyValueMap_t *map = new SchemaKeyValueMap_t(0, 0, DefLessFunc(uint32_t));
        tableMap->Insert(classKey, map);
        return false;
    }

    short fieldsSize = pClassInfo->GetFieldsSize();
    SchemaClassFieldData_t *pFields = pClassInfo->GetFields();

    SchemaKeyValueMap_t *keyValueMap = new SchemaKeyValueMap_t(0, 0, DefLessFunc(uint32_t));
    keyValueMap->EnsureCapacity(fieldsSize);
    tableMap->Insert(classKey, keyValueMap);

    for (int i = 0; i < fieldsSize; ++i)
    {
        SchemaClassFieldData_t &field = pFields[i];
        keyValueMap->Insert(hash_32_fnv1a_const(field.m_name), { field.m_offset, Schema::IsFieldNetworked(field) });
    }

    return true;
}

int16_t Schema::FindChainOffset(const char *className)
{
    std::string module = "server";
    std::string prefix = "";
    CSchemaSystemTypeScope *pType = g_SchemaSystem->FindTypeScopeForModule(prefix.append(MODULE_PREFIX).append(module.c_str()).append(MODULE_EXT).c_str());

    if (!pType)
        return false;

    SchemaClassInfoData_t *pClassInfo = pType->FindDeclaredClass(className);

    do
    {
        SchemaClassFieldData_t *pFields = pClassInfo->GetFields();
        short fieldsSize = pClassInfo->GetFieldsSize();
        for (int i = 0; i < fieldsSize; ++i)
        {
            SchemaClassFieldData_t &field = pFields[i];

            if (V_strcmp(field.m_name, "__m_pChainEntity") == 0)
            {
                return field.m_offset;
            }
        }
    } while ((pClassInfo = pClassInfo->GetParent()) != nullptr);

    return 0;
}

SchemaKey Schema::GetOffset(const char *className, uint32_t classKey, const char *memberName, uint32_t memberKey)
{
    static SchemaTableMap_t schemaTableMap(0, 0, DefLessFunc(uint32_t));
    int16_t tableMapIndex = schemaTableMap.Find(classKey);
    if (!schemaTableMap.IsValidIndex(tableMapIndex))
    {
        if (InitSchemaFieldsForClass(&schemaTableMap, className, classKey))
            return GetOffset(className, classKey, memberName, memberKey);

        return { 0, 0 };
    }

    SchemaKeyValueMap_t *tableMap = schemaTableMap[tableMapIndex];
    int16_t memberIndex = tableMap->Find(memberKey);
    if (!tableMap->IsValidIndex(memberIndex))
    {
        return { 0, 0 };
    }

    return tableMap->Element(memberIndex);
}

void SetStateChanged(Z_CBaseEntity* pEntity, int offset)
{
    SignatureCall::StateChanged(pEntity->m_NetworkTransmitComponent(), pEntity, offset, -1, -1);

    pEntity->m_lastNetworkChange = g_SourceEngine->GetServerGlobals()->curtime;
    pEntity->m_isSteadyState().ClearAll();
};
