// Copyright (C) 2014-2015 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef VSOMEIP_SERVICE_DISCOVERY_IMPL
#define VSOMEIP_SERVICE_DISCOVERY_IMPL

#include <map>
#include <memory>
#include <mutex>
#include <set>

#include <boost/asio/system_timer.hpp>

#include "service_discovery.hpp"
#include "../../routing/include/types.hpp"
#include "ip_option_impl.hpp"

namespace vsomeip {

class endpoint;
class serializer;

namespace sd {

class entry_impl;
class eventgroupentry_impl;
class option_impl;
class request;
class serviceentry_impl;
class service_discovery_fsm;
class service_discovery_host;
class subscription;

typedef std::map<service_t, std::map<instance_t, std::shared_ptr<request> > > requests_t;

class service_discovery_impl: public service_discovery,
        public std::enable_shared_from_this<service_discovery_impl> {
public:
    service_discovery_impl(service_discovery_host *_host);
    virtual ~service_discovery_impl();

    std::shared_ptr<configuration> get_configuration() const;
    boost::asio::io_service & get_io();

    void init();
    void start();
    void stop();

    void request_service(service_t _service, instance_t _instance,
            major_version_t _major, minor_version_t _minor, ttl_t _ttl);
    void release_service(service_t _service, instance_t _instance);

    void subscribe(service_t _service, instance_t _instance,
            eventgroup_t _eventgroup, major_version_t _major, ttl_t _ttl, client_t _client,
            subscription_type_e _subscription_type);
    void unsubscribe(service_t _service, instance_t _instance,
            eventgroup_t _eventgroup, client_t _client);
    void unsubscribe_all(service_t _service, instance_t _instance);

    void send(bool _is_announcing);

    void on_message(const byte_t *_data, length_t _length,
            const boost::asio::ip::address &_sender);

    void on_offer_change();

private:
    std::pair<session_t, bool> get_session(const boost::asio::ip::address &_address);
    void increment_session(const boost::asio::ip::address &_address);

    bool is_reboot(const boost::asio::ip::address &_address,
            bool _reboot_flag, session_t _session);

    void insert_option(std::shared_ptr<message_impl> &_message,
            std::shared_ptr<entry_impl> _entry,
            const boost::asio::ip::address &_address, uint16_t _port,
            bool _is_reliable);
    void insert_find_entries(std::shared_ptr<message_impl> &_message,
            requests_t &_requests);
    void insert_offer_entries(std::shared_ptr<message_impl> &_message,
            services_t &_services);
    void insert_offer_service(std::shared_ptr<message_impl> _message,
                              service_t _service, instance_t _instance,
                              const std::shared_ptr<const serviceinfo> &_info);
    void insert_subscription(std::shared_ptr<message_impl> &_message,
            service_t _service, instance_t _instance, eventgroup_t _eventgroup,
            std::shared_ptr<subscription> &_subscription);
    void insert_subscription_ack(std::shared_ptr<message_impl> &_message,
            service_t _service, instance_t _instance, eventgroup_t _eventgroup,
            std::shared_ptr<eventgroupinfo> &_info, ttl_t _ttl);
    void insert_subscription_nack(std::shared_ptr<message_impl> &_message, service_t _service,
            instance_t _instance, eventgroup_t _eventgroup,
            std::shared_ptr<eventgroupinfo> &_info);

    void process_serviceentry(std::shared_ptr<serviceentry_impl> &_entry,
            const std::vector<std::shared_ptr<option_impl> > &_options);
    void process_offerservice_serviceentry(
            service_t _service, instance_t _instance, major_version_t _major,
            minor_version_t _minor, ttl_t _ttl,
            const boost::asio::ip::address &_reliable_address,
            uint16_t _reliable_port,
            const boost::asio::ip::address &_unreliable_address,
            uint16_t _unreliable_port);
    void send_unicast_offer_service(const std::shared_ptr<const serviceinfo>& _info,
                                    service_t _service, instance_t _instance,
                                    major_version_t _major,
                                    minor_version_t _minor);
    void process_findservice_serviceentry(service_t _service,
                                          instance_t _instance,
                                          major_version_t _major,
                                          minor_version_t _minor);
    void process_eventgroupentry(std::shared_ptr<eventgroupentry_impl> &_entry,
            const std::vector<std::shared_ptr<option_impl> > &_options);

    void handle_eventgroup_subscription(service_t _service,
            instance_t _instance, eventgroup_t _eventgroup,
            major_version_t _major, ttl_t _ttl,
            const boost::asio::ip::address &_reliable_address,
            uint16_t _reliable_port, uint16_t _unreliable_port);
    void handle_eventgroup_subscription_ack(service_t _service,
            instance_t _instance, eventgroup_t _eventgroup,
            major_version_t _major, ttl_t _ttl,
            const boost::asio::ip::address &_address, uint16_t _port);
    void serialize_and_send(std::shared_ptr<message_impl> _message,
            const boost::asio::ip::address &_address);

    void start_ttl_timer();
    ttl_t stop_ttl_timer();
    void check_ttl(const boost::system::error_code &_error);
    boost::asio::ip::address get_current_remote_address() const;
    bool check_static_header_fields(
            const std::shared_ptr<const message> &_message) const;
    void send_eventgroup_subscription_nack(service_t _service,
                                           instance_t _instance,
                                           eventgroup_t _eventgroup,
                                           major_version_t _major);
    bool check_layer_four_protocol(
            const std::shared_ptr<const ip_option_impl> _ip_option) const;
    void get_subscription_endpoints(subscription_type_e _subscription_type,
                                    std::shared_ptr<endpoint>& _unreliable,
                                    std::shared_ptr<endpoint>& _reliable,
                                    boost::asio::ip::address* _address,
                                    bool* _has_address,
                                    service_t _service, instance_t _instance,
                                    client_t _client) const;

private:
    boost::asio::io_service &io_;
    service_discovery_host *host_;

    boost::asio::ip::address unicast_;
    uint16_t port_;
    bool reliable_;
    std::shared_ptr<endpoint> endpoint_;

    std::shared_ptr<serializer> serializer_;
    std::shared_ptr<deserializer> deserializer_;

    std::shared_ptr<service_discovery_fsm> default_;
    std::map<std::string, std::shared_ptr<service_discovery_fsm> > additional_;

    requests_t requested_;
    std::mutex requested_mutex_;
    std::map<service_t,
            std::map<instance_t,
                    std::map<eventgroup_t, std::map<client_t, std::shared_ptr<subscription> > > > > subscribed_;
    std::mutex subscribed_mutex_;

    std::mutex serialize_mutex_;

    // Sessions
    std::map<boost::asio::ip::address, std::pair<session_t, bool> > sessions_;
    std::map<boost::asio::ip::address, session_t > sessions_receiving_;

    // Reboots
    std::set<boost::asio::ip::address> reboots_;

    // Runtime
    std::weak_ptr<runtime> runtime_;

    // TTL handling
    boost::asio::system_timer ttl_timer_;
    ttl_t smallest_ttl_;
    ttl_t ttl_;
};

}  // namespace sd
}  // namespace vsomeip

#endif // VSOMEIP_SERVICE_DISCOVERY_IMPL
