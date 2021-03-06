/*
 Copyright (C) 2012-2015 Soomla Inc.

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

#ifndef COCOS2DXSTORE_CCSOOMLASTORECONFIGBUILDER_H
#define COCOS2DXSTORE_CCSOOMLASTORECONFIGBUILDER_H

#ifdef __cplusplus

#include <soomla/CCSoomlaConfigBuilder.h>

#include "soomla/config/CCSoomlaStoreGpConfigBuilder.h"
#include "soomla/config/CCSoomlaStoreIosConfigBuilder.h"

namespace soomla {
class CCSoomlaStoreConfigBuilder : public CCSoomlaConfigBuilder {
public:
    CCSoomlaStoreConfigBuilder();
    static CCSoomlaStoreConfigBuilder* create();
    CCSoomlaStoreConfigBuilder*
    setIosConfiguration(CCSoomlaStoreIosConfigBuilder* iosConfig);
    CCSoomlaStoreConfigBuilder*
    setGpConfiguration(CCSoomlaStoreGpConfigBuilder* gpConfig);
};
} // namespace soomla

#endif // __cplusplus

#endif // COCOS2DXSTORE_CCSOOMLASTORECONFIGBUILDER_H
