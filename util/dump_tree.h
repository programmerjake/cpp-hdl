/**
 * Copyright 2018 Jacob Lifshay
 *
 * This file is part of Cpp-HDL.
 *
 * Cpp-HDL is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Cpp-HDL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Cpp-HDL.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <unordered_map>
#include <deque>
#include <memory>
#include <string>
#include <sstream>
#include "string_view.h"
#include "string_pool.h"
#include "arena.h"
#include <type_traits>
#include <vector>

namespace util
{
struct DumpTree final
{
    std::unordered_map<StringPool::Entry, std::string> simpleVariables{};
    std::unordered_map<StringPool::Entry, const DumpTree *> pointerVariables{};
    string_view nodeName;
    static void writeJSON(std::ostream &os, const DumpTree *tree);
    static std::string convertToJSON(const DumpTree *tree)
    {
        std::ostringstream ss;
        writeJSON(ss, tree);
        return ss.str();
    }
};

class DumpState;

namespace dump_tree_detail
{
template <typename T>
void utilDumpFunction(const T *) = delete;
}

template <typename T>
struct DumpFunctor
{
    void operator()(const T *sourceNode, DumpTree *dumpNode, DumpState &state) const
    {
        using dump_tree_detail::utilDumpFunction;
        utilDumpFunction(sourceNode, dumpNode, state);
    }
};

class DumpState final
{
private:
    Arena &arena;
    StringPool &stringPool;
    struct WorkQueueItem
    {
        const void *sourceNode;
        DumpTree *dumpNode;
        typedef void (*Callback)(const void *sourceNode, DumpTree *dumpNode, DumpState &state);
        Callback callback;
        constexpr WorkQueueItem(const void *sourceNode,
                                DumpTree *dumpNode,
                                Callback callback) noexcept : sourceNode(sourceNode),
                                                              dumpNode(dumpNode),
                                                              callback(callback)
        {
        }
    };
    std::deque<WorkQueueItem> workQueue;
    bool working = false;
    struct PushWorking
    {
        bool &working;
        bool oldWorking;
        PushWorking(DumpState *dumpState, bool newWorking) noexcept : working(dumpState->working),
                                                                      oldWorking(working)
        {
            working = newWorking;
        }
        ~PushWorking()
        {
            working = oldWorking;
        }
    };
    std::unordered_map<const void *, const DumpTree *> sourceNodeToDumpNodeMap;

private:
    const DumpTree *getDumpNodeTypeErased(const void *sourceNode, WorkQueueItem::Callback callback)
    {
        if(!sourceNode)
            return nullptr;
        auto &retval = sourceNodeToDumpNodeMap[sourceNode];
        if(retval)
            return retval;
        auto *dumpNode = arena.create<DumpTree>();
        retval = dumpNode;
        workQueue.push_back({sourceNode, dumpNode, callback});
        if(!working)
        {
            PushWorking pushWorking(this, true);
            while(!workQueue.empty())
            {
                auto item = workQueue.back();
                workQueue.pop_back();
                item.callback(item.sourceNode, item.dumpNode, *this);
            }
        }
        return retval;
    }
    void setSimpleTypeErased(DumpTree *dumpNode, string_view memberName, std::string value)
    {
        dumpNode->simpleVariables[stringPool.intern(memberName)] = std::move(value);
    }
    void setPointerTypeErased(DumpTree *dumpNode, string_view memberName, const DumpTree *value)
    {
        dumpNode->pointerVariables[stringPool.intern(memberName)] = value;
    }
    template <typename T,
              typename = std::enable_if_t<std::is_convertible<T &&, std::string>::value
                                          && !std::is_pointer<std::remove_reference_t<T>>::value>>
    static std::string valueToString(T &&value)
    {
        return static_cast<std::string>(std::forward<T>(value));
    }
    template <typename T,
              typename = std::enable_if_t<!std::is_convertible<T &&, std::string>::value
                                          && !std::is_pointer<std::remove_reference_t<T>>::value>,
              typename = void>
    static std::string valueToString(T &&value)
    {
        std::ostringstream ss;
        ss << std::boolalpha << std::forward<T>(value);
        return ss.str();
    }
    static std::string valueToString(const StringPool::Entry &value)
    {
        if(!value)
            return "<nullptr>";
        return *value;
    }
    static std::string getNameWithIndex(string_view memberName, std::size_t index)
    {
        std::ostringstream ss;
        ss << memberName << "[" << index << "]";
        return ss.str();
    }
    void setSimpleIndexedTypeErased(DumpTree *dumpNode,
                                    string_view memberName,
                                    std::size_t index,
                                    std::string value)
    {
        setSimpleTypeErased(dumpNode, getNameWithIndex(memberName, index), std::move(value));
    }
    void setPointerIndexedTypeErased(DumpTree *dumpNode,
                                     string_view memberName,
                                     std::size_t index,
                                     const DumpTree *value)
    {
        setPointerTypeErased(dumpNode, getNameWithIndex(memberName, index), value);
    }

public:
    explicit DumpState(Arena &arena, StringPool &stringPool) : arena(arena), stringPool(stringPool)
    {
    }
    template <typename T>
    const DumpTree *getDumpNode(const T *sourceNode)
    {
        return getDumpNodeTypeErased(
            static_cast<const void *>(sourceNode),
            [](const void *sourceNode, DumpTree *dumpNode, DumpState &state)
            {
                DumpFunctor<T>()(static_cast<const T *>(sourceNode), dumpNode, state);
            });
    }
    template <typename T>
    void setSimple(DumpTree *dumpNode, string_view memberName, T &&value)
    {
        setSimpleTypeErased(dumpNode, memberName, valueToString(std::forward<T>(value)));
    }
    template <typename T>
    void setPointer(DumpTree *dumpNode, string_view memberName, const T *value)
    {
        setPointerTypeErased(dumpNode, memberName, getDumpNode(value));
    }
    template <typename T>
    void setSimpleIndexed(DumpTree *dumpNode, string_view memberName, std::size_t index, T &&value)
    {
        setSimpleIndexedTypeErased(
            dumpNode, memberName, index, valueToString(std::forward<T>(value)));
    }
    template <typename T>
    void setPointerIndexed(DumpTree *dumpNode,
                           string_view memberName,
                           std::size_t index,
                           const T *value)
    {
        setPointerIndexedTypeErased(dumpNode, memberName, index, getDumpNode(value));
    }
    template <typename T>
    void setSimpleArray(DumpTree *dumpNode, string_view memberName, std::size_t index, T &&value)
    {
        setSimpleIndexedTypeErased(dumpNode, memberName, valueToString(std::forward<T>(value)));
    }
    template <typename T>
    void setSimpleIndexed(DumpTree *dumpNode, string_view memberName, const std::vector<T> &value)
    {
        for(std::size_t index = 0; index < value.size(); index++)
            setPointerIndexed(dumpNode, memberName, index, value[index]);
    }
    template <typename T>
    void setPointerArray(DumpTree *dumpNode, string_view memberName, const std::vector<T *> &value)
    {
        for(std::size_t index = 0; index < value.size(); index++)
            setPointerIndexed(dumpNode, memberName, index, value[index]);
    }
};
}
