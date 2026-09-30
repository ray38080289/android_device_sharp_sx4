/*
 * SPDX-FileCopyrightText: The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Stand-in for the stock libAncDlmk.so (Megvii dense face landmarks), which
 * LineageOS cannot ship. libAncBeautify-hal.so links against it and calls
 * only this function; on a nonzero return it logs the error, leaves its API
 * handle NULL and skips landmark processing. The rest of the camera HAL is
 * unaffected.
 */

int _anc_dense_landmark_get_api_impl(void *api, int arg1, int arg2, int arg3, int arg4) {
    (void)api;
    (void)arg1;
    (void)arg2;
    (void)arg3;
    (void)arg4;
    return -1;
}
