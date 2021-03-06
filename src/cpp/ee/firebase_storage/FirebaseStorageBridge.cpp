//
//  FirebaseStorage.cpp
//  ee_x
//
//  Created by Zinge on 5/10/17.
//
//

#include "ee/firebase_storage/FirebaseStorageBridge.hpp"

#include <firebase/storage.h>

#include <ee/core/ScopeGuard.hpp>
#include <ee/firebase_core/FirebaseCoreBridge.hpp>

namespace ee {
namespace firebase {
namespace storage {
using Self = Bridge;

Self::Bridge() {
    initialized_ = false;
    fetching_ = false;
    storage_ = nullptr;
}

Self::~Bridge() {}

bool Self::initialize() {
    if (initialized_) {
        return true;
    }

    FirebaseCore::initialize();

    auto app = ::firebase::App::GetInstance();
    if (app == nullptr) {
        return false;
    }

    ::firebase::InitResult initResult;
    storage_ = ::firebase::storage::Storage::GetInstance(
        app, std::addressof(initResult));
    if (initResult != ::firebase::InitResult::kInitResultSuccess) {
        return false;
    }

    initialized_ = true;
    fetching_ = false;
    return true;
}

double Self::getMaxDownloadRetryTime() const {
    if (not initialized_) {
        return -1;
    }
    return storage_->max_download_retry_time();
}

double Self::getMaxUploadRetryTime() const {
    if (not initialized_) {
        return -1;
    }
    return storage_->max_upload_retry_time();
}

double Self::getMaxOperationRetryTime() const {
    if (not initialized_) {
        return -1;
    }
    return storage_->max_operation_retry_time();
}

void Self::setMaxDownloadRetryTime(double seconds) {
    if (not initialized_) {
        return;
    }
    storage_->set_max_download_retry_time(seconds);
}

void Self::setMaxOperationRetryTime(double seconds) {
    if (not initialized_) {
        return;
    }
    storage_->set_max_operation_retry_time(seconds);
}

void Self::setMaxUploadRetryTime(double seconds) {
    if (not initialized_) {
        return;
    }
    storage_->set_max_upload_retry_time(seconds);
}

void Self::getHash(const std::string& filePath, const HashCallback& callback) {
    auto guard = std::make_shared<ScopeGuard>(std::bind(callback, false, ""));
    if (not initialized_) {
        return;
    }
    auto file = storage_->GetReference(filePath.c_str());
    if (not file.is_valid()) {
        return;
    }
    file.GetMetadata().OnCompletion(
        [callback,
         guard](const ::firebase::Future<::firebase::storage::Metadata>& fut) {
            if (fut.error() != ::firebase::storage::kErrorNone) {
                return;
            }
            auto metadata = fut.result();
            if (not metadata->is_valid()) {
                return;
            }
            guard->dismiss();
            // MD5 is not supported yet, use updated_time.
            callback(true, std::to_string(metadata->updated_time()));
        });
}

void Self::getData(const std::string& filePath, const DataCallback& callback) {
    auto guard = std::make_shared<ScopeGuard>(std::bind(callback, false, ""));
    if (not initialized_) {
        return;
    }
    auto file = storage_->GetReference(filePath.c_str());
    if (not file.is_valid()) {
        return;
    }
    if (fetching_) {
        return;
    }
    fetching_ = true;
    file.GetBytes(std::addressof(buffer_.at(0)), buffer_.size())
        .OnCompletion([this, callback,
                       guard](const ::firebase::Future<std::size_t>& fut) {
            fetching_ = false;
            if (fut.error() != ::firebase::storage::kErrorNone) {
                return;
            }
            guard->dismiss();
            auto bytes = fut.result();
            assert(*bytes <= max_file_size_in_bytes);
            callback(true, std::string(std::addressof(buffer_.at(0)), *bytes));
        });
}
} // namespace storage
} // namespace firebase
} // namespace ee
