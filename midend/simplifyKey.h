/*
Copyright 2016 VMware, Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef _MIDEND_SIMPLIFYKEY_H_
#define _MIDEND_SIMPLIFYKEY_H_

#include "ir/ir.h"
#include "frontends/common/resolveReferences/referenceMap.h"
#include "frontends/p4/typeMap.h"

namespace P4 {

// Policy used to decide whether a key expression is too complex.
class KeyIsComplex {
 public:
    virtual ~KeyIsComplex() {}
    virtual bool isTooComplex(const IR::Expression* expression) const = 0;
};

// Policy which returns 'true' whenever a key is not a left value
// or a call to the isValid().
class NonLeftValue : public KeyIsComplex {
    const ReferenceMap* refMap;
    const TypeMap*      typeMap;
 public:
    NonLeftValue(const ReferenceMap* refMap, const TypeMap* typeMap) :
            refMap(refMap), typeMap(typeMap)
    { CHECK_NULL(refMap); CHECK_NULL(typeMap); }
    bool isTooComplex(const IR::Expression* expression) const;
};

class TableInsertions {
 public:
    std::vector<const IR::Declaration_Variable*> declarations;
    std::vector<const IR::AssignmentStatement*>  statements;
};

// If some of the fields of a table key are "too complex",
// turn them into simpler expressions.
class SimplifyKey : public Transform {
    ReferenceMap*       refMap;
    const TypeMap*      typeMap;
    const KeyIsComplex* policy;
    std::map<const IR::P4Table*, TableInsertions*> toInsert;
 public:
    SimplifyKey(ReferenceMap* refMap, const TypeMap* typeMap, const KeyIsComplex* policy) :
            refMap(refMap), typeMap(typeMap), policy(policy)
    { CHECK_NULL(refMap); CHECK_NULL(typeMap); CHECK_NULL(policy); setName("SimplifyKey"); }
    const IR::Node* doStatement(const IR::Statement* statement, const IR::Expression* expression);

    // These should be all kinds of statements that may contain a table apply
    // after the program has been simplified
    const IR::Node* postorder(IR::MethodCallStatement* statement) override
    { return doStatement(statement, statement->methodCall); }
    const IR::Node* postorder(IR::IfStatement* statement) override
    { return doStatement(statement, statement->condition); }
    const IR::Node* postorder(IR::SwitchStatement* statement) override
    { return doStatement(statement, statement->expression); }
    const IR::Node* postorder(IR::KeyElement* element) override;
    const IR::Node* postorder(IR::P4Table* table) override;
};

}  // namespace P4

#endif /* _MIDEND_SIMPLIFYKEY_H_ */
