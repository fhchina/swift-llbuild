//===- Manifest.h -----------------------------------------------*- C++ -*-===//
//
// This source file is part of the Swift.org open source project
//
// Copyright (c) 2014 - 2015 Apple Inc. and the Swift project authors
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See http://swift.org/LICENSE.txt for license information
// See http://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
//
//===----------------------------------------------------------------------===//

#ifndef LLBUILD_NINJA_MANIFEST_H
#define LLBUILD_NINJA_MANIFEST_H

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace llbuild {
namespace ninja {

/// This class represents a set of name to value variable bindings.
class BindingSet {
  /// The parent binding scope, if any.
  const BindingSet *ParentScope = 0;

  /// The actual bindings, mapping from Name to Value.
  std::unordered_map<std::string, std::string> Entries;

public:
  BindingSet(const BindingSet* ParentScope = 0) : ParentScope(ParentScope) {}

  /// Get the parent scope.
  const BindingSet* getParentScope() const {
    return ParentScope;
  }

  /// Get the map of bindings.
  const std::unordered_map<std::string, std::string>& getEntries() const {
    return Entries;
  }

  /// Insert a binding into the set.
  void insert(const std::string& Name, const std::string& Value) {
    Entries[Name] = Value;
  }

  /// Look up the given variable name in the binding set, returning its value or
  /// the empty string if not found.
  std::string lookup(const std::string& Name) const {
    auto it = Entries.find(Name);
    if (it != Entries.end())
      return it->second;

    if (ParentScope)
      return ParentScope->lookup(Name);

    return "";
  }
};

/// A node represents a unique path as present in the manifest.
//
// FIXME: Figure out what the deal is with normalization.
class Node {
  std::string Path;

public:
  explicit Node(const std::string& Path) : Path(Path) {}

  const std::string& getPath() const { return Path; }
};

/// A pool represents a generic bucket for organizing commands.
class Pool {
  /// The name of the pool.
  std::string Name;

  /// The pool depth, or 0 if unspecified.
  uint32_t Depth = 0;

public:
  explicit Pool(const std::string& Name) : Name(Name) {}

  const std::string& getName() const { return Name; }

  uint32_t getDepth() const {
    return Depth;
  }
  void setDepth(uint32_t Value) {
    Depth = Value;
  }
};

/// A rule represents a template which can be expanded to produce a particular
/// command.
class Rule {
  /// The name of the rule.
  std::string Name;

  /// The rule parameters, which are all unexpanded string exprs.
  //
  // FIXME: It would be nice to optimize this more, and the common case is that
  // we have a fixed set of values which are never dynamically expanded for most
  // parameters *other* than the command.
  std::unordered_map<std::string, std::string> Parameters;

public:
  explicit Rule(const std::string& Name) : Name(Name) {}

  const std::string& getName() const { return Name; }

  std::unordered_map<std::string, std::string>& getParameters() {
    return Parameters;
  }
  const std::unordered_map<std::string, std::string>& getParameters() const {
    return Parameters;
  }

  /// Check whether the given string is a valid rule parameter.
  static bool isValidParameterName(const std::string& Name);
};

/// A manifest represents the complete set of rules and commands used to perform
/// a build.
class Manifest {
  /// The top level variable bindings.
  BindingSet Bindings;

  /// The nodes in the manifest, stored as a map on the node name.
  //
  // FIXME: This is an inefficent map, given that the string is contained
  // inside the node.
  typedef std::unordered_map<std::string, std::unique_ptr<Node>> node_set;
  node_set Nodes;

  /// The pools in the manifest, stored as a map on the pool name.
  //
  // FIXME: This is an inefficent map, given that the string is contained
  // inside the pool.
  typedef std::unordered_map<std::string, std::unique_ptr<Pool>> pool_set;
  pool_set Pools;

  /// The rules in the manifest, stored as a map on the rule name.
  //
  // FIXME: This is an inefficent map, given that the string is contained
  // inside the rule.
  typedef std::unordered_map<std::string, std::unique_ptr<Rule>> rule_set;
  rule_set Rules;

  /// The default targets, if specified.
  std::vector<Node*> DefaultTargets;

public:
  /// Get the final set of top level variable bindings.
  BindingSet& getBindings() { return Bindings; }
  /// Get the final set of top level variable bindings.
  const BindingSet& getBindings() const { return Bindings; }

  node_set& getNodes() {
    return Nodes;
  }
  const node_set& getNodes() const {
    return Nodes;
  }

  pool_set& getPools() {
    return Pools;
  }
  const pool_set& getPools() const {
    return Pools;
  }

  rule_set& getRules() {
    return Rules;
  }
  const rule_set& getRules() const {
    return Rules;
  }

  std::vector<Node*>& getDefaultTargets() {
    return DefaultTargets;
  }
  const std::vector<Node*>& getDefaultTargets() const {
    return DefaultTargets;
  }
};

}
}

#endif