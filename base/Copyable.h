#ifndef TINYNET_NET_COPYABLE_H
#define TINYNET_NET_COPYABLE_H

namespace xy
{

/// A tag class emphasises the objects are copyable.
/// The empty base class optimization applies.
/// Any derived class of copyable should be a value type.
class Copyable
{
 protected:
  Copyable() = default;
  ~Copyable() = default;
};

}  // namespace xy

#endif  // TINYNET_NET_COPYABLE_H
