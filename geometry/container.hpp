/** TRACCC library, part of the ACTS project (R&D line)
 *
 * (c) 2021-2022 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */

#pragma once

// Project include(s).
#include "host_container.hpp"
#include "shared_definitions.hpp"

// System include(s).
#include <type_traits>

namespace traccc {

/// @name Types used to send data back and forth between host and device code
/// @{

/// Type trait defining all "collection types" for an EDM class
template <typename item_t>
struct collection_types {

    /// @c item_t must not be a constant type
    static_assert(std::is_const<item_t>::value == false,
                  "The template parameter must not be a constant type");

    /// Host collection for @c item_t
    using host = vecmem::vector<item_t>;

};  // struct collection_types

/// Type trait defining all "container types" for an EDM class pair
template <typename header_t, typename item_t>
struct container_types {

    /// @c header_t must not be a constant type
    static_assert(std::is_const<header_t>::value == false,
                  "The header type must not be constant");
    /// @c item_t must not be a constant type
    static_assert(std::is_const<item_t>::value == false,
                  "The item type must not be constant");

    /// Host container for @c header_t and @c item_t
    using host = host_container<header_t, item_t>;

};  // struct container_types

}  // namespace traccc
