// Copyright (C) 2014-2015 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef VSOMEIP_LOCAL_CLIENT_ENDPOINT_IMPL_HPP
#define VSOMEIP_LOCAL_CLIENT_ENDPOINT_IMPL_HPP

#include <boost/asio/io_service.hpp>
#include <boost/asio/local/stream_protocol.hpp>

#ifdef WIN32
#include <boost/asio/ip/tcp.hpp>
#endif

#include <vsomeip/defines.hpp>

#include "client_endpoint_impl.hpp"

namespace vsomeip {

#ifdef WIN32
typedef client_endpoint_impl<boost::asio::ip::tcp,
        VSOMEIP_MAX_TCP_MESSAGE_SIZE > local_client_endpoint_base_impl;
#else
typedef client_endpoint_impl<boost::asio::local::stream_protocol,
        VSOMEIP_MAX_LOCAL_MESSAGE_SIZE> local_client_endpoint_base_impl;
#endif

class local_client_endpoint_impl: public local_client_endpoint_base_impl {
public:
    local_client_endpoint_impl(std::shared_ptr<endpoint_host> _host,
                               endpoint_type _local,
                               boost::asio::io_service &_io,
                               std::uint32_t _max_message_size);

    virtual ~local_client_endpoint_impl();

    void start();

    bool is_local() const;

private:
    void send_queued();

    void send_start_tag();
    void send_queued_data();
    void send_end_tag();

    void send_magic_cookie();

    void connect();
    void receive();

    void send_start_tag_cbk(boost::system::error_code const &_error,
                            std::size_t _bytes);
    void send_queued_data_cbk(boost::system::error_code const &_error,
                              std::size_t _bytes);
    void receive_cbk(boost::system::error_code const &_error,
                     std::size_t _bytes);
};

} // namespace vsomeip

#endif // VSOMEIP_LOCAL_CLIENT_ENDPOINT_IMPL_HPP
