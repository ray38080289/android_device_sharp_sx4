/*
 * SPDX-FileCopyrightText: The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package androidx.camera.extensions.impl;

import android.hardware.camera2.CaptureRequest;

/**
 * Sharp camera HAL features, switched on by vendor request keys. The HAL does all the
 * processing itself (anc_night multi-frame merge, backlight-detected HDR).
 */
enum SharpFeature {
    NIGHT(Keys.NIGHT),
    HDR(Keys.HDR),
    // What the stock camera sends for normal photos: the HAL picks night or HDR per scene
    AUTO(Keys.NIGHT, Keys.HDR);

    private static final int STAGE_ID = 1;

    private final CaptureRequest.Key<Byte>[] mKeys;

    @SafeVarargs
    SharpFeature(CaptureRequest.Key<Byte>... keys) {
        mKeys = keys;
    }

    CaptureStageImpl stage() {
        SettableCaptureStage stage = new SettableCaptureStage(STAGE_ID);
        for (CaptureRequest.Key<Byte> key : mKeys) {
            stage.addCaptureRequestParameters(key, (byte) 1);
        }
        return stage;
    }

    /** Stock enables its extension on every numeric camera id up to 10. */
    static boolean isAvailable(String cameraId) {
        try {
            return Integer.parseInt(cameraId) <= 10;
        } catch (NumberFormatException e) {
            return false;
        }
    }

    private static final class Keys {
        static final CaptureRequest.Key<Byte> NIGHT =
                new CaptureRequest.Key<>("com.sharp.camera.supernight.enable", Byte.class);
        static final CaptureRequest.Key<Byte> HDR =
                new CaptureRequest.Key<>("com.sharp.camera.hdr.enable", Byte.class);
    }
}
