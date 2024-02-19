#pragma once

#include <type_traits>
#include "tier0/dbg.h"
#include <tier1/utlmap.h>
#include "const.h"
#include "cbaseentity.h"
#include "../Virtual.h"
#include "stdint.h"
#include "cschemasystem.h"
#include "Memory.h"

class CEntityInstance;
class CBasePlayerController;
class Z_CBaseEntity;

struct SchemaKey {
    int16_t offset;
    bool networked;
};

void SetStateChanged(Z_CBaseEntity *pEntity, int offset);

class Schema {
public:
    using SchemaKeyValueMap_t = CUtlMap<uint32_t, SchemaKey>;
    using SchemaTableMap_t = CUtlMap<uint32_t, SchemaKeyValueMap_t*>;

    static constexpr uint32_t val_32_const = 0x811c9dc5;
    static constexpr uint32_t prime_32_const = 0x1000193;
    static constexpr uint64_t val_64_const = 0xcbf29ce484222325;
    static constexpr uint64_t prime_64_const = 0x100000001b3;

    static int16_t FindChainOffset(const char *className);

    static SchemaKey GetOffset(const char *className, uint32_t classKey, const char *memberName, uint32_t memberKey);

    static void SetStateChanged(Z_CBaseEntity *pEntity, int offset);

    static bool InitSchemaFieldsForClass(SchemaTableMap_t *tableMap, const char *className, uint32_t classKey);

    static void (FASTCALL *NetworkStateChanged)(int64 chainEntity, int64 offset, int64 a3);

    static bool IsFieldNetworked(SchemaClassFieldData_t &field);

    static void (FASTCALL *StateChanged)(void *networkTransmitComponent, CEntityInstance *ent, int64 offset, int16 a4, int16 a5);

    static constexpr uint64_t hash_64_fnv1a_const(const char *const str, const uint64_t value = val_64_const) noexcept {
        return (str[0] == '\0') ? value : hash_64_fnv1a_const(&str[1], (value ^ uint64_t(str[0])) * prime_64_const);
    };

    static constexpr uint32_t hash_32_fnv1a_const(const char *const str, const uint32_t value = val_32_const) noexcept {
        return (str[0] == '\0') ? value : hash_32_fnv1a_const(&str[1], (value ^ uint32_t(str[0])) * prime_32_const);
    };
};

#define DECLARE_SCHEMA_CLASS(className) DECLARE_SCHEMA_CLASS_BASE(className, false)

#define DECLARE_SCHEMA_CLASS_INLINE(className) DECLARE_SCHEMA_CLASS_BASE(className, true)

#define DECLARE_SCHEMA_CLASS_BASE(className, isStruct)            \
    typedef className ThisClass;                                \
    static constexpr const char *ThisClassName = #className;    \
    static constexpr bool IsStruct = isStruct;

// Use this when you want the member's value itself
#define SCHEMA_FIELD(type, varName) \
    SCHEMA_FIELD_OFFSET(type, varName, 0)

// Use this when you want a pointer to a member
#define SCHEMA_FIELD_POINTER(type, varName) \
    SCHEMA_FIELD_POINTER_OFFSET(type, varName, 0)

#define SCHEMA_FIELD_OFFSET(type, varName, extra_offset)                                                                                                          \
    class varName##_prop                                                                                                                                          \
    {                                                                                                                                                             \
    public:                                                                                                                                                       \
        std::add_lvalue_reference_t<type> Get()                                                                                                                   \
        {                                                                                                                                                         \
            static constexpr auto datatable_hash = Schema::hash_32_fnv1a_const(ThisClassName);                                                                            \
            static constexpr auto prop_hash = Schema::hash_32_fnv1a_const(#varName);                                                                                      \
                                                                                                                                                                  \
            static const auto m_key =                                                                                                                             \
                Schema::GetOffset(ThisClassName, datatable_hash, #varName, prop_hash);                                                                               \
                                                                                                                                                                  \
            static const size_t offset = offsetof(ThisClass, varName);                                                                                            \
            ThisClass *pThisClass = (ThisClass *)((byte *)this - offset);                                                                                         \
                                                                                                                                                                  \
            return *reinterpret_cast<std::add_pointer_t<type>>(                                                                                                   \
                (uintptr_t)(pThisClass) + m_key.offset + extra_offset);                                                                                           \
        }                                                                                                                                                         \
        void Set(type val)                                                                                                                                        \
        {                                                                                                                                                         \
            static constexpr auto datatable_hash = Schema::hash_32_fnv1a_const(ThisClassName);                                                                            \
            static constexpr auto prop_hash = Schema::hash_32_fnv1a_const(#varName);                                                                                      \
                                                                                                                                                                  \
            static const auto m_key =                                                                                                                             \
                Schema::GetOffset(ThisClassName, datatable_hash, #varName, prop_hash);                                                                               \
                                                                                                                                                                  \
            static const auto m_chain =                                                                                                                           \
                Schema::FindChainOffset(ThisClassName);                                                                                                              \
                                                                                                                                                                  \
            static const size_t offset = offsetof(ThisClass, varName);                                                                                            \
            ThisClass *pThisClass = (ThisClass *)((byte *)this - offset);                                                                                         \
                                                                                                                                                                  \
            if (m_chain != 0 && m_key.networked)																			\
			{																												\
				SignatureCall::NetworkStateChanged((uintptr_t)(pThisClass) + m_chain, m_key.offset + extra_offset, 0xFFFFFFFF);	\
			}																												\
			else if(m_key.networked)																						\
			{																												\
				/* WIP: Works fine for most props, but inlined classes in the middle of a class will
					need to have their this pointer corrected by the offset .*/												\
				if (!IsStruct)																								\
					SetStateChanged((Z_CBaseEntity*)pThisClass, m_key.offset + extra_offset);								\
				else if (IsPlatformPosix()) /* This is currently broken on windows */										\
					CALL_VIRTUAL(void, 1, pThisClass, m_key.offset + extra_offset, 0xFFFFFFFF, 0xFFFF);						\
			}																												\
			*reinterpret_cast<std::add_pointer_t<type>>((uintptr_t)(pThisClass) + m_key.offset + extra_offset) = val;		\
        }                                                                                                                                                         \
        operator std::add_lvalue_reference_t<type>() { return Get(); }                                                                                            \
        std::add_lvalue_reference_t<type> operator()() { return Get(); }                                                                                          \
        std::add_lvalue_reference_t<type> operator->() { return Get(); }                                                                                          \
        void operator()(type val) { Set(val); }                                                                                                                   \
        void operator=(type val) { Set(val); }                                                                                                                    \
    } varName;

#define SCHEMA_FIELD_POINTER_OFFSET(type, varName, extra_offset)                                                            \
    class varName##_prop                                                                                                    \
    {                                                                                                                        \
    public:                                                                                                                    \
        type *Get()                                                                                                            \
        {                                                                                                                    \
            static constexpr auto datatable_hash = Schema::hash_32_fnv1a_const(ThisClassName);                                        \
            static constexpr auto prop_hash = Schema::hash_32_fnv1a_const(#varName);                                                \
                                                                                                                            \
            static const auto m_key =                                                                                        \
                Schema::GetOffset(ThisClassName, datatable_hash, #varName, prop_hash);                                        \
                                                                                                                            \
            static const size_t offset = offsetof(ThisClass, varName);                                                        \
            ThisClass *pThisClass = (ThisClass *)((byte *)this - offset);                                                    \
                                                                                                                            \
            return reinterpret_cast<std::add_pointer_t<type>>(                                                                \
                (uintptr_t)(pThisClass) + m_key.offset + extra_offset);                                                        \
        }                                                                                                                    \
        operator type*() { return Get(); }                                                                                    \
        type* operator ()() { return Get(); }                                                                                \
        type* operator->() { return Get(); }                                                                                \
    } varName;