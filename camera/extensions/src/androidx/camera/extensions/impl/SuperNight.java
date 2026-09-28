/*
 * SPDX-FileCopyrightText: The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package androidx.camera.extensions.impl;

import android.hardware.camera2.CaptureRequest;

/** Sharp HAL multi-frame night processing, as used by the stock extensions for this device. */
final class SuperNight {
    private static final CaptureRequest.Key<Byte> ENABLE =
            new CaptureRequest.Key<>("com.sharp.camera.supernight.enable", Byte.class);
    private static final int STAGE_ID = 1;

    private SuperNight() {}

    static CaptureStageImpl stage() {
        SettableCaptureStage stage = new SettableCaptureStage(STAGE_ID);
        stage.addCaptureRequestParameters(ENABLE, (byte) 1);
        return stage;
    }

    /** Stock enables night on every numeric camera id up to 10. */
    static boolean isAvailable(String cameraId) {
        try {
            return Integer.parseInt(cameraId) <= 10;
        } catch (NumberFormatException e) {
            return false;
        }
    }
}
