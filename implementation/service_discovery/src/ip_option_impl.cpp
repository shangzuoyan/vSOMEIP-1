// Copyright (C) 2014-2015 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <vsomeip/constants.hpp>

#include "../include/constants.hpp"
#include "../include/ip_option_impl.hpp"
#include "../../message/include/deserializer.hpp"
#include "../../message/include/serializer.hpp"

namespace vsomeip {
namespace sd {

ip_option_impl::ip_option_impl() :
        protocol_(layer_four_protocol_e::UNKNOWN),
        port_(0xFFFF) {
}

ip_option_impl::~ip_option_impl() {
}

bool ip_option_impl::operator ==(const option_impl &_other) const {
    if (type_ != _other.get_type())
        return false;

#ifdef VSOMEIP_TODO
    const ip_option_impl & other =
            dynamic_cast<const ip_option_impl &>(_other);
#endif
    return true;
}

unsigned short ip_option_impl::get_port() const {
    return port_;
}

void ip_option_impl::set_port(unsigned short _port) {
    port_ = _port;
}

layer_four_protocol_e ip_option_impl::get_layer_four_protocol() const {
    return protocol_;
}

void ip_option_impl::set_layer_four_protocol(
        layer_four_protocol_e _protocol) {
    protocol_ = _protocol;
}

bool ip_option_impl::is_multicast() const {
    return (type_ == option_type_e::IP4_MULTICAST
            || type_ == option_type_e::IP6_MULTICAST);
}

} // namespace sd
} // namespace vsomeip

