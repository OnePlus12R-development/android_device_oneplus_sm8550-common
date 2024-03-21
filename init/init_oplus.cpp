/*
 * Copyright (C) 2022-2023 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include <android-base/logging.h>
#include <android-base/properties.h>

#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>

using android::base::GetProperty;

/*
 * SetProperty does not allow updating read only properties and as a result
 * does not work for our use case. Write "OverrideProperty" to do practically
 * the same thing as "SetProperty" without this restriction.
 */
void OverrideProperty(const char* name, const char* value) {
    size_t valuelen = strlen(value);

    prop_info* pi = (prop_info*)__system_property_find(name);
    if (pi != nullptr) {
        __system_property_update(pi, value, valuelen);
    } else {
        __system_property_add(name, strlen(name), value, valuelen);
    }
}

/*
 * Only for read-only properties. Properties that can be wrote to more
 * than once should be set in a typical init script (e.g. init.oplus.hw.rc)
 * after the original property has been set.
 */
void vendor_load_properties() {
    auto hw_region_id = std::stoi(GetProperty("ro.boot.hw_region_id", "0"));
    auto prjname = std::stoi(GetProperty("ro.boot.prjname", "0"));

    switch (hw_region_id) {
        case 0: // aston IN/EU
            OverrideProperty("ro.product.product.model", "CPH2585");
            break;
        case 21:
            if (prjname == 22811) { // salami CN
                OverrideProperty("ro.product.device", "OP591BL1");
                OverrideProperty("ro.product.vendor.device", "OP591BL1");
                OverrideProperty("ro.product.product.model", "PHB110");
            } else if (prjname == 23801) { // aston CN
                OverrideProperty("ro.product.device", "OP5CF9L1");
                OverrideProperty("ro.product.vendor.device", "OP5CF9L1");
                OverrideProperty("ro.product.product.model", "PJE110");
            } else if (prjname == 22861) { // salami IN
                OverrideProperty("ro.product.product.model", "CPH2447");
            }
            break;
        case 22: // salami EU
            OverrideProperty("ro.product.product.model", "CPH2449");
            break;
        case 23:
            if (prjname == 23861) {
                OverrideProperty("ro.product.product.model", "CPH2611"); // aston NA
            } else {
                OverrideProperty("ro.product.product.model", "CPH2451"); // salami NA
            }
            break;
        default:
            LOG(ERROR) << "Unexpected region ID: " << hw_region_id;
    }
}
