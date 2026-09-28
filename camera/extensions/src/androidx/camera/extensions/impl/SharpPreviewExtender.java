/*
 * SPDX-FileCopyrightText: The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package androidx.camera.extensions.impl;

import android.content.Context;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.params.SessionConfiguration;
import android.util.Pair;
import android.util.Size;

import java.util.List;

/** Preview for a {@link SharpFeature}: the key is sent on the repeating request. */
abstract class SharpPreviewExtender implements PreviewExtenderImpl {
    private final SharpFeature mFeature;

    SharpPreviewExtender(SharpFeature feature) {
        mFeature = feature;
    }

    @Override
    public boolean isExtensionAvailable(String cameraId, CameraCharacteristics chars) {
        return SharpFeature.isAvailable(cameraId);
    }

    @Override
    public void init(String cameraId, CameraCharacteristics chars) {}

    @Override
    public CaptureStageImpl getCaptureStage() {
        return mFeature.stage();
    }

    @Override
    public ProcessorType getProcessorType() {
        return ProcessorType.PROCESSOR_TYPE_REQUEST_UPDATE_ONLY;
    }

    @Override
    public ProcessorImpl getProcessor() {
        return RequestUpdateProcessorImpls.noUpdateProcessor();
    }

    @Override
    public List<Pair<Integer, Size[]>> getSupportedResolutions() {
        return null;
    }

    @Override
    public void onInit(String cameraId, CameraCharacteristics chars, Context context) {}

    @Override
    public void onDeInit() {}

    @Override
    public CaptureStageImpl onPresetSession() {
        return mFeature.stage();
    }

    @Override
    public CaptureStageImpl onEnableSession() {
        return mFeature.stage();
    }

    @Override
    public CaptureStageImpl onDisableSession() {
        return null;
    }

    @Override
    public int onSessionType() {
        return SessionConfiguration.SESSION_REGULAR;
    }
}
