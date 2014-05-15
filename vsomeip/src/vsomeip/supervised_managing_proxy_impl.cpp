//
// supervised_managing_proxy_impl.cpp
//
// This file is part of the BMW Some/IP implementation.
//
// Copyright ������ 2013, 2014 Bayerische Motoren Werke AG (BMW).
// All rights reserved.
//

#include <vsomeip_internal/application_impl.hpp>
#include <vsomeip_internal/supervised_managing_proxy_impl.hpp>

namespace vsomeip {

supervised_managing_proxy_impl::supervised_managing_proxy_impl(application_base_impl &_owner)
	: managing_proxy_impl(_owner),
	  administration_proxy_impl(_owner),
	  proxy_base_impl(_owner),
	  log_user(_owner) {
}

supervised_managing_proxy_impl::~supervised_managing_proxy_impl() {
}

void supervised_managing_proxy_impl::init() {
	administration_proxy_impl::init();
	managing_proxy_impl::init();
}

void supervised_managing_proxy_impl::start() {
	administration_proxy_impl::start();
	managing_proxy_impl::start();
}

void supervised_managing_proxy_impl::stop() {
	managing_proxy_impl::stop();
	administration_proxy_impl::stop();
}

bool supervised_managing_proxy_impl::provide_service(service_id _service, instance_id _instance, const endpoint *_location) {
	bool is_successful = false;

	if (administration_proxy_impl::provide_service(_service, _instance, _location)) {
		if (managing_proxy_impl::provide_service(_service, _instance, _location)) {
			is_successful = true;
		} else {
			administration_proxy_impl::withdraw_service(_service, _instance, _location);
		}
	}

	return is_successful;
}

bool supervised_managing_proxy_impl::withdraw_service(service_id _service, instance_id _instance, const endpoint *_location) {
	bool is_successful = false;

	if (administration_proxy_impl::withdraw_service(_service, _instance, _location)) {
		if (managing_proxy_impl::withdraw_service(_service, _instance, _location)) {
			is_successful = true;
		} else {
			administration_proxy_impl::provide_service(_service, _instance, _location);
		}
	}

	return is_successful;
}

bool supervised_managing_proxy_impl::start_service(service_id _service, instance_id _instance) {
	bool is_successful = false;

	if (administration_proxy_impl::start_service(_service, _instance)) {
		if (managing_proxy_impl::start_service(_service, _instance)) {
			is_successful = true;
		} else {
			administration_proxy_impl::stop_service(_service, _instance);
		}
	}

	return is_successful;
}

bool supervised_managing_proxy_impl::stop_service(service_id _service, instance_id _instance) {
	bool is_successful = false;

	if (administration_proxy_impl::stop_service(_service, _instance)) {
		if (managing_proxy_impl::stop_service(_service, _instance)) {
			is_successful = true;
		} else {
			administration_proxy_impl::start_service(_service, _instance);
		}
	}

	return is_successful;
}

bool supervised_managing_proxy_impl::request_service(service_id _service, instance_id _instance, const endpoint *_location) {
	bool is_successful = false;

	if (administration_proxy_impl::request_service(_service, _instance, _location)) {
		if (managing_proxy_impl::request_service(_service, _instance, _location)) {
			is_successful = true;
		} else {
			administration_proxy_impl::release_service(_service, _instance);
		}
	}

	return is_successful;
}

bool supervised_managing_proxy_impl::release_service(service_id _service, instance_id _instance) {
	bool is_successful = false;

	const endpoint *location = find_client_location(_service, _instance);

	if (administration_proxy_impl::release_service(_service, _instance)) {
		if (managing_proxy_impl::release_service(_service, _instance)) {
			is_successful = true;
		} else {
			administration_proxy_impl::request_service(_service, _instance, location);
		}
	}

	return is_successful;
}

void supervised_managing_proxy_impl::register_method(service_id _service, instance_id _instance, method_id _method) {
}

void supervised_managing_proxy_impl::deregister_method(service_id _service, instance_id _instance, method_id _method) {
}

bool supervised_managing_proxy_impl::enable_magic_cookies(service_id _service, instance_id _instance) {
	return false;
}

bool supervised_managing_proxy_impl::disable_magic_cookies(service_id _service, instance_id _instance) {
	return false;
}

} // namespace vsomeip
